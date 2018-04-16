#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../AcquisitionApp/src/acquisitionwidget.h"
#include "acquisitioninterface.h"
#include "configurator.h"
#include "imagedisplayerwidget.h"
#include "processinterface.h"

namespace Ui {
class MainWindow;
}
class Analyzer;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(Analyzer* analyzer, QWidget* parent = 0);
    ~MainWindow();

private slots:
    void cameraSelectedWithoutAcqBuilt();
    void acqSelectionChanged(int index);
    void ffStateChanged(int state);

private:
    Ui::MainWindow* ui;

    ProcessInterface* m_processInterface;
    Configurator* m_configurator;
    Analyzer* m_analyzer;
    AcquisitionWidget* m_acquisitionWdiget;
    ImageDisplayerWidget* m_imageDisplayerWidget;
    AcquisitionInterface* m_acqInterface;

    void setupAcqCombobox();
};

#endif  // MAINWINDOW_H
