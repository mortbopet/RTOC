#include <QApplication>
#include "src/ironmanwidget.h"

int main(int argc, char** argv) {
    QApplication a(argc, argv);
    IronManWidget w;
    w.show();
    return a.exec();
}
