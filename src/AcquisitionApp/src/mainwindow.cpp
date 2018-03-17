#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "imagedisplayer.h"

#ifdef BUILD_ACQ
#include "acquisitor.h"
#endif

#include <QPushButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), m_ui(new Ui::MainWindow) {
    m_ui->setupUi(this);

    m_ui->graphicsView->setScene(new ImageDisplayer());

#ifdef BUILD_ACQ
    // connect Acquisitor initializer
    connect(m_ui->initialize, &QPushButton::clicked, Acquisitor::get(), &Acquisitor::initialize);
    connect(Acquisitor::get(), &Acquisitor::initialized, m_ui->start, &QPushButton::setEnabled);
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

MainWindow::~MainWindow() {
    delete m_ui;
}
