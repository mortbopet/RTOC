#ifndef ACQUISITIONINTERFACE_H
#define ACQUISITIONINTERFACE_H

/**
 * @brief The AcquisitionInterface class
 * Used as an interface to access an cv::mat& to the next image in line in a blocking manner.
 * Access can be to either the ImageDisplayWidget's list of images, or a query to the camera to
 * transfer an image to local memory
 */

#include <opencv/cv.h>
#include <QMetaType>
#include "imagedisplayerwidget.h"
enum class AcqSource { Camera, Folder, Webcam };
Q_DECLARE_METATYPE(AcqSource)

class AcquisitionInterface {
public:
    AcquisitionInterface(ImageDisplayerWidget* imageDisplayer);

    void reset();

    void setAcqSourceState(AcqSource source) { m_source = source; }

    cv::Mat& getNextImage(bool& successful);

private:
    AcqSource m_source;

    ImageDisplayerWidget* m_imageDisplayerWidget;
};

#endif  // ACQUISITIONINTERFACE_H
