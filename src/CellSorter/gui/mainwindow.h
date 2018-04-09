#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../AcquisitionApp/src/acquisitionwidget.h"
#include "configurator.h"
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

private:
    Ui::MainWindow* ui;

    ProcessInterface* m_processInterface;
    Configurator* m_configurator;
    Analyzer* m_analyzer;
    AcquisitionWidget* m_acquisitionWdiget;
};

#endif  // MAINWINDOW_H
