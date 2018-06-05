#ifndef CELLSORTER_MACHINELEARNING_H
#define CELLSORTER_MACHINELEARNING_H

#include <vector>
#include "datacontainer.h"

class Machinelearning {
public:
    Machinelearning();
    virtual void loadModel(const std::string& path);
    virtual double predictObject(DataContainer &dataContainer);

    int get_XBoundary() const;
    void set_XBoundary(int m_XBoundary);

private:
    int findClosestXpos(const int& pos, DataContainer& dataContainer);

    int _XBoundary = 250;

};

class LogisticRegression : public Machinelearning {
public:
    LogisticRegression();
    void loadModel(const std::string& path) override;
    double predictObject(DataContainer &dataContainer) override;

private:
    std::vector<int> xpos;
    std::vector<data::DataFlags> attributes;
    std::vector<double> coefficients;

};

#endif //CELLSORTER_MACHINELEARNING_H