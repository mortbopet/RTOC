#include "genericcamerawidget.h"
#include "ui_genericcamerawidget.h"

GenericCameraWidget::GenericCameraWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GenericCameraWidget)
{
    ui->setupUi(this);
}

GenericCameraWidget::~GenericCameraWidget()
{
    delete ui;
}
