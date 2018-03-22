#include "imagedisplayer.h"

#include <QDir>

ImageDisplayer::ImageDisplayer() {
    // Set image request interval
    setImageInterval(10);
    connect(&m_imageRequester, &QTimer::timeout, [=] { emit requestImage(); });
    addItem(&m_image);
}
void ImageDisplayer::setImageInterval(int freq) {
    m_imageRequester.setInterval(1.0 / (freq)*1000.0);
}

void ImageDisplayer::startRequestingImages() {
    m_imageRequester.start();
}

void ImageDisplayer::stopRequestingImages() {
    m_imageRequester.stop();
}

void ImageDisplayer::setImageDimensions(QPair<int, int> dim) {
    // A callback from Acquisitor is emitted whenever the camera/framegrabber dimensions have
    // changed
    m_dim = dim;
}

void ImageDisplayer::setImage(const std::vector<char>& data) {
    QImage img(reinterpret_cast<const uchar*>(data.data()), m_dim.first, m_dim.second,
               QImage::Format_Grayscale8);
    m_image.setPixmap(QPixmap::fromImage(img));
}
