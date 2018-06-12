#ifndef RTOC_MACHINELEARNING_H
#define RTOC_MACHINELEARNING_H

#include <memory>
#include <vector>
#include "datacontainer.h"
//namespace inp {
//static std::map<std::string, int> guiMap {
//        {"Closest data point", 0},
//        {"Average of closest data points", 1},
//        {"Inverse distance weighting", 2}
//};
//}

class Machinelearning {
public:
    Machinelearning();
    virtual void loadModel(const std::string& path);

    virtual void predictObject(DataContainer& dataContainer, std::string interpolation_style, double decisionBoundary=0.5);

    int get_XBoundary() const;
    void set_XBoundary(int m_XBoundary);

    void outputToFile(const std::string& path);

    struct outputData {
        double output;
        double probabilityOut;
        int label;

        outputData(double output, double probabilityOut, int label) :
                output(output), probabilityOut(probabilityOut), label(label) {}
    };

    const std::vector<outputData>& getResults() {return results;};

protected:
    std::vector<int> xpos;
    std::vector<data::DataFlags> attributes;
    std::vector<outputData> results;
    int findClosestXpos(const int& pos, DataContainer& dataContainer);
    double interpolation_average(const int& pos, DataContainer& dataContainer, data::DataFlags attribute);
    double interpolation_inverseDistance(const int &pos, DataContainer &dataContainer, data::DataFlags &attribute);

private:
    int _XBoundary = 250;
};

class LogisticRegression : public Machinelearning {
public:
    LogisticRegression();
    void loadModel(const std::string& path) override;

    void predictObject(DataContainer& dataContainer, std::string interpolation_style, double decisionBoundary) override;

private:
    std::vector<double> coefficients;
    int intercept;
};

class ArtificialNeuralNetwork : public Machinelearning {
public:
    ArtificialNeuralNetwork();

private:
    std::vector<int> weights;
};

class DecisionTree : public Machinelearning {
public:
    DecisionTree();

private:
};

class NaiveBayes : public Machinelearning {
public:
    NaiveBayes();

private:
};

std::unique_ptr<Machinelearning> identifyModel(const std::string& path);

#endif  // RTOC_MACHINELEARNING_H
