#ifndef RTOC_OBJECTFINDER_H
#define RTOC_OBJECTFINDER_H

#include <algorithm>
#include <vector>
#include <functional>

#include "opencv/cv.hpp"

#include "experiment.h"
#include "framefinder.h"
#include "mathlab.h"
#include "setup.h"
#include "tracker.h"


// --------------------- ObjectHandler ---------------------
namespace {
class ObjectHandler {
public:
    ObjectHandler(Experiment *experiment);

    void setup(long handleFlags);

    bool invoke_all(const DataContainer *dc) {
        for (auto &&fn : m_conditionFunctions) {
            if (fn(this, dc)) {
                return true;
            }
        }
        return false;
    }

private:
    // Class members - do not edit
    Experiment *m_experiment;

    template<typename Function>
    void add(Function &&fn) {
        m_conditionFunctions.emplace_back(std::forward<Function>(fn));
    }

    std::vector<std::function<bool(const ObjectHandler *, const DataContainer *dc)>> m_conditionFunctions;

// -------------------- CONDITIONS --------------------
public:
    /*
     *  All conditions should have a corresponding enum,
     *  and the only public member related to the condition methods
     *  should be this enum
     */
    enum Conditions {
        FrameCount = 1 << 0,
        FrameBeforeInlet = 1 << 1,
        FrameAfterOutlet = 1 << 2
    };


private:
// Decision parameters

    int m_countThreshold = 25;


// Decision methods/functions

    /*  Are required to follow the template:
     *  static bool condition(const ObjectHandler* handler, const DataContainer* dc) {
     *      // Some statement determing wether object is accepted or rejected
     *      // Example:
     *      if ((*dc).size() < m_countThreshold) {
     *          return true;
     *      }
     *      return false;
     *  }
     *
     */
    static bool frameCount(const ObjectHandler *handler, const DataContainer *dc) {
        return (*dc).size() < (*handler).m_countThreshold;
    }

    static bool frameBeforeInlet(const ObjectHandler *handler, const DataContainer *dc) {
        auto e = (*handler).m_experiment;
        cv::Rect bb_i = (*dc).front()->template getValue<cv::Rect>(data::BoundingBox);
        return ((bb_i.x + bb_i.width) > e->inlet - 1);
    }

    static bool frameAfterOutlet(const ObjectHandler *handler, const DataContainer *dc) {
        auto e = (*handler).m_experiment;
        cv::Rect bb_o = (*dc).back()->template getValue<cv::Rect>(data::BoundingBox);
        return ((bb_o.x + bb_o.width) < e->outlet);
    }
};
} // namespace

// --------------------- ObjectFinder ---------------------
class ObjectFinder {
public:
    ObjectFinder(Experiment* experiment, Setup* setup);

    int findObjects();
    void runThreaded();

    bool handleObject(const DataContainer& dataContainer);

    unsigned long cleanObjects();

    void setImages(const cv::Mat* raw, const cv::Mat* processed);

    void reset();

    Experiment* m_experiment;
    Setup* m_setup;

    void forceStop() { m_forceStop = true; }

private:



    ObjectHandler* handler;

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
    DataContainer m_cc;

    std::pair<double, Tracker> findNearestObject(const cv::Point& object, const std::vector<Tracker>& listOfObjects);
    void writeToDataVector(const int& index, Experiment& experiment);

    // Concurrency
    bool m_forceStop = false;
};








#endif  // RTOC_OBJECTFINDER_H
