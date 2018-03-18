#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#ifdef BUILD_ACQ
#include "../acquisitor_src/acquisitor.h"
#endif

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

private:
    Ui::MainWindow* m_ui;
    Logger m_logger;
};

#endif  // MAINWINDOW_H
