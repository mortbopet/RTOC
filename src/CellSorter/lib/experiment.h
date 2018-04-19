#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <opencv/cv.hpp>
#include <string>
#include <vector>

#include "datacontainer.h"
#include "framefinder.h"

/** @brief Struct for each Experiment. Used as a container for parameters.
 * More functions can be created to change those parameters
 * or set different presets.
 */
struct Experiment {
    int inlet;        // Bottom corner of constriction inlet
    int outlet;       // Bottom corner of constriction outlet
    int cellNum = 0;  // Used for cell registration
    std::vector<framefinder::Frame> acc, dis, processed;
    double intensity_threshold;
    std::string imagePath;
    std::vector<std::unique_ptr<DataContainer>> data;

    void defaultSettings(const std::string& imgPath) {
        inlet = 80;
        outlet = 200;
        intensity_threshold = 0.0354;
        imagePath = imgPath;
    }
};

#endif  // EXPERIMENT_H
