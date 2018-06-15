#include "experimentsetup.h"
#include "ui_experimentrunner.h"
#include "ui_experimentsetup.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QThread>

#include "guihelpers.h"

#include "ui_experimentrunner.h"

ExperimentSetup::ExperimentSetup(Analyzer* analyzer, AcquisitionInterface* iface, QWidget* parent)
    : QWidget(parent), ui(new Ui::ExperimentSetup), m_analyzer(analyzer), m_interface(iface) {
    ui->setupUi(this);

    // Create data options
    setupDataOptions();

    ui->run->setIcon(QIcon(":/icons/resources/play-button.svg"));

    // connect static checkboxes
    connect(ui->storeRaw, &QCheckBox::clicked, ui->rawPrefix, &QLineEdit::setEnabled);
    connect(ui->storeProcessed, &QCheckBox::clicked, ui->processedPrefix, &QLineEdit::setEnabled);

    ui->storeImagesDuringExperimentLabel->setIcon(QIcon(":/icons/resources/question.svg"));
    ui->storeImagesDuringExperimentLabel->setToolTip(
        "<nobr>Images will be written to memory during experiment execution.</nobr> This is "
        "usefull if a long-running experiment will run, where the size of the acquired images are "
        "greater than the currently accessible amount of RAM. Warning: enabling this will most "
        "likely reduce fps drastically.");

    // Populate experiment types
    for (const auto& item : etDescriptors.keys()) {
        ui->experimentType->addItem(etDescriptors[item], QVariant::fromValue(item));
    }
    connect(ui->experimentType, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ExperimentSetup::updateCurrentSetup);

    // Only allow alphanumeric characters in lineedit
    QRegExpValidator* validator = new QRegExpValidator();
    validator->setRegExp(QRegExp(QString("\\S+")));
    ui->rawPrefix->setValidator(validator);
    ui->processedPrefix->setValidator(validator);
    ui->experimentName->setValidator(validator);

    ui->rectime->setRange(0, 999999999);

    connectWidgets();

    // Update current setup to load default GUI values
    updateCurrentSetup();
}

void ExperimentSetup::connectWidgets() {
    // Connect all editors to updateSetup
    connect(ui->storeImagesDuringExperiment, &QCheckBox::clicked, [=] { updateCurrentSetup(); });
    connect(ui->storeRaw, &QCheckBox::clicked, [=] { updateCurrentSetup(); });
    connect(ui->storeProcessed, &QCheckBox::clicked, [=] { updateCurrentSetup(); });
    connect(ui->experimentName, &QLineEdit::editingFinished, [=] { updateCurrentSetup(); });
    connect(ui->experimentPath, &QLineEdit::editingFinished, [=] { updateCurrentSetup(); });
    connect(ui->processedPrefix, &QLineEdit::editingFinished, [=] { updateCurrentSetup(); });
    connect(ui->rawPrefix, &QLineEdit::editingFinished, [=] { updateCurrentSetup(); });
    connect(ui->setAllFlags, &QPushButton::clicked, [=] { setAllDataFlagsState(true); });
    connect(ui->unsetAllFlags, &QPushButton::clicked, [=] { setAllDataFlagsState(false); });
    connect(ui->rectime, QOverload<int>::of(&QSpinBox::valueChanged),
            [=] { updateCurrentSetup(); });
    connect(ui->distanceThresholdInlet, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [=] { updateCurrentSetup(); });
    connect(ui->distanceThresholdPath, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            [=] { updateCurrentSetup(); });
    connect(ui->countThreshold, QOverload<int>::of(&QSpinBox::valueChanged),
            [=] { updateCurrentSetup(); });
    connect(ui->modelPath, &QLineEdit::textChanged, [=] { updateCurrentSetup(); });
}

ExperimentSetup::~ExperimentSetup() {
    delete ui;
}

void ExperimentSetup::updateCurrentSetup() {
    m_currentSetup.storeImagesDuringExperiment = ui->storeImagesDuringExperiment->isChecked();
    m_currentSetup.storeRaw = ui->storeRaw->isChecked();
    m_currentSetup.storeProcessed = ui->storeProcessed->isChecked();
    m_currentSetup.rawPrefix = ui->rawPrefix->text().toStdString();
    m_currentSetup.processedPrefix = ui->processedPrefix->text().toStdString();
    m_currentSetup.outputPath = ui->experimentPath->text().toStdString();
    m_currentSetup.modelPath = ui->modelPath->text().toStdString();
    m_currentSetup.experimentName = ui->experimentName->text().toStdString();
    m_currentSetup.outputPath = ui->experimentPath->text().toStdString();
    m_currentSetup.recordingTime = ui->rectime->value();
    m_currentSetup.countThreshold = ui->countThreshold->value();
    m_currentSetup.distanceThresholdInlet = ui->distanceThresholdInlet->value();
    m_currentSetup.distanceThresholdPath = ui->distanceThresholdPath->value();

    m_currentSetup.extractData = false;
    m_currentSetup.runProcessing = true;
    switch (ui->experimentType->currentData(Qt::UserRole).value<ExperimentTypes>()) {
        case ExperimentTypes::Acquisition: {
            ui->storeProcessed->setChecked(false);
            m_currentSetup.storeProcessed = false;  // can't store images when we haven't made them!
            m_currentSetup.runProcessing = false;
            break;
        }
        case ExperimentTypes::AcquisitionAndProcessing: {
            break;
        }
        case ExperimentTypes::AcquisitionAndExtraction: {
            m_currentSetup.extractData = true;
            break;
        }
        case ExperimentTypes::AcquisitionAndRealTimeID: {
            m_currentSetup.extractData = true;
            break;
        }
    }
}

bool ExperimentSetup::verifyCanRunExperiment() {
    // Check whether the acquisitor has a valid source and is ready
    bool success = false;
    m_interface->reset();
    m_interface->getNextImage(success);
    m_interface->reset();
    if (!success) {
        QMessageBox::warning(
            this, "Error",
            QString("Could not retrieve image from acquisition source. Has a valid acquisition "
                    "source been set up in the Acquisition tab?"));
        return false;
    }

    // Check whether an experiment directory and name has been set
    if (ui->experimentName->text().isEmpty() || ui->experimentPath->text().isEmpty()) {
        QMessageBox::warning(this, "Error", QString("Please set a valid experiment name and path"));
        return false;
    }

    // check whether valid image prefixes have been set
    if (ui->storeRaw->isChecked() && ui->rawPrefix->text().isEmpty()) {
        QMessageBox::warning(this, "Error", QString("Please set a prefix for raw images"));
        return false;
    }
    if (ui->storeProcessed->isChecked() && ui->processedPrefix->text().isEmpty()) {
        QMessageBox::warning(this, "Error", QString("Please set a prefix for processed images"));
        return false;
    }

    if (ui->storeRaw->isChecked() && ui->storeProcessed->isChecked() &&
        (ui->rawPrefix->text() == ui->processedPrefix->text())) {
        QMessageBox::warning(
            this, "Error",
            QString("Please set different names for processed and raw image prefixes"));
        return false;
    }

    // Setup specific setup
    switch (ui->experimentType->currentData(Qt::UserRole).value<ExperimentTypes>()) {
        case ExperimentTypes::Acquisition: {
            break;
        }
        case ExperimentTypes::AcquisitionAndProcessing: {
            break;
        }
        case ExperimentTypes::AcquisitionAndExtraction: {
            break;
        }
        case ExperimentTypes::AcquisitionAndRealTimeID: {
            if (ui->modelPath->text().isEmpty()) {
                QMessageBox::warning(this, "Error", QString("No model path set"));
                return false;
            }
            break;
        }
    }

    return true;
}

void ExperimentSetup::setupDataOptions() {
    // load data options as defined in datacontainer.h
    const int columns = 2;  // number of columns in layout
    int column = 0;
    int row = 0;
    for (const auto& option : data::guiMap) {
        if (std::get<0>(option.first) == 1) {
            QCheckBox* checkbox = new QCheckBox(QString::fromStdString(option.second));
            ui->dataLayout->addWidget(checkbox, row, column);
            m_dataOptionCheckboxes.append(checkbox);

            // Create connector slot for each checkbox which sets its corresponding bit in dataFlags
            // to the state reported by QCheckBox::toggled
            connect(checkbox, &QCheckBox::toggled, [=](bool state) {
                m_currentSetup.dataFlags ^=
                    (-state ^ m_currentSetup.dataFlags) & std::get<1>(option.first);
            });

            // Logic for inserting checkboxes into the layout correctly
            column++;
            if (column == columns) {
                column = 0;
                row++;
            }
        }
    }
}

void ExperimentSetup::on_run_clicked() {
    if (verifyCanRunExperiment()) {
        m_experimentPath = QDir(ui->experimentPath->text()).filePath(ui->experimentName->text());
        // Run the experiment
        if (!setupExperimentDirectory()) {
            QMessageBox::warning(
                this, "Error",
                QString("Could not create required output files. does directory: %1 "
                        "already exist?")
                    .arg(m_experimentPath));
            return;
        }
        // Update setup - just to be sure that we have the latest changes
        updateCurrentSetup();

        // we can run the experiment
        ExperimentRunner runner(m_analyzer, m_currentSetup);
        runner.ui->experimentName->setText(ui->experimentName->text());
        runner.exec();
    }
}

bool ExperimentSetup::setupExperimentDirectory() const {
    QDir expDir(m_experimentPath);
    if (expDir.exists()) {
        return false;
    } else {
        QDir().mkdir(m_experimentPath);
        if (ui->storeRaw->isChecked())
            QDir().mkdir(QDir(m_experimentPath).filePath(ui->rawPrefix->text()));
        if (ui->storeProcessed->isChecked())
            QDir().mkdir(QDir(m_experimentPath).filePath(ui->processedPrefix->text()));
    }
    return true;
}

void ExperimentSetup::on_setExperimentPath_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, ("Experiment output directory"), ".",
                                                    QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()) {
        ui->experimentPath->setText(dir);
    }
}

template <class Archive>
void ExperimentSetup::serialize(Archive& ar, const unsigned int version) {
    SERIALIZE_CHECKBOX(ar, ui->storeRaw, storeRaw);
    SERIALIZE_CHECKBOX(ar, ui->storeProcessed, storeProcessed);
    SERIALIZE_COMBOBOX(ar, ui->experimentType, experimentType);
    SERIALIZE_LINEEDIT(ar, ui->experimentName, ExperimentName);
    SERIALIZE_LINEEDIT(ar, ui->experimentPath, ExperimentPath);
    SERIALIZE_LINEEDIT(ar, ui->modelPath, ModelPath);
    SERIALIZE_LINEEDIT(ar, ui->processedPrefix, ProcessedPrefix);
    SERIALIZE_LINEEDIT(ar, ui->rawPrefix, RawPrefix);
    SERIALIZE_SPINBOX(ar, ui->rectime, recordingTime);
    SERIALIZE_SPINBOX(ar, ui->distanceThresholdInlet, distanceThresholdInlet);
    SERIALIZE_SPINBOX(ar, ui->distanceThresholdPath, distanceThresholdPath);
    SERIALIZE_SPINBOX(ar, ui->countThreshold, countThreshold);
    for (auto dataOption : ui->extractData->findChildren<QCheckBox*>()) {
        bool v = dataOption->isChecked();
        QString name = dataOption->text();
        name.replace(' ', '_');
        ar& boost::serialization::make_nvp(name.toStdString().c_str(), v);
        dataOption->setChecked(v);
    }

    SERIALIZE_CHECKBOX(ar, ui->inletOutletIsSet, inletOutletIsSet);
    ar& boost::serialization::make_nvp("m_currentSetup", m_currentSetup);
}

EXPLICIT_INSTANTIATE_XML_ARCHIVE(ExperimentSetup)

void ExperimentSetup::on_experimentType_currentIndexChanged(int index) {
    const auto type = ui->experimentType->currentData(Qt::UserRole).value<ExperimentTypes>();
    ui->setModelPath->setEnabled(false);
    ui->modelPath->setEnabled(false);

    if (type == ExperimentTypes::Acquisition) {
        // disable storeProcessed stuff
        ui->storeProcessed->setEnabled(false);
        ui->processedPrefix->setEnabled(false);
    } else {
        ui->storeProcessed->setEnabled(true);
        ui->processedPrefix->setEnabled(true);
    }

    if (type == ExperimentTypes::Acquisition || type == ExperimentTypes::AcquisitionAndProcessing) {
        // disable extract data options
        ui->extractData->setEnabled(false);
        return;
    } else {
        ui->extractData->setEnabled(true);
    }

    if (type == ExperimentTypes::AcquisitionAndRealTimeID) {
        ui->setModelPath->setEnabled(true);
        ui->modelPath->setEnabled(true);
    }
}

void ExperimentSetup::setAllDataFlagsState(bool state) {
    for (const auto& option : m_dataOptionCheckboxes)
        option->setChecked(state);
    update();
}

void ExperimentSetup::on_setOutlet_clicked() {
    // Get an image from the acquisitor
    // Check whether the acquisitor has a valid source and is ready
    bool success = false;
    m_interface->reset();
    auto img = m_interface->getNextImage(success);
    m_interface->reset();
    if (!success) {
        QMessageBox::warning(
            this, "Error",
            QString("Could not retrieve image from acquisition source. Has a valid acquisition "
                    "source been set up in the Acquisition tab?"));
        return;
    }

    //
    QImage image = QImage(img.data, img.cols, img.rows, QImage::Format_Grayscale8);
    InletOutletWidget w(image);

    if (ui->inletOutletIsSet->isChecked()) {
        // load values
        w.load(m_currentSetup.inlet, true);
        w.load(m_currentSetup.outlet, false);
    }

    if (w.exec() == QDialog::Accepted) {
        ui->inletOutletIsSet->setChecked(true);
        m_currentSetup.inlet = std::pair<int, int>(w.inletX, w.inletY);
        m_currentSetup.outlet = std::pair<int, int>(w.outletX, w.outletY);
    };
}

void ExperimentSetup::on_setModelPath_clicked() {
    auto filename = QFileDialog::getOpenFileName(this, "Open model file", QDir::currentPath(),
                                                 "txt file (*.txt)");
    if (!filename.isNull())
        ui->modelPath->setText(filename);
}
