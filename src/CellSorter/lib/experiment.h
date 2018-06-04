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
class Experiment {
public:
    Experiment() {}
    ~Experiment() {}

    int inlet = 80;
    int outlet = 210;
    int cellNum = 0;  // Used for cell registration
    std::vector<cv::Mat> processed;
    std::vector<cv::Mat> rawBuffer;
    double intensity_threshold;
    std::vector<std::unique_ptr<DataContainer>> data;


    void reset() {
        processed.clear();
        rawBuffer.clear();
        data.clear();
    }
};

#endif  // EXPERIMENT_H
