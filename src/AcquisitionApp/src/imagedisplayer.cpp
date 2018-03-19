#include "imagedisplayer.h"

#include <QDir>

ImageDisplayer::ImageDisplayer() {
    // Set image request interval
    m_imageRequester.setInterval(20);
    connect(&m_imageRequester, &QTimer::timeout, [=] { emit requestImage(); });
    addItem(&m_image);
}

void ImageDisplayer::startRequestingImages() {
    m_imageRequester.start();
}

void ImageDisplayer::stopRequestingImages() {
    m_imageRequester.stop();
}

void ImageDisplayer::setImage(const std::vector<char>& data) {
    int dim = floor(sqrt(data.size()));
    QImage img(reinterpret_cast<const uchar*>(data.data()), dim, dim, QImage::Format_Grayscale8);
    m_image.setPixmap(QPixmap::fromImage(img));
}
