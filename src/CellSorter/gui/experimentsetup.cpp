#include "experimentsetup.h"
#include "ui_experimentsetup.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QToolTip>
#include "../lib/datacontainer.h"

ExperimentSetup::ExperimentSetup(Analyzer* analyzer, QWidget* parent)
    : m_analyzer(analyzer), QWidget(parent), ui(new Ui::ExperimentSetup) {
    ui->setupUi(this);

    // setup tooltips
    ui->help->setIcon(QIcon(":/icons/resources/question.svg"));
    setToolTips();

    ui->run->setIcon(QIcon(":/icons/resources/play-button.svg"));

    setupDataOptions();

    // connect static checkboxes
    connect(ui->storeRaw, &QCheckBox::clicked, ui->rawPrefix, &QLineEdit::setEnabled);
    connect(ui->storeProcessed, &QCheckBox::clicked, ui->processedPrefix, &QLineEdit::setEnabled);

    // Only allow alphanumeric characters in lineedit
    QRegExpValidator* validator = new QRegExpValidator();
    validator->setRegExp(QRegExp(QString("\\S+")));
    ui->rawPrefix->setValidator(validator);
    ui->processedPrefix->setValidator(validator);
    ui->experimentName->setValidator(validator);
}

ExperimentSetup::~ExperimentSetup() {
    delete ui;
}

void ExperimentSetup::setToolTips() {
    connect(ui->help, &QPushButton::clicked,
            [=] { QToolTip::showText(QCursor::pos(), ui->help->toolTip()); });
    ui->help->setToolTip("<nobr>Hover over labels to receive tooltip</nobr> for the given action");
    ui->l_outputpath->setToolTip(
        "<nobr>Folder which all experiment related files</nobr> will be written to");
    ui->l_etype->setToolTip(
        "<nobr>Set the experiment type to match</nobr>set experiment-specific parameters");
    ui->l_fps->setToolTip(
        "<nobr>Frames per second. This value is used for both</nobr> bounding the rate at which "
        "the "
        "acquisition source is requested images, as well as being used to calculate the buffersize"
        "for writing the unsaved images to.");
    ui->l_rectime->setToolTip(
        "<nobr>The recording time is both used for calulating image buffersize</nobr> as well as "
        "automatically stopping the acquisition if below checkbox is not set");
    ui->contExec->setToolTip(
        "<nobr>If set, experiment will continue acquisition after </nobr> set execution time has "
        "elapsed. This can result in a reduction in acquisition rate, if images have to be written "
        "to the disk.");
    ui->experimentName->setToolTip(
        "Experiment name will be used in generation of folder & file names");
    ui->runProcessing->setToolTip(
        "<nobr>If set, the processing pipeline as specified in the </nobr> processing tab will be "
        "used in real-time when processing the images. Unsetting this variable can be used for "
        "only acquiring images from the camera, or if very high speed acquisition is desired "
        "(where real-time processing may bottleneck the image acquisition speed)");
}

bool ExperimentSetup::verifyCanRunExperiment() {
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
            // connect stuff here

            // Logic for inserting checkboxes into the layout correctly
            column++;
            if (column == columns) {
                column = 0;
                row++;
            }
        }
    }
}

void ExperimentSetup::on_groupBox_toggled(bool arg1) {}

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
        if (!setupExperimentDirectory())
            ;
    }
}

bool ExperimentSetup::setupExperimentDirectory() const {
    QDir expDir(m_experimentPath);
    if (expDir.exists()) {
        return false;
    } else {
        QDir().mkdir(m_experimentPath);
        QDir().mkdir(QDir(m_experimentPath).filePath(ui->rawPrefix->text()));
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
