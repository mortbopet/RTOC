#include "acquisitioninterface.h"

AcquisitionInterface::AcquisitionInterface() {}

cv::Mat& AcquisitionInterface::getNextImage(bool& successful) {
    switch (m_source) {
        case AcqSource::Camera: {
            break;
        }
        case AcqSource::Webcam: {
            break;
        }
        case AcqSource::Folder: {
            break;
        }
    }

    if (m_source == AcqSource::Camera) {
        // Implement for camera!
        return m_imageDisplayerWidget->getNextImage(successful);
    } else {
        return m_imageDisplayerWidget->getNextImage(successful);
    }
}

void AcquisitionInterface::reset() {
    m_imageDisplayerWidget->reset();
}
