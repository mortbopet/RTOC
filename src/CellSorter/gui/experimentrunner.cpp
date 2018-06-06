#include "experimentrunner.h"
#include "ui_experimentrunner.h"

#include <QMessageBox>
#include <QPushButton>
#include <QTimer>

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

ExperimentRunner::ExperimentRunner(Analyzer* analyzer, Setup setup, QWidget* parent)
    : QDialog(parent), ui(new Ui::ExperimentRunner), m_setup(setup), m_analyzer(analyzer) {
    ui->setupUi(this);

    // change "Abort" button to stop acquisition
    ui->buttonBox->button(QDialogButtonBox::Abort)->setText("Finish acquisition");

    // Setup future watchers to trigger state switching
    connect(&m_acqFutureWatcher, &QFutureWatcher<void>::finished,
            [=] { stateChanged(State::GettingData); });
    connect(&m_getDataFutureWatcher, &QFutureWatcher<void>::finished,
            [=] { stateChanged(State::StoringData); });
    connect(&m_storeDataFutureWatcher, &QFutureWatcher<void>::finished,
            [=] { stateChanged(State::StoringImages); });
    connect(&m_storeImageFutureWatcher, &QFutureWatcher<void>::finished,
            [=] { stateChanged(State::Finished); });

    // Setup gui update timer - used for querying analyzer for current progress
    m_timer = new QTimer(this);
    m_timer->setInterval(250);
    connect(m_timer, &QTimer::timeout, this, &ExperimentRunner::guiUpdateTimerElapsed);
    m_time.start();
    m_timer->start();

    // Start the experiment
    stateChanged(State::Acquiring);
}

ExperimentRunner::~ExperimentRunner() {
    delete ui;
}

void ExperimentRunner::stateChanged(State state) {
    // Executes state switching logic. Works in conjunction with
    // Experimentrunner::guiUpdateTimerElapsed

    m_state = state;

    switch (m_state) {
        case State::Acquiring: {
            ui->infoLabel->setText("Acquiring images from source...");
            // Run the experiment
            QFuture<void> future = QtConcurrent::run(m_analyzer, &Analyzer::runAnalyzer, m_setup);
            m_acqFutureWatcher.setFuture(future);

            ui->acqCount->setText(0);
            ui->dataextraction->setEnabled(false);
            return;
        }
        case State::GettingData: {
            ui->infoLabel->setText("Extracting data from images...");

            // force acqProgressbar to 100%
            ui->acqProgress->setMaximum(1);
            ui->acqProgress->setValue(1);

            // disable acquisition view and enable data extraction view
            ui->acq->setEnabled(false);
            ui->dataextraction->setEnabled(true);

            // Setup data extraction toolbar
            ui->dataProgress->setMaximum(m_analyzer->m_experiment.processed.size() - 1);

            // Start data extraction
            QFuture<void> future = QtConcurrent::run(m_analyzer, &Analyzer::findObjects);
            m_getDataFutureWatcher.setFuture(future);
            return;
        }
        case State::StoringData: {
            ui->infoLabel->setText("Storing data to file...");
            // DATA GENERATION FINISHED
            // Export experiment data to file - should probably be async as to not block GUI
            ui->dataProgress->setMaximum(1);
            ui->dataProgress->setValue(1);

            std::string filename =
                QDir(QDir(QString::fromStdString(m_setup.outputPath))
                         .filePath(QString::fromStdString(m_setup.experimentName)))
                    .filePath("data.someFormat")
                    .toStdString();

            QFuture<void> future =
                QtConcurrent::run(m_analyzer, &Analyzer::exportExperiment, filename);
            m_storeDataFutureWatcher.setFuture(future);
            return;
        }
        case State::StoringImages: {
            // ACQUISITION FINISHED
            ui->infoLabel->setText("Storing images to disk...");

            // make sure that the actual value of the acquired images is written
            ui->acqCount->setText(QString::number(m_analyzer->acquiredImagesCnt()));

            ui->acqProgress->setMaximum(0);
            ui->acqProgress->setValue(0);

            // Start image acquisition. If asynchronous image writing has been enabled, the analyzer
            // is already storing images. calling writeImages(true) ensures that this call is
            // joining the ImageWriters with our execution - as in, we wait for the ImageWriters to
            // finish writing all images from their queues
            QFuture<void> future = QtConcurrent::run(m_analyzer, &Analyzer::writeImages, true);
            m_storeImageFutureWatcher.setFuture(future);
            return;
        }
        case State::Finished: {
            // force data progress bar to 100%
            ui->dataProgress->setMaximum(1);
            ui->dataProgress->setValue(1);

            ui->acqProgress->setMaximum(1);
            ui->acqProgress->setValue(1);

            ui->processImageN->setText(ui->acqCount->text());

            ui->infoLabel->setText("Finished experiment");
            // Change the button box to be in a "finalized" state, because we can only exit the
            // window now
            ui->buttonBox->button(QDialogButtonBox::Abort)->setText("Close window");
            ui->buttonBox->button(QDialogButtonBox::Cancel)->hide();

            // Check analyzer status
            checkAnalyzerStatusMessage(m_analyzer->getStatus());

            return;
        }
    }
}

void ExperimentRunner::guiUpdateTimerElapsed() {
    int ms = m_time.elapsed();
    if (m_state != State::Finished) {
        ui->elapsed->setText(QString::number(ms / 1000));
    }

    if (m_state == State::Acquiring) {
        size_t acquiredImages = m_analyzer->m_experiment.processed.size();

        // Guesstimate current FPS rate
        static int preTime = 0;
        static int preSize = 0;
        double currentFps = ((acquiredImages - preSize) * 1000.0) / (ms - preTime);
        preTime = ms;
        preSize = acquiredImages;

        if (currentFps > 0) {
            ui->fps->setText(QString::number(currentFps));
        }
        // Set current image progress
        ui->acqCount->setText(QString::number(acquiredImages));
    } else if (m_state == State::StoringImages) {
    } else if (m_state == State::GettingData) {
        int currentFrame = m_analyzer->m_currentProcessingFrame;
        ui->dataProgress->setValue(currentFrame);
        ui->processImageN->setText(QString::number(currentFrame));
    }
}

void ExperimentRunner::reject() {
    if (m_state != State::Finished) {
        QString warning =
            "Are you sure you want to stop the experiment? Images will NOT be saved to the disk";
        if (QMessageBox::question(this, "Stop experiment", warning) == QMessageBox::Yes) {
            // Stop analyzer
            m_analyzer->stopAnalyzer();
            QDialog::reject();
        }
    }
}

void ExperimentRunner::on_buttonBox_clicked(QAbstractButton* button) {
    if (m_state != State::Finished) {
        if (button == ui->buttonBox->button(QDialogButtonBox::Abort)) {
            // Stop acquisition and run data processing
            m_analyzer->stopAnalyzer();
        } else if (button == ui->buttonBox->button(QDialogButtonBox::Cancel)) {
            this->reject();
        }
    } else {
        // Experiment is finished, so we can only (successfully) close the window at this point
        accept();
    }
}

namespace {
#define CHECK_STATUS_BIT(status, bit, message)               \
    if (status & bit) {                                      \
        QMessageBox::information(0, "Information", message); \
    }
}

void ExperimentRunner::checkAnalyzerStatusMessage(const int status) const {
    CHECK_STATUS_BIT(status, StatusBits::UnknownError, "Unknown error");
    CHECK_STATUS_BIT(status, StatusBits::NoObjectsFound, "Could not find any objects");
}
