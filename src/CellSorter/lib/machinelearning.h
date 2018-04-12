#ifndef CELLSORTER_MACHINELEARNING_H
#define CELLSORTER_MACHINELEARNING_H

#include <opencv/ml.h>
#include <fstream>
#include <iostream>
#include <opencv/cv.hpp>
#include <string>

#include "experiment.h"
#include "parameter.h"
#include "process.h"
#include "datacontainer.h"

namespace {
#define PARAMETER_CONTAINER m_parameters

// Shorthand macros for creating process parameter member values
#define CREATE_ENUM_PARM(type, name, displayName) \
    EnumParameter<type> name = EnumParameter<type>(PARAMETER_CONTAINER, displayName)
#define CREATE_ENUM_PARM_DEFAULT(type, name, displayName, default) \
    EnumParameter<type> name = EnumParameter<type>(PARAMETER_CONTAINER, displayName, default)

#define CREATE_VALUE_PARM(type, name, displayName) \
    ValueParameter<type> name = ValueParameter<type>(PARAMETER_CONTAINER, displayName)
#define CREATE_VALUE_PARM_DEFAULT(type, name, displayName, default) \
    ValueParameter<type> name = ValueParameter<type>(PARAMETER_CONTAINER, displayName, default)
}  // namespace

//// Parent class

class MachineLearning {
public:
    MachineLearning();  // Constructor

    void add_to_trainset(DataContainer* m_datacontainer);

    void prepareModel();

    virtual void create_model() const = 0;

    virtual void train_model() const = 0;

    virtual void predict_model(DataContainer* m_datacontainer) const = 0;

protected:
    virtual cv::Ptr<cv::ml::LogisticRegression> crossvalidate(DataContainer* m_datacontainer) const = 0;

    std::vector<ParameterBase*> PARAMETER_CONTAINER;
    std::vector<std::vector<double>> m_datafromcontainers; // To store data extracted from datacontainer
    cv::Ptr<cv::ml::TrainData> m_datafromcontainers_CVtype;
    std::vector<DataContainer*> m_trainset;  // to store all data objects to perform moedel on
    cv::Ptr<cv::ml::LogisticRegression> model; // Temp solution for storing models
};

//// Child classes: Supervised learning

// Linear regression
class LogisticRegression : public MachineLearning {
public:
    LogisticRegression();  // Constructor

    void create_model() const override;

    void train_model() const override;

    void predict_model(DataContainer* m_datacontainer) const override;

    CREATE_VALUE_PARM(float, m_learningrate, "Learning rate of regression");
    CREATE_VALUE_PARM(int, m_iters, "Number of iterations for model");
    CREATE_VALUE_PARM(int, m_alpha, "Alpha-value for the model");
    CREATE_VALUE_PARM(int, m_kfold, "Number of seperations in crossvalidation");

private:
    cv::Ptr<cv::ml::LogisticRegression> crossvalidate(DataContainer* m_datacontainer) const override;
};

////// Child classes: Unsupervised learning
//
//// Clustering
//class Clustering : public MachineLearning {
//    Clustering();  // Constructor
//
//    void create_model() const override {}
//
//    void train_model(DataContainer* m_datacontainer) const override {}
//
//    void predict_model(DataContainer* m_datacontainer) const override {}
//
//private:
//    cv::Ptr<cv::ml::LogisticRegression> crossvalidate(DataContainer* m_datacontainer) const override {}
//};

#endif  // CELLSORTER_MACHINELEARNING_H