#include <QApplication>
#include "gui/mainwindow.h"
#include "lib/analyzer.h"

#include <chrono>

#include <QDebug>
#include <QSplashScreen>

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(icons);
    QApplication a(argc, argv);

    QString projectFileName = QString();
    if (argc == 2) {
        projectFileName = QString::fromLatin1(argv[1]);
    }

    // Splash or nah
    QPixmap pixmap(":/resources/splashscreen.png");
    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents();
    // -------------

    MainWindow window(projectFileName);
    window.show();

    return a.exec();
}
