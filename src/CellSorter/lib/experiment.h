#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <opencv/cv.hpp>
#include <string>
#include <vector>

#include "framefinder.h"
#include "datacontainer.h"

//// Experiment
/* Struct for each Experiment. Used as a container for parameters.
 * More functions can be created to change those parameters
 * or set different presets.
 */

struct Experiment {
    //// Setting range for min and max values of accepted RBC's
    int convex_MIN{}; // min value of convex area to be accepted
    int convex_MAX{}; // max value of convex area to be accepted
    int majorAxisLength_MIN{}; // min value of major axis length to be accepted
    int majorAxisLength_MAX{}; // max value of major axis length to be accepted
    double solidity_MIN{}; // min value of solidity of the RBC to be accepted
    double solidity_MAX{}; // max value of solidity of the RBC to be accepted. This should be left as 1

    int inlet{};    // Bottom corner of constriction inlet
    int outlet{};   // Bottom corner of constriction outlet
    int cellNum{};  // Used for cell registration
    std::vector<Frame> acc{}, dis{}, processed{};
    double intensity_threshold{};
    std::string imagePath;
    std::vector<std::unique_ptr<DataContainer>> data;

    void defaultSettings(const std::string& imgPath) {
        convex_MIN = 200;
        convex_MAX = 1450;
        majorAxisLength_MIN = 0;
        majorAxisLength_MAX = 65;
        solidity_MIN = 0.87;
        solidity_MAX = 1;

        inlet = 80;
        outlet = 200;
        cellNum = 0;
        intensity_threshold = 0.0354;
        imagePath = imgPath;
    }

    void setConvex_Range(int min, int max) {
        convex_MIN = min;
        convex_MAX = max;
    }

    void setMajorAxisLength_Range(int min, int max) {
        majorAxisLength_MIN = min;
        majorAxisLength_MAX = max;
    }

    void setSolidity_Range(int min, int max) {
        solidity_MIN = min;
        solidity_MAX = max;
    }
};

#endif  // EXPERIMENT_H
