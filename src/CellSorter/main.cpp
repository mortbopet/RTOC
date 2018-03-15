#include <stdio.h>
#include <fstream>
#include <opencv/cv.hpp>
#include "lib/analyzer.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    //// Set path to image data and to _Accepted.txt and _Discarded.txt
    string localImagePath = "../../../data/ImgD1_cut";
    string localTextPath = "../../../data/";

    // Initializing
    analyzer testAnalyze;

    // Sets Experiment to default. More functions to come to change parameters
    testAnalyze.loadExperimentPreset(localImagePath);

    // Loads lists of images and gets background
    testAnalyze.loadImageNames();
    testAnalyze.selectBG();

    //// CHOSE OPTION FOR OPERATION
    int n = 0;

    // OPTION 2: Loop through all pictures
    testAnalyze.loadRBCPreset();
    while (testAnalyze.m_Experiment.acc.back().filename !=
           testAnalyze.m_Experiment.acc[n+1].filename) {
        n += 1;
        cv::Mat buf_img = testAnalyze.m_Experiment.acc[n+1].image;
        testAnalyze.m_img = testAnalyze.m_Experiment.acc[n].image;
        testAnalyze.runProcesses();
        cv::hconcat(testAnalyze.m_img, buf_img, testAnalyze.m_img);
        testAnalyze.showImg();

    }
    cv::waitKey(0);

    return 0;
}
