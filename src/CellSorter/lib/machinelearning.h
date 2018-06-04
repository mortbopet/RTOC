#ifndef CELLSORTER_MACHINELEARNING_H
#define CELLSORTER_MACHINELEARNING_H

#include <vector>
#include "datacontainer.h"

class Machinelearning {
public:
    Machinelearning();
    void loadModel (const std::string& path);
    double predictContainer (DataContainer& dataContainer);

private:
    int findClosestXpos(const int& pos, DataContainer& dataContainer);

    std::vector<int> xpos;
    std::vector<data::DataFlags> attributes;
    std::vector<double> coefficients;
};

#endif //CELLSORTER_MACHINELEARNING_H