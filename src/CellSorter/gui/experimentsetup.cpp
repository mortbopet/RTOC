#include "experimentsetup.h"
#include "ui_experimentsetup.h"

#include <QCheckBox>
#include <QToolTip>
#include "../lib/datacontainer.h"

ExperimentSetup::ExperimentSetup(Analyzer* analyzer, QWidget* parent)
    : m_analyzer(analyzer), QWidget(parent), ui(new Ui::ExperimentSetup) {
    ui->setupUi(this);

    // setup tooltips
    ui->help->setIcon(QIcon(":/icons/resources/question.svg"));
    setToolTips();

    setupDataOptions();

    // connect static checkboxes
    connect(ui->storeRaw, &QCheckBox::clicked, ui->rawPrefix, &QLineEdit::setEnabled);
    connect(ui->storeProcessed, &QCheckBox::clicked, ui->processedPrefix, &QLineEdit::setEnabled);

    // Only allow alphanumeric characters in lineedit
    QRegExpValidator* validator = new QRegExpValidator();
    validator->setRegExp(QRegExp(QString("\\S+")));
    ui->rawPrefix->setValidator(validator);
    ui->processedPrefix->setValidator(validator);
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
}

void ExperimentSetup::setupDataOptions() {
    // load data options as defined in datacontainer.h
    const int columns = 2;  // number of columns in layout
    int column = 0;
    int row = 0;
    for (const auto& option : data::guiMap) {
        QCheckBox* checkbox = new QCheckBox(QString::fromStdString(option.second));
        ui->dataLayout->addWidget(checkbox, row, column);
        // connect stuff here

        // Logic for inserting into the layout correctly
        column++;
        if (column == columns) {
            column = 0;
            row++;
        }
    }
}

void ExperimentSetup::on_groupBox_toggled(bool arg1) {}
