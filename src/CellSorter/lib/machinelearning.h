#ifndef CELLSORTER_MACHINELEARNING_H
#define CELLSORTER_MACHINELEARNING_H

#include <vector>
#include "datacontainer.h"

class Machinelearning {
public:
    Machinelearning();
    void loadModel(const std::string& path);
    double predictObject(DataContainer &dataContainer);

    int get_XBoundary() const;
    void set_XBoundary(int m_XBoundary);

private:
    int findClosestXpos(const int& pos, DataContainer& dataContainer);

    std::vector<int> xpos;
    std::vector<data::DataFlags> attributes;
    std::vector<double> coefficients;

    int _XBoundary = 250;

};

#endif //CELLSORTER_MACHINELEARNING_H