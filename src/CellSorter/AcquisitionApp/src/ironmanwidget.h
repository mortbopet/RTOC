#ifndef IRONMANWIDGET_H
#define IRONMANWIDGET_H

#ifdef BUILD_ACQ
#include "../acquisitor_src/acquisitor.h"
#endif

#include "cameradisplayerwidget.h"
#include "logger.h"

#include <QTimer>
#include <QWidget>

namespace Ui {
class IronManWidget;
}

class IronManWidget : public QWidget {
    Q_OBJECT

public:
    explicit IronManWidget(QWidget* parent = 0);
    ~IronManWidget();

private slots:
    void on_actionExit_triggered();
    void on_clearLog_clicked();

    void on_scale_currentIndexChanged(const QString& arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_txtPathButton_clicked();
    void on_xmlPathButton_clicked();

private:
    CameraDisplayerWidget m_imageDisplayer;

    Ui::IronManWidget* m_ui;
    Logger* m_logger;

#ifdef BUILD_ACQ
private slots:
    void initializeFramegrabber();
    void acqStateChanged(AcqState state);
    void setButtonStates(AcqState state);

private:
// The acquisitor object is initialized to reside as a permanent object in a separate thread.
// All communication is facilitated through signals/slots through QMetaObject::InvokeMethod
#endif
};

#endif  // IRONMANWIDGET_H
