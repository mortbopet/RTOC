#include <stdio.h>
#include <fstream>
#include <opencv/cv.hpp>
#include "analyzer.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    //// Set path to image data and to _Accepted.txt and _Discarded.txt
    string localImagePath = "/Users/eskidlbs/Desktop/data/ImgD1/";
    string localTextPath = "/Users/eskidlbs/Desktop/data/";

    // Initializing
    analyzer testAnalyze;

    // Sets patient to default. More functions to come to change parameters
    testAnalyze.loadPatientPreset(localImagePath, localTextPath);

    // Loads lists of images and gets background
    testAnalyze.loadImageNames();
    testAnalyze.selectBG();

    //// CHOSE OPTION FOR OPERATION
    int option = 1; // temporary solution

    // OPTION 1: Load 1 picture
    if (option == 1) {
        int testPicture = 65;  // Number of accepted picture to show
        testAnalyze.m_img =
            imread(testAnalyze.patient.imagePath + testAnalyze.patient.acc[testPicture], IMREAD_GRAYSCALE);

        // Loads and runs commands
        testAnalyze.loadRBCPreset();
        testAnalyze.runProcesses();
        testAnalyze.showImg();
    } else if (option == 2) {
        // OPTION 2: Loop through all pictures
        int n = 0;
        while (testAnalyze.patient.acc.back() != testAnalyze.patient.acc[n]) {
            n += 1;
            cout << n << endl;
            testAnalyze.m_img = imread(testAnalyze.patient.imagePath + testAnalyze.patient.acc[n], IMREAD_GRAYSCALE);
            testAnalyze.loadRBCPreset();
            testAnalyze.runProcesses();
            testAnalyze.showImg();
        }
    }
}
