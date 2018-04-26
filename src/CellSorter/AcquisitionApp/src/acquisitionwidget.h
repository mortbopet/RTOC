#ifndef ACQUISITIONWIDGET_H
#define ACQUISITIONWIDGET_H

#ifdef BUILD_ACQ
#include "../acquisitor_src/acquisitor.h"
#endif

#include "imagedisplayer.h"
#include "logger.h"

#include <QTimer>
#include <QWidget>

namespace Ui {
class AcquisitionWidget;
}

class AcquisitionWidget : public QWidget {
    Q_OBJECT

public:
    explicit AcquisitionWidget(QWidget* parent = 0);
    ~AcquisitionWidget();

private slots:
    void on_actionExit_triggered();
    void on_clearLog_clicked();

    void on_scale_currentIndexChanged(const QString& arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_txtPathButton_clicked();
    void on_xmlPathButton_clicked();

private:
    ImageDisplayer m_imageDisplayer;

    Ui::AcquisitionWidget* m_ui;
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

#endif  // ACQUISITIONWIDGET_H
