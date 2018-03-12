#ifndef CELLSORTER_CSHELPER_H
#define CELLSORTER_CSHELPER_H
#include <fstream>
#include <iostream>
#include "Experiment.h"
#include "Process.h"

#include <opencv/cv.hpp>

class analyzer {
public:
    void loadRBCPreset();

    void loadPatientPreset(std::string img, std::string txt);

    void loadImageNames();

    void selectBG();

    void runProcesses();

    void resetProcesses();

    void showImg();

    Experiment m_Experiment;  // CHECK IF THOSE CAN BE PRIVATE
    cv::Mat m_img;
    cv::Mat m_bg;

private:
    std::vector<Process*> m_processes;
};

#endif  // CELLSORTER_CSHELPER_H
