#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::MainWindow) {
    m_ui->setupUi(this);

    // connect image request from ImageDisplayer

    m_ui->graphicsView->setScene(&m_imageDisplayer);

    m_logger.setLog(m_ui->log);

#ifdef BUILD_ACQ

    // Setup ImageDisplayer requests to the acquisitor
    connect(&m_imageDisplayer, &ImageDisplayer::requestImage, Acquisitor::get(),
            &Acquisitor::requestImageData, Qt::QueuedConnection);

    // connect Acquisitor to UI
    connect(Acquisitor::get(), &Acquisitor::sendImageData, &m_imageDisplayer,
            &ImageDisplayer::setImage, Qt::QueuedConnection);
    connect(Acquisitor::get(), &Acquisitor::writeToLog, &m_logger, &Logger::writeLineToLog,
            Qt::QueuedConnection);
    connect(Acquisitor::get(), &Acquisitor::stateChanged, this, &MainWindow::acqStateChanged,
            Qt::QueuedConnection);

    // setup request timer to write '.' while awaiting answers from acquisitor
    m_acqWaitTimer.setInterval(150);
    connect(&m_acqWaitTimer, &QTimer::timeout, [=] { m_logger.writeToLog("."); });

    // connect Acquisitor initializer
    connect(m_ui->initialize, &QPushButton::clicked, this, &MainWindow::initializeFramegrabber);

    // Disable start-acq button by default
    m_ui->start->setEnabled(false);

    // Connect start functionality for frame grabber
    connect(m_ui->start, &QPushButton::toggled, [=](bool state) {
        if (state) {
            QMetaObject::invokeMethod(Acquisitor::get(), "startAcq", Qt::QueuedConnection);
        } else {
            QMetaObject::invokeMethod(Acquisitor::get(), "stopAcq", Qt::QueuedConnection);
        }
    });

#endif
}

void MainWindow::acqStateChanged(AcqState state) {
    switch (state) {
        case AcqState::Idle: {
            m_acqWaitTimer.stop();
            m_imageDisplayer.stopRequestingImages();
            break;
        }
        case AcqState::Initializing: {
            m_acqWaitTimer.start();
            break;
        }
        case AcqState::Initialized: {
            m_acqWaitTimer.stop();
            break;
        }
        case AcqState::Acquiring: {
            m_imageDisplayer.startRequestingImages();
            break;
        }
    }

    // set UI button states
    setButtonStates(state);
}

void MainWindow::setButtonStates(AcqState state) {
    switch (state) {
        case AcqState::Idle: {
            // Acquisitor is no longer initialized/initialization failed
            m_ui->initialize->setEnabled(true);
            m_ui->initialize->setText("Initialize framegrabber");
            break;
        }
        case AcqState::Initializing: {
            m_ui->initialize->setEnabled(false);
            m_ui->initialize->setText("Initializing...");
            break;
        }
        case AcqState::Initialized: {
            // Acquisitor is initialized, disable button and set text
            m_ui->initialize->setEnabled(false);
            m_ui->initialize->setText("Initialized");

            m_ui->start->setEnabled(true);
            m_ui->start->setText("Start acquisition");

            break;
        }
        case AcqState::Acquiring: {
            m_ui->start->setText("Stop acquisition");
            break;
        }
    }
}

MainWindow::~MainWindow() {
    delete m_ui;
}

void MainWindow::initializeFramegrabber() {
    // Start acquisitor initialization and disable button
    QMetaObject::invokeMethod(Acquisitor::get(), "initialize", Qt::QueuedConnection);
}

void MainWindow::on_actionExit_triggered() {
    QApplication::exit();
}

void MainWindow::on_filePathButton_clicked() {
    auto filename =
        QFileDialog::getOpenFileName(this, "Open configuration file", "", "XML file (*.xml)");
    if (!filename.isNull()) {
        m_ui->configPath->setText(filename);
    }
}
