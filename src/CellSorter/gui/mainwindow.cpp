#include "mainwindow.h"
#include "guihelpers.h"
#include "lib/analyzer.h"
#include "ui_mainwindow.h"

#include <QCursor>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QToolTip>

MainWindow::MainWindow(Analyzer* analyzer, QWidget* parent)
    : m_analyzer(analyzer), QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_processInterface = new ProcessInterface(m_analyzer);
    m_configurator = new Configurator(m_processInterface);
    ui->configLayout->addWidget(m_configurator);

    ui->ffhelp->setIcon(QIcon(":/icons/resources/question.svg"));
    ui->ffhelp->setToolTip(
        "<nobr>Enabling frame finder will reduce the number</nobr> of images which are processed. "
        "Frame "
        "finder will do continuous acquisiton from the acquistion source, and only report images "
        "back to the processing setup, when a sufficient change in the image has been detected. "
        "This is useful when recording setups where the image may be static for long periods of "
        "time.");

    // Setup framefinder
    connect(ui->ffhelp, &QPushButton::clicked,
            [=] { QToolTip::showText(QCursor::pos(), ui->ffhelp->toolTip()); });
    ui->fftresh->setRange(0, 255);
    connect(ui->fftresh, &QSpinBox::editingFinished,
            [=] { m_acqInterface->setFFThresh(ui->fftresh->value()); });

    // Add ImageDisplayerWidget
    m_imageDisplayerWidget = new ImageDisplayerWidget();
    ui->imageLayout->addWidget(m_imageDisplayerWidget);

    // setup acquisition source combobox
    setupAcqCombobox();

    // Bind ("connect") analyzer and AcquisitionInterface through the imageGetterFunction
    m_acqInterface = new AcquisitionInterface(m_imageDisplayerWidget);
    analyzer->setImageGetterFunction(
        [=](bool& successful) -> cv::Mat& { return m_acqInterface->getNextImage(successful); });

    // connect run analyzer button to analyzer
    ui->runAnalyzer->setIcon(QIcon(":/icons/resources/play-button.svg"));
    connect(ui->runAnalyzer, &QPushButton::clicked, [=] { m_analyzer->runAnalyzer(); });

    // Connect frame finder checkbox to acquisitionInterface
    connect(ui->enableFF, &QCheckBox::stateChanged, this, &MainWindow::ffStateChanged);

#ifdef BUILD_ACQ
    m_acquisitionWdiget = new AcquisitionWidget(this);
    ui->acqLayout->addWidget(m_acquisitionWdiget);
#endif
}

void MainWindow::acqSelectionChanged(int index) {
    // Decode source value
    AcqSource source = ui->acqSource->itemData(index).value<AcqSource>();
    switch (source) {
        case AcqSource::Camera: {
#ifdef BUILD_ACQ
            ui->acqWidgets->setCurrentIndex(3);
#else
            // Display error message
            cameraSelectedWithoutAcqBuilt();
#endif
            break;
        }
        case AcqSource::Webcam: {
            ui->acqWidgets->setCurrentIndex(1);
            break;
        }
        case AcqSource::Folder: {
            ui->acqWidgets->setCurrentIndex(0);
            break;
        }
    }
}

void MainWindow::ffStateChanged(int state) {
    m_acqInterface->setFFState(state);
    ui->fftresh->setEnabled(state);
    ui->fftreshlabel->setEnabled(state);
}

void MainWindow::setupAcqCombobox() {
    ui->acqSource->addItem("Image folder", QVariant::fromValue(AcqSource::Folder));
    ui->acqSource->addItem("Generic camera", QVariant::fromValue(AcqSource::Webcam));
    ui->acqSource->addItem("microEnable 5 ironman AQ8-CXP6D",
                           QVariant::fromValue(AcqSource::Camera));

    connect(ui->acqSource, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &MainWindow::acqSelectionChanged);

    connect(ui->acqSource, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
        AcqSource source = ui->acqSource->itemData(index).value<AcqSource>();
        m_acqInterface->setAcqSourceState(source);
    });

    ui->acqSource->setCurrentIndex(
        GetIndexOfItem(*ui->acqSource, QVariant::fromValue(AcqSource::Folder)));
}

void MainWindow::cameraSelectedWithoutAcqBuilt() {
    QMessageBox::warning(this, "Unsupported",
                         "This version of the software does not support image acquisiton through a "
                         "camera connection.");

    ui->acqSource->setCurrentIndex(
        GetIndexOfItem(*ui->acqSource, QVariant::fromValue(AcqSource::Folder)));
}

MainWindow::~MainWindow() {
    delete ui;
}
