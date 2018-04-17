#include <QApplication>
#include <cstdio>
#include <fstream>
#include <opencv/cv.hpp>
#include "../AcquisitionApp/src/acquisitionwidget.h"
#include "gui/mainwindow.h"
#include "lib/analyzer.h"
#include "lib/timer.h"

int main(int argc, char** argv) {
    Timer t;
    Analyzer analyzer;

    Q_INIT_RESOURCE(icons);
    QApplication a(argc, argv);

    MainWindow window(&analyzer);
    window.show();

    return a.exec();
}
