#include "mainwindow.h"

#include "imagedisplayer.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::MainWindow) {
    m_ui->setupUi(this);

    m_ui->graphicsView->setScene(new ImageDisplayer());
    m_logger.setLog(m_ui->log);

#ifdef BUILD_ACQ
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
            Acquisitor::get()->startAcq();
        } else {
            Acquisitor::get()->stopAcq();
        }
    });
    connect(Acquisitor::get(), &Acquisitor::acquisitionStateChanged, [=](bool state) {
        if (state) {
            m_ui->start->setText("Stop acquisition");
        } else {
            m_ui->start->setText("Start acquisition");
        };
    });
#endif
}

void MainWindow::setInitializerButtonState(bool state) {
    m_acqWaitTimer.stop();
    if (state) {
        // Acquisitor is initialized, disable button and set text
        m_ui->initialize->setEnabled(false);
        m_ui->initialize->setText("Initialized");
    } else {
        // Acquisitor is no longer initialized/initialization failed
        m_ui->initialize->setEnabled(true);
        m_ui->initialize->setText("Initialize framegrabber");
    }
}

MainWindow::~MainWindow() {
    delete m_ui;
}

void MainWindow::initializeFramegrabber() {
    m_acqWaitTimer.start();
    // Start acquisitor initialization and disable button
    if (m_ui->initWithConfig->isEnabled()) {
        // Load configuration file
    }
    m_ui->initialize->setEnabled(false);
    m_ui->initialize->setText("Initializing...");
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
