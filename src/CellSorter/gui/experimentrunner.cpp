#include "experimentrunner.h"
#include "ui_experimentrunner.h"

#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

ExperimentRunner::ExperimentRunner(Analyzer* analyzer, Setup setup, QWidget* parent)
    : QDialog(parent), ui(new Ui::ExperimentRunner), m_analyzer(analyzer) {
    ui->setupUi(this);

    // change "Abort" button to stop acquisition
    ui->buttonBox->button(QDialogButtonBox::Abort)->setText("Finish acquisition");

    ui->acqCount->setText(0);
    ui->dataextraction->setEnabled(false);

    // Setup gui update timer - used for querying analyzer for current progress and whether it is
    // done with acquisition
    m_acqTimer = new QTimer(this);
    m_acqTimer->setInterval(250);
    connect(m_acqTimer, &QTimer::timeout, this, &ExperimentRunner::acqTimerElapsed);
    m_time.start();
    m_acqTimer->start();

    // Setup future watchers
    connect(&m_acqFutureWatcher, &QFutureWatcher<void>::finished, this,
            &ExperimentRunner::acquisitionFinished);
    connect(&m_dataFutureWatcher, &QFutureWatcher<void>::finished, this,
            &ExperimentRunner::dataExtractionFinished);

    // Setup data extraction timer
    m_dataTimer = new QTimer(this);
    m_dataTimer->setInterval(250);
    connect(m_dataTimer, &QTimer::timeout, this, &ExperimentRunner::dataTimerElapsed);

    // Start the experiment
    QFuture<void> future = QtConcurrent::run(m_analyzer, &Analyzer::runAnalyzer, setup);
    m_acqFutureWatcher.setFuture(future);
}

ExperimentRunner::~ExperimentRunner() {
    delete ui;
}

void ExperimentRunner::acquisitionFinished() {
    // set progress bar to 100%
    ui->acqProgress->setMaximum(1);
    ui->acqProgress->setValue(1);
    // disable acquisition view and enable data extraction view
    ui->acq->setEnabled(false);
    ui->dataextraction->setEnabled(true);
    m_acqTimer->stop();  // dont update acquisition values anymore

    // Setup data extraction toolbar
    ui->dataProgress->setMaximum(m_analyzer->m_experiment.processed.size() - 1);

    // Start data extraction and start data timer
    QFuture<void> future = QtConcurrent::run(m_analyzer, &Analyzer::findObjects);
    m_dataFutureWatcher.setFuture(future);

    m_time.restart();
    m_dataTimer->start();
}

void ExperimentRunner::dataExtractionFinished() {
    // do stuff
    QMessageBox::information(this, "Experiment finished", "Good stuff, it worked");

    // Close dialog
    accept();
}

void ExperimentRunner::on_pushButton_2_clicked() {}

void ExperimentRunner::on_pushButton_clicked() {}

void ExperimentRunner::acqTimerElapsed() {
    int seconds = m_time.elapsed() / 1000;
    ui->elapsed->setText(QString::number(seconds));

    size_t acquiredImages = m_analyzer->m_experiment.processed.size();

    // Guesstimate current FPS rate
    int currentFps = acquiredImages / (seconds * 1.0);
    if (currentFps > 0) {
        ui->fps->setText(QString::number(currentFps));
    }
    // Set current image progress
    ui->acqCount->setText(QString::number(acquiredImages));
}

void ExperimentRunner::dataTimerElapsed() {
    // poll analyzer for current frame being processed.
    int currentFrame = m_analyzer->m_currentProcessingFrame;
    ui->dataProgress->setValue(currentFrame);
    ui->processImageN->setText(QString::number(currentFrame));
}

void ExperimentRunner::reject() {
    QString warning =
        "Are you sure you want to stop the experiment? Images will NOT be saved to the disk";
    if (QMessageBox::question(this, "Stop experiment", warning) == QMessageBox::Yes) {
        QDialog::reject();
    }
}

void ExperimentRunner::on_buttonBox_clicked(QAbstractButton* button) {
    if (button == ui->buttonBox->button(QDialogButtonBox::Abort)) {
        // Stop acquisition and run data processing
        m_analyzer->stopAnalyzer();
    } else if (button == ui->buttonBox->button(QDialogButtonBox::Cancel)) {
        this->reject();
    }
}
