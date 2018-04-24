#ifndef GENERICCAMERAWIDGET_H
#define GENERICCAMERAWIDGET_H

#include <QWidget>

#include <opencv/cv.hpp>

#include "../lib/analyzer.h"
#include "../lib/framefinder.h"

namespace Ui {
class GenericCameraWidget;
}

class GenericCameraWidget : public QWidget {
    Q_OBJECT

public:
    explicit GenericCameraWidget(QWidget* parent = 0);
    ~GenericCameraWidget();

    cv::Mat* getNextImage(bool& successful);

private:
    Ui::GenericCameraWidget* ui;

    cv::VideoCapture m_vidcap;
    bool m_open = false;

    cv::Mat m_image, m_capture;
};

#endif  // GENERICCAMERAWIDGET_H
