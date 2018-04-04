#ifndef CELLSORTER_CSHELPER_H
#define CELLSORTER_CSHELPER_H
#include <fstream>
#include <iostream>
#include "experiment.h"
#include "framefinder.h"
#include "process.h"

#include <opencv/cv.hpp>

class Analyzer {
public:
    void loadRBCPreset();
    std::vector<ProcessBase*>* getProcessContainerPtr() { return &m_processes; }

    void loadExperimentPreset(const std::string& img_path);

    void loadImageNames();

    void selectBG();

    void runProcesses();

    void resetProcesses();

    void showImg(const int& delay);

    static void showImg(const cv::Mat& img, const int& delay);

    Experiment m_Experiment;  // CHECK IF THOSE CAN BE PRIVATE
    cv::Mat m_img;
    cv::Mat m_bg;

private:
    std::vector<ProcessBase*> m_processes;
};

#endif  // CELLSORTER_CSHELPER_H
