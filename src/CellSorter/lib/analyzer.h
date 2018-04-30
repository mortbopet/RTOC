#ifndef CELLSORTER_CSHELPER_H
#define CELLSORTER_CSHELPER_H
#include <algorithm>
#include <fstream>
#include <iostream>
#include "experiment.h"
#include "framefinder.h"
#include "objectfinder.h"
#include "process.h"
#include "tracker.h"

#include <opencv/cv.hpp>

#include <functional>

struct Setup {
    bool runProcessing;
    bool extractData;
    bool storeRaw;
    bool storeProcessed;
    long dataFlags = 0;
    std::string rawPrefix;
    std::string processedPrefix;
    std::string outputPath;
    std::string experimentName;
};

class Analyzer : public QObject {
    Q_OBJECT
public:
    processContainerPtr getProcessContainerPtr() { return &m_processes; }

    void loadExperimentPreset(const std::string& img_path);
    void loadImagesFromFolder();
    void loadImagesFromText();
    void setBG(const cv::Mat& bg);
    void selectBG();
    void runProcesses();
    void runAnalyzer(Setup setup);
    void writeImages();
    void resetProcesses();
    void processSingleFrame(cv::Mat& img);
    void processSingleFrame(cv::Mat& img, cv::Mat& bg);
    void stopAnalyzer() { m_asyncStopAnalyzer = true; }
    void resetAnalyzer();

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

    Experiment m_experiment;  // CHECK IF THOSE CAN BE PRIVATE
    cv::Mat m_img;
    cv::Mat m_bg;
    int m_currentProcessingFrame;

private:
    void spinLockWait(int microseconds) const;

    Setup m_setup;

    ObjectFinder m_objectFinder;

    bool m_asyncStopAnalyzer = false;  // called externally when analyzer should stop preliminarily

    void processImage(cv::Mat& img, cv::Mat& bg);

    std::vector<std::unique_ptr<ProcessBase>> m_processes;

    std::vector<std::unique_ptr<DataContainer>> m_data;  // experiment data here ? (JL, 17-04-18)

    std::function<cv::Mat&(bool& sucessful)> m_imageGetterFunction;
};

#endif  // CELLSORTER_CSHELPER_H
