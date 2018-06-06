#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <opencv/cv.hpp>
#include <queue>
#include <string>
#include <vector>

#include "datacontainer.h"
#include "framefinder.h"
#include "imagewriter.h"

#include "helper.h"

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
    std::queue<cv::Mat> processed;
    std::queue<cv::Mat> raw;

    // ImageWriters are used for writing images to disk after analyzer is done with the images
    ImageWriter writeBuffer_raw;
    ImageWriter writeBuffer_processed;

    double intensity_threshold;
    std::vector<std::unique_ptr<DataContainer>> data;

    void reset() {
        clearQueue(processed);
        clearQueue(raw);
        writeBuffer_processed.clear();
        writeBuffer_raw.clear();
        data.clear();
    }
};

#endif  // EXPERIMENT_H
