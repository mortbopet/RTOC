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
    connect(Acquisitor::get(), &Acquisitor::sendImageData, &m_imageDisplayer,
            &ImageDisplayer::setImage, Qt::QueuedConnection);

    // setup request timer to write '.' while awaiting answers from acquisitor
    m_acqWaitTimer.setInterval(150);
    connect(&m_acqWaitTimer, &QTimer::timeout, [=] { m_logger.writeToLog("."); });

    // set acquisitor logger
    connect(Acquisitor::get(), &Acquisitor::writeToLog, &m_logger, &Logger::writeLineToLog,
            Qt::QueuedConnection);

    // connect Acquisitor initializer
    connect(m_ui->initialize, &QPushButton::clicked, this, &MainWindow::initializeFramegrabber);

    connect(Acquisitor::get(), &Acquisitor::initialized, this,
            &MainWindow::setInitializerButtonState, Qt::QueuedConnection);
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
    connect(Acquisitor::get(), &Acquisitor::acquisitionStateChanged, this,
            &MainWindow::acquisitionStateChanged, Qt::QueuedConnection);
#endif
}

void MainWindow::setButtonStates() {
    switch (m_acqState) {
        case AcqState::idle: {
            // Acquisitor is no longer initialized/initialization failed
            m_ui->initialize->setEnabled(true);
            m_ui->initialize->setText("Initialize framegrabber");
            break;
        }
        case AcqState::initializing: {
            m_ui->initialize->setEnabled(false);
            m_ui->initialize->setText("Initializing...");
            break;
        }
        case AcqState::initialized: {
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

void MainWindow::setInitializerButtonState(bool state) {
    m_acqWaitTimer.stop();
    if (state) {
        m_acqState = AcqState::initialized;
    } else {
        m_acqState = AcqState::idle;
    }
    setButtonStates();
}

void MainWindow::acquisitionStateChanged(bool state) {
    if (state) {
        // Acquisition from camera is started, start image requests from ImageDisplayer
        m_imageDisplayer.startRequestingImages();
        m_acqState = AcqState::Acquiring;
    } else {
        m_imageDisplayer.stopRequestingImages();
        m_acqState = AcqState::initialized;
    }
    setButtonStates();
}

MainWindow::~MainWindow() {
    delete m_ui;
}

void MainWindow::initializeFramegrabber() {
    m_acqWaitTimer.start();
    // Start acquisitor initialization and disable button
    m_acqState = AcqState::initializing;
    QMetaObject::invokeMethod(Acquisitor::get(), "initialize", Qt::QueuedConnection);
    setButtonStates();
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
