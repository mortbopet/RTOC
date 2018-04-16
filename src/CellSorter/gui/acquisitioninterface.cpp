#include "acquisitioninterface.h"

AcquisitionInterface::AcquisitionInterface(ImageDisplayerWidget* displayer)
    : m_imageDisplayerWidget(displayer) {}

cv::Mat& AcquisitionInterface::getNextImage(bool& successful) {
    cv::Mat* newImage;

    // Get new image from current source
    while (true) {
        switch (m_source) {
            case AcqSource::Camera: {
                break;
            }
            case AcqSource::Webcam: {
                break;
            }
            case AcqSource::Folder: {
                newImage = m_imageDisplayerWidget->getNextImage(successful);
            }
        }

        if (!successful) {
            // There was an error in acquisiton, return last image and set successful flag to false
            return m_lastImage;
        }

        if (m_ffEnabled) {
            if (hasChanged(m_lastImage, *newImage, m_threshold)) {
                // A sufficient change between current and last image has been detected, return the
                // newly acquired image
                m_lastImage = *newImage;
                return m_lastImage;
            } else {
                // Continue querying acquisition source until hasChanged() returns true
            }
        } else {
            // Frame finder is not enabled - just return the last image
            m_lastImage = *newImage;
            return m_lastImage;
        }
    }
}

void AcquisitionInterface::reset() {
    m_imageDisplayerWidget->reset();
}
