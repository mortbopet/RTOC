#include <QApplication>
#include "../AcquisitionApp/src/acquisitionwidget.h"
#include "gui/mainwindow.h"
#include "lib/analyzer.h"

int main(int argc, char** argv) {
    Analyzer analyzer;

    //// TEMP PLACE FOR TEST ////
    // Create 20 random containers, each with 20 objects ERROR: THOSE VALUES MUST BE EQUAL?
    double randomNumber;
    int numberOfContainers = 100;
    int numberOfObjects = 20;
    for (int i = 0; i < numberOfContainers; i++) {
        analyzer.m_experiment.data.emplace_back(new DataContainer(0x3ffff));
        for (int j = 0; j < numberOfObjects; j++) {
            analyzer.m_experiment.data[i]->appendNew();
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Area, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::ConvexArea, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Circularity, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::Eccentricity, randomNumber);
            randomNumber = 1 + ( std::rand() % ( 100 - 1 + 1 ) );
            (*analyzer.m_experiment.data[i])[j]->setValue(data::GradientScore, randomNumber);
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
        }
    }
    analyzer.exportExperiment("/Users/eskidlbs/Desktop/ml_fagprojekt/file.csv");

    Q_INIT_RESOURCE(icons);
    QApplication a(argc, argv);

    MainWindow window(&analyzer);
    window.show();

    return a.exec();
}
