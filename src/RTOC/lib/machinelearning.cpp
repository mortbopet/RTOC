#include "machinelearning.h"

#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

std::unique_ptr<Machinelearning> identifyModel(const std::string& path) {
    std::string line;
    std::ifstream file(path);
    while (std::getline(file, line, ',')) {
        if (line == "LR") {
            return std::make_unique<LogisticRegression>();
        }
    }
    return nullptr;
}

Machinelearning::Machinelearning() {}

void Machinelearning::loadModel(const std::string& path) {}

void Machinelearning::predictObject(DataContainer& dataContainer, std::string interpolation_style, double decisionBoundary) {}

int Machinelearning::get_XBoundary() const {
    return _XBoundary;
}

void Machinelearning::set_XBoundary(int m_XBoundary) {
    Machinelearning::_XBoundary = m_XBoundary;
}

void Machinelearning::outputToFile(const std::string& path) {
    std::ofstream out(path);
    for (const auto& item : results) {
        out << item.label << "," << item.probabilityOut << "," << item.output << ",\n";
    }
    out.close();
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
    return -1;
}

double Machinelearning::interpolation_inverseDistance(const int &pos, DataContainer &dataContainer,
                                                      data::DataFlags &attribute) { // Shepards method
    double numerator = 0;
    double denominator = 0;
    for (const auto &item : dataContainer) {
        numerator += (item->getValue<double>(attribute) * 1/pow((0.01 + abs(pos - item->getValue<cv::Point>(data::Centroid).x)),2));
        denominator += 1/pow((0.01 + abs(pos - item->getValue<cv::Point>(data::Centroid).x)),2);
    }
    return (numerator / denominator); // Returns interpolated value
}

double Machinelearning::interpolation_average(const int &pos, DataContainer &dataContainer, data::DataFlags attribute) {
    int lowIndex = 0;
    int lowDistance = 0;
    int highIndex = 0;
    int highDistance = 0;
    // First we look for a centroid below the given x-position
    for (int i = 0; i < _XBoundary; i++) {
        int index = 0;
        for (const auto &object : dataContainer) {
            auto centroid = object->getValue<cv::Point>(data::Centroid);
            if ((centroid.x > (pos - i)) && (centroid.x < (pos))) {
                lowIndex = index;
                lowDistance = i;
            }
            index++;
        }
    }
    for (int j = 0; j < _XBoundary; j++) {
        int index = 0;
        for (const auto& object : dataContainer) {
            auto centroid = object->getValue<cv::Point>(data::Centroid);
            if ((centroid.x > (pos)) && (centroid.x < (pos + j))) {
                highIndex = index;
                highDistance = j;
            }
            index++;
        }
    }
    int sumDistance = lowDistance + highDistance;
    return ( (((sumDistance-lowDistance)/(sumDistance))*dataContainer[lowIndex]->getValue<double>(attribute)) +
             (((sumDistance-highDistance)/(sumDistance))*dataContainer[highIndex]->getValue<double>(attribute)) );
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

void LogisticRegression::predictObject(DataContainer& dataContainer, std::string interpolation_style, double decisionBoundary) {
    // customParameter defines the decision boundary between the binary classification.
    // By default this should be set to 0.5.

    double possibility = 0;
    for (int i = 0; i < xpos.size(); i++) {
        if (interpolation_style == "Closest data point") {
            // First, find the object with the given x-position
            int index = findClosestXpos(xpos[i], dataContainer);

            // Next, extract the right attribute and multiply its coefficient onto the model
            // Make arguement change with attribute-type
            auto attributeValue = dataContainer[index]->getValue<double>(attributes[i]);
            possibility += (attributeValue * coefficients[i]);

        } else if (interpolation_style == "Average of closest data points") {
            auto attributeValue = interpolation_average(xpos[i], dataContainer, attributes[i]);
            possibility += (attributeValue * coefficients[i]);

        } else if (interpolation_style == "Inverse distance weighting") {
            auto attributeValue = interpolation_inverseDistance(xpos[i], dataContainer, attributes[i]);
            possibility += (attributeValue * coefficients[i]);
        }
    }
    // Add intercept
    possibility = exp(possibility + intercept);

    // Compute possibility
    possibility = possibility / (1 + possibility);

    // Checks if possibility is above the chosen threshold
    if (possibility >= decisionBoundary) {
        results.emplace_back(1, possibility, dataContainer[0]->getValue<int>(data::Label));
    } else {
        results.emplace_back(0, possibility, dataContainer[0]->getValue<int>(data::Label));
    }
}

ArtificialNeuralNetwork::ArtificialNeuralNetwork() {}

DecisionTree::DecisionTree() {}

NaiveBayes::NaiveBayes() {}
