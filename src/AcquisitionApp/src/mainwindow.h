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
    void initializeFramegrabber();

    void on_actionExit_triggered();

    void on_filePathButton_clicked();

    void setInitializerButtonState(bool state);

    void acquisitionStateChanged(bool state);
    void setButtonStates();

private:
    enum class AcqState { idle, initializing, initialized, Acquiring };
    AcqState m_acqState = AcqState::idle;
    ImageDisplayer m_imageDisplayer;

    Ui::MainWindow* m_ui;
    Logger m_logger;

#ifdef BUILD_ACQ
    // The acquisitor object is initialized to reside as a permanent object in a separate thread.
    // All communication is facilitated through signals/slots through QMetaObject::InvokeMethod
    Acquisitor* m_acquisitor;

    QTimer m_acqWaitTimer;  // timer for printing dots in the log, when awaiting Acquisitor answer
#endif
};

#endif  // MAINWINDOW_H
