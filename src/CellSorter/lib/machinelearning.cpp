#include "machinelearning.h"
#include <math.h>
#include <chrono>
#include <opencv/cv.hpp>
#include <random>
#include "rbc.h"

//// Parent class

MachineLearning::MachineLearning(void) {}

void MachineLearning::add_to_trainset(DataContainer* m_datacontainer) {
    m_trainset.push_back(m_datacontainer);
}

void MachineLearning::prepareModel() {
    // Sets length of datacontainers
    for (int i = 0; i < m_trainset.size(); i++) {
        // Gets data from vector and inserts into dataformat for modelling
        std::vector<double> tempvector = m_trainset[i]->getDataVector();
        m_datafromcontainers[i].insert(m_datafromcontainers[i].end(), tempvector.begin(), tempvector.end());
    }
}

void MachineLearning::create_model() const {}

void MachineLearning::train_model() const {}

void MachineLearning::predict_model(DataContainer* m_datacontainer) const {}

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

void LogisticRegression::create_model() const {
    // cv::Ptr<cv::ml::LogisticRegression> logistic_regression;
    model = cv::ml::LogisticRegression::create();
    model->setLearningRate(m_learningrate.getValue());
    model->setIterations(m_iters.getValue());
    model->setRegularization(cv::ml::LogisticRegression::REG_L2);
    model->setTrainMethod(cv::ml::LogisticRegression::BATCH);
    model->setMiniBatchSize(m_alpha.getValue());
}

void LogisticRegression::train_model() const {
    m_datafromcontainers_CVtype->create(m_datafromcontainers, 0, 'INSERT OUTPUT');
    model->train(m_datafromcontainers_CVtype);
}
//// The model below is with crossvalidation
//void LogisticRegression::train_model(DataContainer* m_datacontainer, int kFold) const {
//    // Two vectors are initialized to hold the temporary indicies the model should be trained and
//    // tested on
//    std::vector<int> tempTrain;
//    std::vector<int> tempTest;
//
//    // Initializes vectors to hold score of different models and vector to hold models as well as a data
//    // container in ml data format.
//    std::vector<int> score;
//    std::vector<cv::Ptr<cv::ml::LogisticRegression>> testModels;
//    const cv::Ptr<cv::ml::TrainData> m_datacontainer_CVTYPE;
//
//
//    // Randomly shuffles indicies of dataset to make it ready for partitioning for model creation.
//    // This is done to make the observations non-biased in terms of time of observing. The seed used
//    // Is random-generated based on system clock, but can be replaced by a custom seed.
//    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//    std::shuffle(m_trainset.begin(), m_trainset.end(), std::default_random_engine(seed));
//
//    // So far the model only crossvalidates regarding best traindata, but can be expanded to evalute
//    // fore more parameters Looping over kFold and computes score for each. Also if Kfold >>
//    // 1/10*number_of_observations, bad partitioning might occur.
//
//    int totalSize = m_trainset.size();
//    int partitionSize = int(floor(totalSize / kFold));
//    for (int i = 0; i < kFold; i++) {  // Iterates over all kFold
//        int tempIndexMin = i * partitionSize;
//        int tempIndexMax = (i + 1) * partitionSize;
//        for (int j = 0; j < totalSize; j++) {  // Iterates over all observations
//            if (j >= tempIndexMin && j < tempIndexMax) { // Puts index into test-set if in test-partition
//                tempTest.push_back(j);
//            } else { // Puts index into train-set if in train-partition
//                tempTrain.push_back(j);
//            }
//        }
//        for (auto j : tempTrain) {
//            m_datacontainer_CVTYPE->create(m_trainset[j]->getDataVector(),0,'INSERT OUTPUT');
//        }
//
//        testModels[i]->train(m_datacontainer_CVTYPE);
//        for (auto j : tempTest) {
//            m_datacontainer_CVTYPE->create(m_trainset[j]->getDataVector(),0,'INSERT OUTPUT');
//        }
//
//
//        tempTrain.clear();
//        tempTest.clear();
//    }
//
//    model = crossvalidate(m_datacontainer);
//}

void LogisticRegression::predict_model(DataContainer* m_datacontainer) const {
    model->predict(m_datacontainer->getDataVector());
}

//cv::Ptr<cv::ml::LogisticRegression>

    // LogisticRegression::crossvalidate(DataContainer* m_datacontainer) const {
    //    // Initialize vector of test models
    //    std::vector<cv::Ptr<cv::ml::LogisticRegression>> testModels;
    //
    //    int test_size = floor(m_datacontainer->getSize() *
    //                          (1 / m_kfold.getValue()));  // Gets size of each test block
    //    std::vector<int>
    //        flags;  // Contains flags for which one. Should be converted into boolean instead.
    //
    //    // Initialize pointers to temporary partitioning of data
    //    std::vector<DataObject*> tempTrain;
    //    std::vector<DataObject*> tempTrain_first;
    //    std::vector<DataObject*> tempTrain_last;
    //    std::vector<DataObject*> tempTest;
    //    std::vector<int> predicted;
    //    std::vector<float> score;
    //
    //    for (int i = 0; i < m_kfold.getValue(); i++) {  // Creates testModels for each kfold
    //        // Gets pointers to test and train data
    //        std::vector<DataObject*> tempTest(&m_datacontainer[i * test_size],
    //                                          &m_datacontainer[(i * test_size) + test_size]);
    //        std::vector<DataObject*> tempTrain_first(&m_datacontainer[0],
    //                                                 &m_datacontainer[i * test_size]);
    //        std::vector<DataObject*> tempTrain_last(&m_datacontainer[(i * test_size) + test_size],
    //                                                &m_datacontainer[m_datacontainer->getSize()]);
    //        tempTrain = tempTrain_first + tempTrain_last;
    //
    //        // 'Converts' train data into cv-format
    //        m_datacontainer_CVTYPE->create(tempTrain[/*rest*/], 0, tempTrain[/*GENDERINDEX*/]);
    //
    //        // Trains it on train-data
    //        testModels[i]->train(m_datacontainer_CVTYPE);
    //
    //        // Gets error-rate from test-data
    //        testModels[i]->predict(tempTest, predicted);
    //
    //        // Predicted output is now put into predicted-vector.
    //        // Those are now compared with the right values to compute an score
    //        // The score is number_of_correct_guesses / total_guesses
    //        for (int j = 0; j < test_size; j++) {
    //            if (predicted[j] == tempTest[/*GENDERINDEX*/]) {
    //                score[i] += 1;
    //            }
    //        }
    //        // Compute the score for model i.
    //        score[i] = score[i] / test_size;
    //    }
    //    // Returns model with highest score
    //    return testModels[std::max_element(score, score + m_kfold.getValue())];
    //}

    ////// Child classes: Unsupervised learning
    //
    //// Clustering
    // Clustering::Clustering() {}
    //
    //// void Clustering::add_to_set(DataObject bloodcell) const {}
    //
    // void Clustering::create_model() const {}
    //
    // void Clustering::train_model(DataContainer* m_datacontainer) const {}
    //
    // void Clustering::predict_model(DataContainer* m_datacontainer) const {}
    //
    // cv::Ptr<cv::ml::LogisticRegression>
    // Clustering::crossvalidate(DataContainer* m_datacontainer) const {}