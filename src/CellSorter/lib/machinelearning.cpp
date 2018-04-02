#include "machinelearning.h"
#include <math.h>
#include <opencv/cv.hpp>
#include "rbc.h"

//// Parent class

MachineLearning::MachineLearning(void) {}

// void MachineLearning::add_to_set(DataObject bloodcell) const {}

void MachineLearning::create_model() const {}

void MachineLearning::train_model(DataContainer* m_datacontainer) const {}

void MachineLearning::predict_model(DataObject* bloodcell) const {}

cv::Ptr<cv::ml::LogisticRegression>
MachineLearning::crossvalidate(DataContainer* m_datacontainer) const {}

//// Child classes: Supervised learning

// Linear regression
LogisticRegression::LogisticRegression() {
    m_learningrate.setRange(0, 1);
    m_iters.setRange(0, 1000000);
    m_alpha.setRange(0, 10);
    m_kfold.setRange(0, 10);  // So far, cap kfold at 10
}

// void LogisticRegression::add_to_set(DataObject bloodcell) const {
//    m_datacontainer.appendNew(); //WHY DOES THIS NOT WORK?
//}

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

    // m_datacontainer_CVTYPE->create(m_datacontainer, 0,m_datacontainer[GENDERINDEX]);
}

void LogisticRegression::train_model(DataContainer* m_datacontainer) const {
    model = crossvalidate(m_datacontainer);

    // model->train(m_RBC_data);
    // perform crossvalidation
}

void LogisticRegression::predict_model(DataObject* bloodcell) const {
    // Input should be a floating point matrix (vector).
    // Each attribute needs to be converted.
    // model->predict(bloodcell);
}

cv::Ptr<cv::ml::LogisticRegression>

LogisticRegression::crossvalidate(DataContainer* m_datacontainer) const {
    std::vector<cv::Ptr<cv::ml::LogisticRegression>> testModels;  // Container for models to test
    int test_size = floor(m_datacontainer->getSize() *
                          (1 / m_kfold.getValue()));  // Gets size of each test block
    std::vector<int>
        flags;  // Contains flags for which one. Should be converted into boolean instead.

    std::vector<DataObject*> tempTrain;
    std::vector<int> predicted;
    std::vector<float> score;

    for (int i = 0; i < m_kfold.getValue(); i++) {  // Creates testModels for each kfold
        // Gets pointers to test and train data
        std::vector<DataObject*> tempTest(&m_datacontainer->m_data[i * test_size],
                                          &m_datacontainer[(i * test_size) + test_size]);
        std::vector<DataObject*> tempTrain_first(&m_datacontainer[0],
                                                 &m_datacontainer[i * test_size]);
        std::vector<DataObject*> tempTrain_last(&m_datacontainer[(i * test_size) + test_size],
                                                &m_datacontainer[&m_datacontainer.getSize()]);
        tempTrain = tempTrain_first + tempTrain_last;

        // 'Converts' train data into cv-format
        m_datacontainer_CVTYPE->create(tempTrain, 0, tempTrain[/*GENDERINDEX*/]);

        // Trains it on train-data
        testModels[i]->train(m_datacontainer_CVTYPE);

        // Gets error-rate from test-data
        testModels[i]->predict(tempTest, predicted);

        // Predicted output is now put into predicted-vector.
        // Those are now compared with the right values to compute an score
        // The score is number_of_correct_guesses / total_guesses
        for (int j = 0; j < test_size; j++) {
            if (predicted[j] == tempTest[/*GENDERINDEX*/]) {
                score[i] += 1;
            }
        }
        // Compute the score for model i.
        score[i] = score[i] / test_size;
    }
    // Returns model with highest score
    return testModels[std::max_element(score, score + m_kfold.getValue())];
}

//// Child classes: Unsupervised learning

// Clustering
Clustering::Clustering() {}

// void Clustering::add_to_set(DataObject bloodcell) const {}

void Clustering::create_model() const {}

void Clustering::train_model(DataContainer* m_datacontainer) const {}

void Clustering::predict_model(DataObject* bloodcell) const {}

cv::Ptr<cv::ml::LogisticRegression>
Clustering::crossvalidate(DataContainer* m_datacontainer) const {}