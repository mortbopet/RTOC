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
#include "genericcamerawidget.h"
#include "imagedisplayerwidget.h"

#ifdef BUILD_ACQ
#include "AcquisitionApp/acquisitor_src/acquisitor.h"
#endif

enum class AcqSource { IronManCamera, Folder, Webcam };
Q_DECLARE_METATYPE(AcqSource)

class AcquisitionInterface : public QObject {
    Q_OBJECT
public:
    AcquisitionInterface(ImageDisplayerWidget* imageDisplayer);

    void reset();
    void setAcqSourceState(AcqSource source) { m_source = source; }
    cv::Mat& getNextImage(bool& successful);

public:
    void setFFState(int state) { m_ffEnabled = state; }
    void setFFThresh(int value) { m_threshold = value; }

private:
    bool m_ffEnabled = false;

    int m_threshold = 0;

    AcqSource m_source = AcqSource::Folder;
    ImageDisplayerWidget* m_imageDisplayerWidget;
    GenericCameraWidget* m_genericCameraWidget;

#ifdef BUILD_ACQ
public:
    void setAcquisitor(Acquisitor* acq) { m_acquisitor = acq; }
    void setIronManDimensions(QPair<int, int> dim) { m_ironManDimensions = dim; }

private:
    Acquisitor* m_acquisitor;
    QPair<int, int> m_ironManDimensions;

    cv::Mat m_ironmanImage;
#endif

    cv::Mat m_lastImage;
};

#endif  // ACQUISITIONINTERFACE_H
