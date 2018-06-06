#include "folderacquisition.h"


cv::Mat* FolderAcquisition::getNextImage(bool& successful) {
    successful = true;

    if (m_acqIndex < m_imageFileList.size()) {
        m_image = cv::imread(m_imageFileList[m_acqIndex++].absoluteFilePath().toStdString(),
                             cv::IMREAD_GRAYSCALE);
    } else {
        successful = false;
        return &m_image;
    }

    if (!m_image.data) {
        successful = false;
        m_acqIndex = 0;
    }

    return &m_image;
}


void FolderAcquisition::reset() {
    m_acqIndex = 0;
}

void FolderAcquisition::setPath(const QString& path) {
    m_dir.setPath(path);
    indexDirectory();
}

void FolderAcquisition::indexDirectory() {
    m_imageFileList.clear();

    // Set image filters
    QStringList filters;
    filters << "*.png"
            << "*.jpg"
            << "*.bmp";
    m_imageFileList = m_dir.entryInfoList(filters, QDir::Files);
    framefinder::sort_qfilelist(m_imageFileList);

    m_nImages = m_imageFileList.size();

    /*

    Then, Update gui!


    // Set slider range
    ui->imageSlider->setRange(1, m_nImages);

    ui->ips->setRange(1, m_nImages);

    // Load first image
    ui->imageSlider->setValue(1);
    on_imageSlider_sliderMoved(ui->imageSlider->value());

    // enable slider & play button
    ui->imageSlider->setEnabled(true);
    ui->play->setEnabled(true);

    */
}




