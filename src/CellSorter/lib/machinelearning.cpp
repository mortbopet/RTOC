#include "machinelearning.h"

#include <iostream>
#include <fstream>
#include <string>

Machinelearning::Machinelearning() {}

void Machinelearning::loadModel(const std::string &path) {
    std::string line;
    std::ifstream file(path);
    int counter = 0;
    while ( std::getline (file, line, ',') ) {
        if (counter%3 == 0) {
            xpos.push_back(std::stoi(line));
        } else if (counter%3 == 1) {
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

int Machinelearning::findClosestXpos(const int& pos, DataContainer& dataContainer) {
    for (int i = 0; i < 0xff; i ++) {
        int index = 0;
        for (const auto& object : dataContainer) {
            auto centroid = object->getValue<cv::Point>(data::Centroid);
            if (( centroid.x > (pos-i)) && (centroid.x < (pos+i) )) {
                return index;
            }
            index += 1;
        }
    }
}

double Machinelearning::predictContainer(DataContainer& dataContainer) {
    double returnValue = 0;
    for (int i = 0; i < xpos.size(); i++) {
        // First, find the object with the given x-position
        int index = findClosestXpos(xpos[i], dataContainer);

        // Next, extract the right attribute and multiply its coefficient onto the model
        // Make arguement change with attribute-type
        auto attributeValue = dataContainer[index]->getValue<double>(attributes[i]);
        returnValue += (attributeValue * coefficients[i]);
    }
    return returnValue;
}