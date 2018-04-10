#include "acquisitioninterface.h"

AcquisitionInterface::AcquisitionInterface() {}

cv::Mat& AcquisitionInterface::getNextImage(bool& successful) {
    if (m_source == AcqSource::Camera) {
    } else {
        m_imageDisplayerWidget->getNextImage(successful);
    }

    return cv::Mat();
}

void AcquisitionInterface::reset() {
    m_imageDisplayerWidget->reset();
}
