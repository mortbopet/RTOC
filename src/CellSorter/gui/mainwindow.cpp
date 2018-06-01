#include "mainwindow.h"
#include "guihelpers.h"
#include "lib/analyzer.h"
#include "ui_mainwindow.h"

#include <QCursor>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QThread>
#include <QToolTip>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Create analyzer and move it to separate thread
    QThread* analyzerThread = new QThread();
    m_analyzer = new Analyzer();
    m_analyzer->moveToThread(analyzerThread);
    analyzerThread->start();

    // Add ImageDisplayerWidget
    m_imageDisplayerWidget = new ImageDisplayerWidget();
    ui->imageLayout->addWidget(m_imageDisplayerWidget);

    // Bind ("connect") analyzer and AcquisitionInterface through the imageGetterFunction
    m_acqInterface = new AcquisitionInterface(m_imageDisplayerWidget);
    m_analyzer->setImageGetterFunction(
        [=](bool& successful) -> cv::Mat& { return m_acqInterface->getNextImage(successful); });
    m_acqInterface->moveToThread(analyzerThread);

    // create various objects
    m_processInterface = new ProcessInterface(m_analyzer);
    m_configurator = new Configurator(m_processInterface, m_analyzer);
    m_experimentSetup = new ExperimentSetup(m_analyzer, m_acqInterface);
    ui->configLayout->addWidget(m_configurator);
    ui->experimentLayout->addWidget(m_experimentSetup);

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

    // setup acquisition source combobox
    setupAcqCombobox();

    // Connect frame finder checkbox to acquisitionInterface
    connect(ui->enableFF, &QCheckBox::stateChanged, this, &MainWindow::ffStateChanged);

#ifdef BUILD_ACQ
    m_acquisitionWdiget = new IronManWidget(this);
    ui->acqLayout->addWidget(m_acquisitionWdiget);

    m_acqInterface->setAcquisitor(Acquisitor::get());
    connect(Acquisitor::get(), &Acquisitor::imageDimensionsChanged,
            [=](QPair<int, int> p) { m_acqInterface->setIronManDimensions(p); });
#endif
}

void MainWindow::acqSelectionChanged(int index) {
    // Decode source value
    AcqSource source = ui->acqSource->itemData(index).value<AcqSource>();
    switch (source) {
        case AcqSource::IronManCamera: {
#ifdef BUILD_ACQ
            ui->acqWidgets->setCurrentIndex(2);
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
                           QVariant::fromValue(AcqSource::IronManCamera));

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

bool MainWindow::loadProject(const QString& path) {
    try {
        std::ifstream ifs(path.toStdString());
        {
            boost::archive::xml_iarchive ia(ifs);
            ia >> BOOST_SERIALIZATION_NVP(m_analyzer);
            ia >> BOOST_SERIALIZATION_NVP(m_experimentSetup);
        }
        ifs.close();
    } catch (...) {
        return false;
    }
    return true;
}

bool MainWindow::storeProject(const QString& path) {
    // Serialize current processes in m_processes in .xml file
    try {
        std::ofstream ofs(path.toStdString());
        {
            boost::archive::xml_oarchive oa(ofs);
            oa << BOOST_SERIALIZATION_NVP(m_analyzer);
            oa << BOOST_SERIALIZATION_NVP(m_experimentSetup);
        }
        ofs.close();
    } catch (...) {
        return false;
    }
    return true;
}
