#ifndef RTOC_OBJECTFINDER_H
#define RTOC_OBJECTFINDER_H

#include <algorithm>
#include <functional>
#include <vector>

#include "opencv/cv.hpp"

#include "experiment.h"
#include "framefinder.h"
#include "mathlab.h"
#include "setup.h"
#include "tracker.h"

// --------------------- ObjectHandler ---------------------
namespace {
class ObjectHandler {
    // --------------------- Conditions --------------------
public:
    /*
     *  All conditions should have a corresponding enum,
     *  and the only public member related to the condition methods
     *  should be this enum
     */
    enum Conditions {
        FrameCount = 1 << 0,
        FrameBeforeInlet = 1 << 1,
        FrameAfterOutlet = 1 << 2,
        AllConditions = 0x7
    };

private:
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

    /**
     * Legibility of found-object based on image-count
     * @return
     */
    static bool frameCount(const ObjectHandler* handler, const DataContainer* dc) {
        return (*dc).size() < (*handler).m_setup->countThreshold;
    }
    /**
     * Legibility of found-object based on whether first frame of object has object before inlet
     * @return
     */
    static bool frameBeforeInlet(const ObjectHandler* handler, const DataContainer* dc) {
        auto e = (*handler).m_experiment;   // Pointer to Experiment

        auto xpos = (*dc).front()->template getValue<double>(data::RelativeXpos);
        return xpos > 0;
    }
    /**
     * Legibility of found-object based on whether last frame of object has object after outlet
     * @return
     */
    static bool frameAfterOutlet(const ObjectHandler* handler, const DataContainer* dc) {
        auto e = (*handler).m_experiment;   // Pointer to Experiment

        auto centroid = (*dc).back()->template getValue<cv::Point>(data::Centroid);
        auto xpos = mathlab::relativeX(centroid, e->outlet_line);
        return xpos < 0;
    }

    // ------------ Class methods and variables ------------
public:
    explicit ObjectHandler(Experiment* experiment, Setup* setup,
                           unsigned long conditionFlags = Conditions::AllConditions);

    bool invoke_all(const DataContainer* dc) {
        for (auto&& fn : m_conditionFunctions) {
            if (fn(this, dc)) {
                return true;
            }
        }
        return false;
    }

private:
    // Class members
    Experiment* m_experiment;
    Setup* m_setup;
    unsigned long m_conditionFlags = 0;

    void setup();

    template <typename Function>
    void add(Function&& fn) {
        m_conditionFunctions.emplace_back(std::forward<Function>(fn));
    }

    std::vector<std::function<bool(const ObjectHandler*, const DataContainer* dc)>>
        m_conditionFunctions;
};
}  // namespace

// ---------------------- ObjectFinder ---------------------
class ObjectFinder {
public:
    ObjectFinder(Experiment* experiment, Setup* setup);

    int findObjects();

    void startThread();
    void waitForThreadToFinish(int targetImageCount);

    bool approveContainer(const DataContainer& dataContainer);

    unsigned long cleanObjects();

    void reset();

    Experiment* m_experiment;
    Setup* m_setup;

    void forceStop() { m_forceStop = true; }

private:
    void findObjectsThreaded();

    ObjectHandler* handler;

    Tracker m_track;
    std::vector<Tracker> m_trackerList, m_frameTracker;

    cv::Mat m_processedImg;
    cv::Mat m_rawImg;
    int m_cellNum = 0;
    int m_frameNum = 0;
    bool m_newObject = false;
    int m_numObjects = 0;
    unsigned long m_dataFlags = 0;
    double m_dist;
    double m_distThreshold;
    cv::Point m_centroid;
    double m_xpos;
    DataContainer m_cc;

    std::pair<double, Tracker> findNearestObject(const cv::Point& object,
                                                 const std::vector<Tracker>& listOfObjects);
    void writeToDataVector(const int& index, Experiment& experiment);

    // Concurrency
    long m_targetImageCount;
    bool m_running = false;
    bool m_forceStop = false;
    volatile bool m_finishedWriting;
};

#endif  // RTOC_OBJECTFINDER_H
