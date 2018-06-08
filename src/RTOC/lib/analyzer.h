#ifndef RTOC_CSHELPER_H
#define RTOC_CSHELPER_H
#include <algorithm>
#include <fstream>
#include <iostream>
#include "experiment.h"
#include "framefinder.h"
#include "machinelearning.h"
#include "objectfinder.h"
#include "process.h"
#include "setup.h"
#include "tracker.h"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include <opencv/cv.hpp>

#include <functional>

// When adding a status bit type, make sure to create a corresponding error message handler in
// ExperimentRunner::checkAnalyzerStatusMessage
enum StatusBits { UnknownError = 1 << 0, NoObjectsFound = 1 << 1 };

class Analyzer : public QObject {
    Q_OBJECT
public:
    processContainerPtr getProcessContainerPtr() { return &m_processes; }

    void loadImagesFromText();
    void setBG(const cv::Mat& bg);
    void runProcesses();
    void runAnalyzer(const Setup& setup);
    void setup(const Setup& setup);
    void writeImages(bool waitForFinish = true);
    void resetProcesses();
    void processSingleFrame(cv::Mat& img);
    void processSingleFrame(cv::Mat& img, cv::Mat& bg);
    int acquiredImagesCnt() { return m_imageCnt; }
    void stopAnalyzer();
    void reset();
    const int getStatus() const { return m_status; }

    void setImageGetterFunction(std::function<cv::Mat&(bool&)> function) {
        m_imageGetterFunction = function;
    }

    void findObjects();

    void exportExperiment(const std::string& path);

    bool storeSetup(const std::string& path);
    bool loadSetup(const std::string& path);

    long getSetupDataFlags();

    const Experiment* getExperiment();

    cv::Mat m_img;
    cv::Mat m_bg;

    // Serialization function for project storage
    template <class Archive>
    void save(Archive& ar, const unsigned int version) const {
        // note, version is always the latest when saving
        ar& BOOST_SERIALIZATION_NVP(m_processes);
    }
    template <class Archive>
    void load(Archive& ar, const unsigned int version) {
        m_processes.clear();
#ifndef __linux__  // https://stackoverflow.com/questions/50038329/serializing-stdvector-of-unique-ptr-using-boostserialization-fails-on-linux
        ar& BOOST_SERIALIZATION_NVP(m_processes);
#endif
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()

private:
    int m_status = 0;

    int m_imageCnt;

    void spinLockWait(int microseconds) const;
    void asyncStop();

    Setup m_setup;

    ObjectFinder* m_objectFinder;
    Experiment m_experiment;  // CHECK IF THOSE CAN BE PRIVATE

    bool m_asyncStopAnalyzer = false;  // called externally when analyzer should stop preliminarily

    void processImage(cv::Mat& img, cv::Mat& bg);

    std::vector<std::unique_ptr<ProcessBase>> m_processes;

    std::vector<std::unique_ptr<DataContainer>> m_data;  // experiment data here ? (JL, 17-04-18)

    std::function<cv::Mat&(bool& sucessful)> m_imageGetterFunction;
};

#endif  // RTOC_CSHELPER_H
