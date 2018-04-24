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
            [=] { stateChanged(State::StoringImages); });
    connect(&m_storeImageFutureWatcher, &QFutureWatcher<void>::finished,
            [=] { stateChanged(State::GettingData); });
    connect(&m_getDataFutureWatcher, &QFutureWatcher<void>::finished,
            [=] { stateChanged(State::StoringData); });
    connect(&m_storeDataFutureWatcher, &QFutureWatcher<void>::finished,
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
        case State::StoringImages: {
            // ACQUISITION FINISHED
            ui->infoLabel->setText("Storing images to disk...");

            // make sure that the actual value of the acquired images is written
            ui->acqCount->setText(QString::number(m_analyzer->m_experiment.processed.size()));

            // Start image storing (Always startet, but quickly terminates if user did not set any
            // store options

            // Calculate size of progress bar
            int nImages = 0;
            nImages = m_setup.storeProcessed ? m_analyzer->m_experiment.processed.size() + nImages
                                             : nImages;
            nImages =
                m_setup.storeRaw ? m_analyzer->m_experiment.rawBuffer.size() + nImages : nImages;

            ui->acqProgress->setMaximum(nImages);
            ui->acqProgress->setValue(0);
            QFuture<void> future = QtConcurrent::run(m_analyzer, &Analyzer::writeImages, m_setup);
            m_storeImageFutureWatcher.setFuture(future);
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
        case State::Finished: {
            // force data progress bar to 100%
            ui->dataProgress->setMaximum(1);
            ui->dataProgress->setValue(1);

            ui->infoLabel->setText("Finished experiment");
            // Change the button box to be in a "finalized" state, because we can only exit the
            // window now
            ui->buttonBox->button(QDialogButtonBox::Abort)->setText("Close window");
            ui->buttonBox->button(QDialogButtonBox::Cancel)->hide();
            return;
        }
    }
}

void ExperimentRunner::guiUpdateTimerElapsed() {
    int seconds = m_time.elapsed() / 1000;
    if (m_state != State::Finished) {
        ui->elapsed->setText(QString::number(seconds));
    }

    if (m_state == State::Acquiring) {
        size_t acquiredImages = m_analyzer->m_experiment.processed.size();

        // Guesstimate current FPS rate
        int currentFps = acquiredImages / (seconds * 1.0);
        if (currentFps > 0) {
            ui->fps->setText(QString::number(currentFps));
        }
        // Set current image progress
        ui->acqCount->setText(QString::number(acquiredImages));
    } else if (m_state == State::StoringImages) {
        int currentFrame = m_analyzer->m_currentProcessingFrame;
        ui->acqProgress->setValue(currentFrame);
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
