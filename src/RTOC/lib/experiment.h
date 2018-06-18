#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <opencv/cv.hpp>
#include <string>
#include <vector>

#include "datacontainer.h"
#include "framefinder.h"
#include "imagewriter.h"
#include "mathlab.h"

#include "helper.h"

#ifndef NDEBUG
#define NDEBUG
#endif
#include "../external/readerwriterqueue/readerwriterqueue.h"

/** @brief Struct for each Experiment. Used as a container for parameters.
 * More functions can be created to change those parameters
 * or set different presets.
 */

class Experiment {
public:
    Experiment() {}
    ~Experiment() {}

    mathlab::Line inlet_line;
    mathlab::Line outlet_line;

    // Queues containing processed and raw images
    moodycamel::BlockingReaderWriterQueue<cv::Mat> processed;
    moodycamel::BlockingReaderWriterQueue<cv::Mat> raw;

    // ImageWriters are used for writing images to disk after analyzer is done with the images
    ImageWriter writeBuffer_raw;
    ImageWriter writeBuffer_processed;

    long m_currentProcessingFrame = 0;

    // Vector containing found objects
    std::vector<std::unique_ptr<DataContainer>> data;

    void reset() {
        clearCamel(processed);
        clearCamel(raw);
        writeBuffer_processed.clear();
        writeBuffer_raw.clear();
        data.clear();
        m_currentProcessingFrame = 0;
    }

    void setInletOutletLines(std::pair<int, int>& inlet, std::pair<int, int>& outlet) {
        if (inlet.second - outlet.second == 0) {
            inlet_line.straight = true;
            inlet_line.x = inlet.first;
            outlet_line.straight = true;
            outlet_line.x = inlet.second;
        } else {
            inlet_line.m = -((double) (inlet.first - outlet.first) / (inlet.second - outlet.second));
            inlet_line.q = (-inlet_line.m) * inlet.first + inlet.second;
            outlet_line.m = inlet_line.m;
            outlet_line.q = (-outlet_line.m) * outlet.first + outlet.second;
        }
    }
};

#endif  // EXPERIMENT_H
