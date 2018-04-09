#include "mainwindow.h"
#include "lib/analyzer.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(Analyzer* analyzer, QWidget* parent)
    : m_analyzer(analyzer), QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_processInterface = new ProcessInterface(m_analyzer->getProcessContainerPtr());
    m_configurator = new Configurator(m_processInterface);

    m_acquisitionWdiget = new AcquisitionWidget(this);

    ui->configLayout->addWidget(m_configurator);
    ui->acqLayout->addWidget(m_acquisitionWdiget);
}

MainWindow::~MainWindow() {
    delete ui;
}
