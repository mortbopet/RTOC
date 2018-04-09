#include "mainwindow.h"
#include "lib/analyzer.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(Analyzer* analyzer, QWidget* parent)
    : m_analyzer(analyzer), QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_processInterface = new ProcessInterface(m_analyzer->getProcessContainerPtr());
    m_configurator = new Configurator(m_processInterface);

    ui->configLayout->addWidget(m_configurator);

#ifdef BUILD_ACQ
    m_acquisitionWdiget = new AcquisitionWidget(this);
    ui->acqLayout->addWidget(m_acquisitionWdiget);
#else
    // Create a dummy widget which informs the user that the acquisiton widget is not built into the
    // current version of the software
    QLabel* infoLabel = new QLabel();
    infoLabel->setText("This version does not include camera acquisiton functionality.");
    infoLabel->setAlignment(Qt::AlignCenter);
    ui->acqLayout->addWidget(infoLabel);
    ui->folderRadiobutton->setChecked(true);

    // Create warning messages if camera acquisiton is selected
    connect(ui->cameraRadiobutton, &QRadioButton::clicked, this,
            &MainWindow::cameraSelectedWithoutAcqBuilt);
#endif
}

void MainWindow::cameraSelectedWithoutAcqBuilt() {
    QMessageBox::warning(this, "Unsupported",
                         "This version of the software does not support image acquisiton through a "
                         "camera connection.");
    ui->folderRadiobutton->setChecked(true);
}

MainWindow::~MainWindow() {
    delete ui;
}
