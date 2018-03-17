#include <QApplication>
#include "src/mainwindow.h"

#ifdef BUILD_ACQ
#include "src/acquisitor.h"
#endif

int main(int argc, char** argv) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
