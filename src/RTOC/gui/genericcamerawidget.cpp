#include "genericcamerawidget.h"
#include "ui_genericcamerawidget.h"

GenericCameraWidget::GenericCameraWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::GenericCameraWidget) {
    ui->setupUi(this);
    m_open = m_vidcap.open(0);
    if (!m_open) {
        // Error handling
    }
    
    ui->image->setAlignment(Qt::AlignCenter);
    
    ui->play->setEnabled(false);
    
    
}

GenericCameraWidget::~GenericCameraWidget() {
    delete ui;
}

cv::Mat* GenericCameraWidget::getNextImage(bool &successful) {
    successful = true;

    if (m_open) {
        m_vidcap >> m_capture;
        cv::cvtColor(m_capture, m_image, CV_BGR2GRAY);
    } else {
        successful = false;
    }

    if (!m_image.data) {
        successful = false;
    }

    return &m_image;
}

void GenericCameraWidget::showImage() {
    bool success = false;
    getNextImage(success);
    if (success) {
        ui->image->setPixmap(QPixmap::fromImage(QImage(m_image.data, m_image.cols, m_image.rows, QImage::Format_Grayscale8)));
    }

}
