#ifndef CELLSORTER_CSHELPER_H
#define CELLSORTER_CSHELPER_H
#include <fstream>
#include <iostream>
#include "patient.h"
#include "process.h"

#include <opencv/cv.hpp>

class analyzer {
public:
    void loadRBCPreset();

    void loadPatientPreset(std::string img, std::string txt);

    void loadImageNames();

    void selectBG();

    void runProcesses();

    void showImg();

    patient m_patient;  // Struct of parameters
    cv::Mat m_img;
    cv::Mat m_bg;

private:
    std::vector<process*> m_processes;
};

#endif  // CELLSORTER_CSHELPER_H
