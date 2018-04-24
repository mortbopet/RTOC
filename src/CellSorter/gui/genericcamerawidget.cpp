#include "genericcamerawidget.h"
#include "ui_genericcamerawidget.h"

GenericCameraWidget::GenericCameraWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::GenericCameraWidget) {
    ui->setupUi(this);
}

GenericCameraWidget::~GenericCameraWidget() {
    delete ui;
}

cv::Mat* GenericCameraWidget::getNextImage(bool &successful) {
    cv::Mat img;
    successful = true;

    if (m_vidcap.open(0)) {
        m_vidcap >> img;
        cv::cvtColor(img, m_image, CV_BGR2GRAY);
    } else {
        successful = false;
    }

    if (!m_image.data) {
        successful = false;
    }

    return &m_image;
}
