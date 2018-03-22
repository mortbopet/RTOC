#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#ifdef BUILD_ACQ
#include "../acquisitor_src/acquisitor.h"
#endif

#include "imagedisplayer.h"
#include "logger.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_clearLog_clicked();

    void on_scale_currentIndexChanged(const QString& arg1);

    void on_spinBox_valueChanged(int arg1);

    void on_txtPathButton_clicked();
    void on_xmlPathButton_clicked();

private:
    ImageDisplayer m_imageDisplayer;

    Ui::MainWindow* m_ui;
    Logger m_logger;

#ifdef BUILD_ACQ
private slots:
    void initializeFramegrabber();
    void acqStateChanged(AcqState state);
    void setButtonStates(AcqState state);

private:
    // The acquisitor object is initialized to reside as a permanent object in a separate thread.
    // All communication is facilitated through signals/slots through QMetaObject::InvokeMethod
    Acquisitor* m_acquisitor;

    QTimer m_acqWaitTimer;  // timer for printing dots in the log, when awaiting Acquisitor answer
#endif
};

#endif  // MAINWINDOW_H
