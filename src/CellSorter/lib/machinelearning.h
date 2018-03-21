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
    // Make all those variables dynamic
    int learningrate = 0.001;
    int iters = 1000;
    int alpha = 1;
    int kfold = 5;
    cv::Ptr<cv::ml::LogisticRegression> model;

    MachineLearning();  // Constructor

    virtual void create_model() const = 0;

    virtual void train_model() const = 0;

    virtual void predict_model() const = 0;

    virtual void crossvalidate() const = 0;
};

//// Child classes: Supervised learning

// Linear regression
class LinearRegression : public MachineLearning {  // TODO: Create child classes
public:
    LinearRegression();  // Constructor

    void create_model() {}

    void train_model() {}

    void predict_model() {}

    void crossvalidate() {}

private:
};

//// Child classes: Unsupervised learning

// Clustering
class Clustering : public MachineLearning {
    Clustering();  // Constructor

    void create_model() {  // Combines functions below to create a regression model
    }

    void train_model() {}

    void predict_model() {}

    void crossvalidate() {  // Performs crossvalidation of different logistic models
    }
};

#endif  // CELLSORTER_MACHINELEARNING_H