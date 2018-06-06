#include "machinelearning.h"

#include <fstream>
#include <iostream>
#include <string>

std::unique_ptr<Machinelearning> identifyModel(const std::string& path) {
    std::string line;
    std::ifstream file(path);
    while (std::getline(file, line, ',')) {
        if (line == "LR") {
            Machinelearning *model = new LogisticRegression();
            auto ptr = std::unique_ptr<Machinelearning>{model};
            return ptr;
        }
    }
}

Machinelearning::Machinelearning() {}

void Machinelearning::loadModel(const std::string& path) {}

double Machinelearning::predictObject(DataContainer& dataContainer, double decisionBoundary) {}

int Machinelearning::get_XBoundary() const {
    return _XBoundary;
}

void Machinelearning::set_XBoundary(int m_XBoundary) {
    Machinelearning::_XBoundary = m_XBoundary;
}

int Machinelearning::findClosestXpos(const int& pos, DataContainer& dataContainer) {
    for (int i = 0; i < _XBoundary; i++) {
        int index = 0;
        for (const auto& object : dataContainer) {
            auto centroid = object->getValue<cv::Point>(data::Centroid);
            if ((centroid.x > (pos - i)) && (centroid.x < (pos + i))) {
                return index;
            }
            index++;
        }
    }
}

LogisticRegression::LogisticRegression() {}

void LogisticRegression::loadModel(const std::string& path) {
    std::string line;
    std::ifstream file(path);
    int counter = 0;
    int flag = 0;
    while (std::getline(file, line, ',')) {
        if (flag == 0) {
            if (counter == 1) {
                intercept = std::stoi(line);
                counter = -1;
                flag = 1;
            }
        } else if (counter % 3 == 0) {
            xpos.push_back(std::stoi(line));
        } else if (counter % 3 == 1) {
            for (const auto& item : data::guiMap) {
                if (item.second == line) {
                    attributes.push_back(std::get<1>(item.first));
                }
            }
        } else {
            coefficients.push_back(stod(line));
        }
        counter += 1;
    }
}

double LogisticRegression::predictObject(DataContainer& dataContainer, double decisionBoundary) {
    // customParameter defines the decision boundary between the binary classification.
    // By default this should be set to 0.5.

    double possibility = 0;
    for (int i = 0; i < xpos.size(); i++) {
        // First, find the object with the given x-position
        int index = findClosestXpos(xpos[i], dataContainer);

        // Next, extract the right attribute and multiply its coefficient onto the model
        // Make arguement change with attribute-type
        auto attributeValue = dataContainer[index]->getValue<double>(attributes[i]);
        possibility += (attributeValue * coefficients[i]);
    }
    // Add intercept
    possibility = exp(possibility + intercept);

    // Compute possibility
    possibility = possibility / (1 + possibility);

    // Checks if possibility is above the chosen threshold
    if (possibility >= decisionBoundary) {
        return 1;
    } else {
        return 0;
    }
}


ArtificialNeuralNetwork::ArtificialNeuralNetwork() {
}

DecisionTree::DecisionTree() {
}

NaiveBayes::NaiveBayes() {
}