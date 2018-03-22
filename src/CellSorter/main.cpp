#include <cstdio>
#include <fstream>
#include <opencv/cv.hpp>
#include "lib/analyzer.h"


int main(int argc, char** argv) {
    Analyzer analyzer;

    // Get path to pictures
    analyzer.loadExperimentPreset("../../../data/ImgD1/");

    // Loop through all pictures to reject or accept
    analyzer.loadImageNames();

    // Select Background
    analyzer.selectBG();

    // Load processes preset
    analyzer.loadRBCPreset();

    cv::Mat temp0, temp1;
    for (const Frame& frame : analyzer.m_Experiment.acc) {
        temp0 = frame.image.clone();
        analyzer.m_img = frame.image;
        analyzer.runProcesses();
        cv::hconcat(temp0, analyzer.m_img, temp0);
        Analyzer::showImg(temp0, 1);
    }



    return 0;
}
