#include "acquisitioninterface.h"

AcquisitionInterface::AcquisitionInterface(ImageDisplayerWidget* displayer)
    : m_imageDisplayerWidget(displayer) {}

cv::Mat& AcquisitionInterface::getNextImage(bool& successful) {
    cv::Mat* newImage = nullptr;

    successful =
        false;  // initially, we break if the selected acquisition source cannot retrieve an image

    // Get new image from current source
    while (true) {
        switch (m_source) {
            case AcqSource::IronManCamera: {
#ifdef BUILD_IRONMAN
                auto imageData = m_acquisitor->requestImageDataBlocking(successful);
                m_ironmanImage = cv::Mat(m_ironManDimensions.second, m_ironManDimensions.first,
                                         CV_8UC1, imageData->data());
                newImage = &m_ironmanImage;
#endif
                break;
            }
            case AcqSource::Webcam: {
                newImage = m_genericCameraWidget->getNextImage(successful);
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
            if (m_lastImage.empty()) {
                // last image needs to be initialized
                m_lastImage = *newImage;
            }
            if (framefinder::hasChanged(m_lastImage, *newImage, m_threshold)) {
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
    // Reset all acquisition devices such that they are ready to be queried for images from their
    // initial state/position
    m_imageDisplayerWidget->reset();
}
