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

//// Machine learning class
// Input into class should be a pointer to a data container with desired features for each
// bloodcell. This data should be in the dimensions of :number_of_bloodcells x features
//
// number_of_bloodcells:  Bloodcells in sample
// features:              Data of desired features. Each frame with each new attribute is a feature
//
// Also, as an input its required to specify whether the sample is from an addult, a fetal or
// a mix.

//// Parent class

class MachineLearning {
public:
    MachineLearning();  // Constructor

    //virtual void add_to_set(DataObject bloodcell) const = 0;  // Used for adding a RBC to a set

    virtual void create_model() const = 0;

    virtual void train_model(DataContainer* m_datacontainer) const = 0;

    virtual void predict_model(DataObject* bloodcell, DataContainer* m_datacontainer) const = 0;

protected:
    virtual cv::Ptr<cv::ml::LogisticRegression> crossvalidate(DataContainer* m_datacontainer) const = 0;

    std::vector<ParameterBase*> PARAMETER_CONTAINER;
    DataContainer m_datacontainer;
    const cv::Ptr<cv::ml::TrainData> m_datacontainer_CVTYPE;
    cv::Ptr<cv::ml::LogisticRegression> model; // Temp solution for storing models
};

//// Child classes: Supervised learning

// Linear regression
class LogisticRegression : public MachineLearning {
public:
    LogisticRegression();  // Constructor

    //void add_to_set(DataObject bloodcell) const override {}

    void create_model() const override {}

    void train_model(DataContainer* m_datacontainer) const override {}

    void predict_model(DataObject* bloodcell, DataContainer* m_datacontainer) const override {}

    CREATE_VALUE_PARM(float, m_learningrate, "Learning rate of regression");
    CREATE_VALUE_PARM(int, m_iters, "Number of iterations for model");
    CREATE_VALUE_PARM(int, m_alpha, "Alpha-value for the model");
    CREATE_VALUE_PARM(int, m_kfold, "Number of seperations in crossvalidation");

private:
    cv::Ptr<cv::ml::LogisticRegression> crossvalidate(DataContainer* m_datacontainer) const override {}
};

//// Child classes: Unsupervised learning

// Clustering
class Clustering : public MachineLearning {
    Clustering();  // Constructor

    //void add_to_set(DataObject bloodcell) const override {}

    void create_model() const override {}

    void train_model(DataContainer* m_datacontainer) const override {}

    void predict_model(DataObject* bloodcell, DataContainer* m_datacontainer) const override {}

private:
    cv::Ptr<cv::ml::LogisticRegression> crossvalidate(DataContainer* m_datacontainer) const override {}
};

#endif  // CELLSORTER_MACHINELEARNING_H