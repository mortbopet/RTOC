#include "experimentsetup.h"
#include "ui_experimentsetup.h"

#include <QCheckBox>
#include "../lib/datacontainer.h"

ExperimentSetup::ExperimentSetup(Analyzer* analyzer, QWidget* parent)
    : m_analyzer(analyzer), QWidget(parent), ui(new Ui::ExperimentSetup) {
    ui->setupUi(this);

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
