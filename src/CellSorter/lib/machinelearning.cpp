#include "machinelearning.h"
#include <math.h>
#include <opencv/cv.hpp>
#include "rbc.h"

//// Parent class

MachineLearning::MachineLearning(void) {}

void MachineLearning::add_to_set(RBC bloodcell) const {}

void MachineLearning::create_model() const {}

void MachineLearning::train_model() const {}

void MachineLearning::predict_model(RBC bloodcell) const {}

void MachineLearning::crossvalidate() const {}

//// Child classes: Supervised learning

// Linear regression
LogisticRegression::LogisticRegression() {
    m_learningrate.setRange(0, 1);
    m_iters.setRange(0, 1000000);
    m_alpha.setRange(0, 10);
    m_kfold.setRange(0, 10);  // So far, cap kfold at 10
}

void LogisticRegression::add_to_set(RBC bloodcell) const {
    // m_RBC_DATA.push_back(bloodcell); //WHY DOES THIS NOT WORK?
}

void LogisticRegression::create_model() const {
    // cv::Ptr<cv::ml::LogisticRegression> logistic_regression;
    model = cv::ml::LogisticRegression::create();
    model->setLearningRate(m_learningrate.getValue());
    model->setIterations(m_iters.getValue());
    model->setRegularization(cv::ml::LogisticRegression::REG_L2);
    model->setTrainMethod(cv::ml::LogisticRegression::BATCH);
    model->setMiniBatchSize(m_alpha.getValue());

    // Function-call below takes RBC_data and puts into correct type being cv::ml::TrainData. This
    // type should maybe already be included in RBC.h?
    //
    // First argument should be matrix of
    // Observations X Attributes Last argument should be a vector of the types of each cell

    // m_RBC_data->create(m_RBC, 0,m_RBC->type);
}

void LogisticRegression::train_model() const {
    model->train(m_RBC_data);

    //model->train(m_RBC_data);
    // perform crossvalidation

}

void LogisticRegression::predict_model(RBC bloodcell) const {
    // Input should be a floating point matrix (vector).
    // Each attribute needs to be converted.
    //model->predict(bloodcell);
}

void LogisticRegression::crossvalidate() const {
    cv::Ptr<cv::ml::StatModel> testModels; // Container for models to test
    int test_size = floor(m_RBC.size()*(1/m_kfold.getValue())); // Gets size of each test block
    std::vector<bool> flags; // Contains flags for which one
    int offset; // Pointer for setting offset for data seperation

    for (int i = 0; i < m_kfold.getValue(); i ++) { // Creates testModels for each kfold
        offset
    }
}

//// Child classes: Unsupervised learning

// Clustering
Clustering::Clustering() {}

void Clustering::add_to_set(RBC bloodcell) const {}

void Clustering::create_model() const {}

void Clustering::train_model() const {}

void Clustering::predict_model(RBC bloodcell) const {}

void Clustering::crossvalidate() const {}