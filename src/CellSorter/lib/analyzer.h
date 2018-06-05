#ifndef CELLSORTER_CSHELPER_H
#define CELLSORTER_CSHELPER_H
#include <algorithm>
#include <fstream>
#include <iostream>
#include "experiment.h"
#include "framefinder.h"
#include "machinelearning.h"
#include "objectfinder.h"
#include "process.h"
#include "tracker.h"
#include "machinelearning.h"
#include "setup.h"

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/vector.hpp>

#include <opencv/cv.hpp>

#include <functional>


class Analyzer : public QObject {
    Q_OBJECT
public:
    processContainerPtr getProcessContainerPtr() { return &m_processes; }

    void loadImagesFromText();
    void setBG(const cv::Mat& bg);
    void selectBG();
    void runProcesses();
    void runAnalyzer(Setup setup);
    void writeImages();
    void resetProcesses();
    void processSingleFrame(cv::Mat& img);
    void processSingleFrame(cv::Mat& img, cv::Mat& bg);
    void stopAnalyzer();
    void resetAnalyzer();
    const int getStatus() const { return m_status; }

    void setImageGetterFunction(std::function<cv::Mat&(bool&)> function) {
        m_imageGetterFunction = function;
    }

    void findObjects();
    void cleanObjects();

    void exportExperiment(const std::string& path);

    bool storeSetup(const std::string& path);
    bool loadSetup(const std::string& path);
    bool storeData(const std::string& path);

    void showImg(const int& delay);
    static void showImg(const cv::Mat& img, const int& delay);

    long getSetupDataFlags();

    Experiment m_experiment;  // CHECK IF THOSE CAN BE PRIVATE
    cv::Mat m_img;
    cv::Mat m_bg;
    int m_currentProcessingFrame;

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

    void spinLockWait(int microseconds) const;
    void asyncStop();

    Setup m_setup;

    ObjectFinder m_objectFinder;

    bool m_asyncStopAnalyzer = false;  // called externally when analyzer should stop preliminarily

    void processImage(cv::Mat& img, cv::Mat& bg);

    std::vector<std::unique_ptr<ProcessBase>> m_processes;

    std::vector<std::unique_ptr<DataContainer>> m_data;  // experiment data here ? (JL, 17-04-18)

    std::function<cv::Mat&(bool& sucessful)> m_imageGetterFunction;
};

#endif  // CELLSORTER_CSHELPER_H
