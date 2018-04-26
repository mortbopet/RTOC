#include <QApplication>
#include "gui/mainwindow.h"
#include "lib/analyzer.h"

int main(int argc, char** argv) {
    Analyzer analyzer;

    Q_INIT_RESOURCE(icons);
    QApplication a(argc, argv);

    MainWindow window(&analyzer);
    window.show();

    return a.exec();
}
