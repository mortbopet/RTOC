#ifndef GENERICCAMERAWIDGET_H
#define GENERICCAMERAWIDGET_H

#include <QWidget>

namespace Ui {
class GenericCameraWidget;
}

class GenericCameraWidget : public QWidget {
    Q_OBJECT

public:
    explicit GenericCameraWidget(QWidget* parent = 0);
    ~GenericCameraWidget();

private:
    Ui::GenericCameraWidget* ui;
};

#endif  // GENERICCAMERAWIDGET_H
