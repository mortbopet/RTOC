#include "cameradisplayerwidget.h"

#include <QDir>

CameraDisplayerWidget::CameraDisplayerWidget() {
    // Set shape mode of image - may yield a bit of performance
    // http://doc.qt.io/archives/qt-4.8/qgraphicspixmapitem.html#ShapeMode-enum
    m_image.setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

    // Set image request interval
    setImageInterval(10);
    connect(&m_imageRequester, &QTimer::timeout, this, &CameraDisplayerWidget::tryRequestImage);
    addItem(&m_image);
}
void CameraDisplayerWidget::setImageInterval(int freq) {
    m_imageRequester.setInterval(1.0 / (freq)*1000.0);
}

void CameraDisplayerWidget::startRequestingImages() {
    m_imageRequester.start();
}

void CameraDisplayerWidget::stopRequestingImages() {
    m_imageRequester.stop();
}

void CameraDisplayerWidget::tryRequestImage() {
    // Checks whether an image is currently being generated from the Acquisitor memory - if true,
    // then no reason to contact acquisitor for a new image
    if (!m_settingImage) {
        emit requestImage();
    }
}

void CameraDisplayerWidget::setImageDimensions(QPair<int, int> dim) {
    // A callback from Acquisitor is emitted whenever the camera/framegrabber dimensions have
    // changed
    m_dim = dim;
}

void CameraDisplayerWidget::setImage(const std::vector<char>& data) {
    m_settingImage = true;
    m_image.setPixmap(
        QPixmap::fromImage(QImage(reinterpret_cast<const uchar*>(data.data()), m_dim.first,
                                  m_dim.second, QImage::Format_Grayscale8)));
    m_settingImage = false;
}
