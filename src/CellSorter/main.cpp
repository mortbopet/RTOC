#include <QApplication>
#include "../AcquisitionApp/src/acquisitionwidget.h"
#include "gui/mainwindow.h"
#include "lib/analyzer.h"

int main(int argc, char** argv) {
    Analyzer analyzer;

    //// TEMP PLACE FOR TEST ////
    double randomNumber;
    int numberOfContainers = 100;
    int numberOfObjects = 20;
    for (int i = 0; i < numberOfContainers; i++) {
        analyzer.m_experiment.data.emplace_back(new DataContainer(0x3ffff));
        int counter = 0;
        for (int j = 0; j < numberOfObjects; j++) {
            analyzer.m_experiment.data[i]->appendNew();
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Area, randomNumber);
            (*analyzer.m_experiment.data[i])[j]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Centroid, cv::Point(5, 70));
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::ConvexArea, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Circularity, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Eccentricity, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Frame, 5);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::GradientScore, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Inlet, 80);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Outlet, 180);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Label, 21950);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Major_axis, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Minor_axis, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Solidity, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Symmetry, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Perimeter, randomNumber);
            (*analyzer.m_experiment.data[i])[j]->setValue(data::OutputValue, (i%2)*1.0);
            counter += 1;
        }
    }
    analyzer.exportAllFormat("/Users/eskidlbs/Desktop/ml_fagprojekt/testfile.txt");

    Q_INIT_RESOURCE(icons);
    QApplication a(argc, argv);

    MainWindow window(&analyzer);
    window.show();

    return a.exec();
}
