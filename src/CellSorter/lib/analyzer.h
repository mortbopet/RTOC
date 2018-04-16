#ifndef CELLSORTER_CSHELPER_H
#define CELLSORTER_CSHELPER_H
#include <fstream>
#include <iostream>
#include "experiment.h"
#include "framefinder.h"
#include "process.h"
#include "tracker.h"

#include <opencv/cv.hpp>

#include <functional>



class Analyzer {
public:
    void loadRBCPreset();
    processContainerPtr getProcessContainerPtr() { return &m_processes; }

    void loadExperimentPreset(const std::string& img_path);
    void loadImagesFromFolder();
    void loadImagesFromText();
    void selectBG();
    void runProcesses();
    void runAnalyzer();
    void resetProcesses();
    void showImg(const int& delay);

    void setImageGetterFunction(std::function<cv::Mat&(bool&)> function) {
        m_imageGetterFunction = function;
    }

    void findCells();

    bool storeData(const std::string& path);

    bool storeSetup(const std::string& path);
    bool loadSetup(const std::string& path);

    static void showImg(const cv::Mat& img, const int& delay);

    Experiment m_Experiment;  // CHECK IF THOSE CAN BE PRIVATE
    cv::Mat m_img;
    cv::Mat m_bg;

private:
    std::vector<std::unique_ptr<ProcessBase>> m_processes;

    std::vector<std::unique_ptr<DataContainer>> m_data;

    std::function<cv::Mat&(bool& sucessful)> m_imageGetterFunction;
};

#endif  // CELLSORTER_CSHELPER_H
