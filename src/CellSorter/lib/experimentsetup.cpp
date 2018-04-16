#include "experimentsetup.h"
#include "ui_experimentsetup.h"

ExperimentSetup::ExperimentSetup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExperimentSetup)
{
    ui->setupUi(this);
}

ExperimentSetup::~ExperimentSetup()
{
    delete ui;
}
