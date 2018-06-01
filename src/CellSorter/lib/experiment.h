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

    int inlet;        // Bottom corner of constriction inlet
    int outlet;       // Bottom corner of constriction outlet
    int cellNum = 0;  // Used for cell registration
    std::vector<framefinder::Frame> acc, dis, processed;
    std::vector<cv::Mat> rawBuffer;
    double intensity_threshold;
    std::vector<std::unique_ptr<DataContainer>> data;


    void reset() {
        acc.clear();
        dis.clear();
        processed.clear();
        rawBuffer.clear();
        data.clear();
    }
};

#endif  // EXPERIMENT_H

/* Experiment bør kun holde nogle vigtige meta-data for den pågældende type Experiment

class Experiment {
    std::vector<std::unique_ptr<ConditionBase>> m_conditions;   // Ikke egentig syntaks
                                                                // Holder listen af conditions der
gør at et objekt bliver slettet
                                                                //  - Kunne teknisk set godt ligge
under analyzer


}

class RBC : Experiment {

}

*/
