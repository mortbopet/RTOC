#ifndef CELLSORTER_PROPERTIES_H
#define CELLSORTER_PROPERTIES_H

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <opencv/cv.hpp>
#include <sstream>
#include <string>

//// Experient
/* Struct for each Experiment. Used as a container for parameters.
 * More functions can be created to change those parameters
 * or set different presets.
 */

struct Experiment {
    int inlet;    // Bottom corner of constriction inlet
    int outlet;   // Bottom corner of constriction outlet
    int yref;     // Vertical center of inlet
    int cellNum;  // Used for cell registration
    // int tracker[] = {}; // Used for cell registration
    float edge_thres;         // Threshold for extracting channel edge
    cv::Mat se_edge;          // SE for extracting channel edge (SE = structuring element).
    cv::Mat se_RBC;           // SE for extracting channel edge
    cv::Mat se_noiseremoval;  // SE for removing noise
    std::vector<std::string> acc, dis;
    std::string imagePath, textPath;  // Holds information about path to images and to _Accepted.txt and _Discarded.txt

    void defaultSettings(std::string imgPath, std::string checkPath) {
        inlet = 65;
        outlet = 603;
        yref = 52;
        edge_thres = 0.272 * 15;
        se_edge = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 30));
        se_RBC = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(15, 15));
        se_noiseremoval = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2, 2));
        cellNum = 0;
        imagePath = imgPath;
        textPath = checkPath;
    }
};

#endif  // CELLSORTER_PROPERTIES_H
