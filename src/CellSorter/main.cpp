#include <QApplication>
#include "gui/mainwindow.h"
#include "lib/analyzer.h"

#include <chrono>

#include <QDebug>

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(icons);
    QApplication a(argc, argv);

    MainWindow window;
    window.show();

    return a.exec();
}
