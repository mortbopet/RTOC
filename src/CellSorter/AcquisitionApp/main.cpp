#include <QApplication>
#include "src/acquisitionwidget.h"

int main(int argc, char** argv) {
    QApplication a(argc, argv);
    AcquisitionWidget w;
    w.show();
    return a.exec();
}
