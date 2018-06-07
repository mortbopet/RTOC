#ifndef RTOC_OBJECTFINDER_H
#define RTOC_OBJECTFINDER_H

#include <algorithm>
#include <vector>

#include "opencv/cv.hpp"

#include "experiment.h"
#include "framefinder.h"
#include "mathlab.h"
#include "setup.h"
#include "tracker.h"

class ObjectFinder {
public:
    ObjectFinder();

    int findObjects(Experiment& experiment, const Setup& setup);

    unsigned long cleanObjects(Experiment& e);

    void setImages(const cv::Mat* raw, const cv::Mat* processed);

    void setConditions(const Experiment& experiment);

    void reset();

private:
    Tracker m_track;
    std::vector<Tracker> m_trackerList, m_frameTracker;

    const cv::Mat* m_processedImg;
    const cv::Mat* m_rawImg;
    int m_cellNum = 0;
    int m_frameNum = 0;
    bool m_newObject = false;
    int m_numObjects = 0;
    unsigned long m_dataFlags = 0;
    double m_dist;
    double m_distThreshold;
    cv::Point m_centroid;
    int m_countThreshold = 25;
    DataContainer m_cc;

    std::vector<std::function<bool(const DataContainer*)>> m_conditions;

    std::pair<double, Tracker> findNearestObject(const cv::Point& object,
                                                 const std::vector<Tracker>& listOfObjects);
    void writeToDataVector(const int& index, Experiment& experiment);
};

#endif  // RTOC_OBJECTFINDER_H
