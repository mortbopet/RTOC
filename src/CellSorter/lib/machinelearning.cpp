#include "machinelearning.h"
#include <math.h>
#include <opencv/cv.hpp>

//// Parent class

MachineLearning::MachineLearning(void) {}

void MachineLearning::create_model() const {}

void MachineLearning::train_model() const {}

void MachineLearning::predict_model() const {}

void MachineLearning::crossvalidate() const {}

//// Child classes: Supervised learning

// Linear regression
LinearRegression::LinearRegression() {}

void LinearRegression::create_model() const {
    model = cv::ml::LogisticRegression::create();
    model->setLearningRate(learningrate);
    model->setIterations(iters);
    model->setRegularization(cv::ml::LogisticRegression::REG_L2);
    model->setTrainMethod(cv::ml::LogisticRegression::BATCH);
    model->setMiniBatchSize(1);
}

void LinearRegression::train_model() const {}

void LinearRegression::predict_model() const {}

void LinearRegression::crossvalidate() const {}

//// Child classes: Unsupervised learning

// Clustering
Clustering::Clustering() {}

void Clustering::create_model() const {}

void Clustering::train_model() const {}

void Clustering::predict_model() const {}

void Clustering::crossvalidate() const {}