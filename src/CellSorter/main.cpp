#include "analyzer.cpp"
#include "process.cpp"
#include <opencv/cv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>

using namespace cv;
using namespace std;

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    // Constructing struct for properties of test setup.
    Properties testSetup;

    testSetup.inlet = 65;
    testSetup.outlet = 603;
    testSetup.yref = 52;
    testSetup.edge_thres = 0.272*15;
    testSetup.se_edge = getStructuringElement(MORPH_RECT, Size(1,30));
    testSetup.se_RBC = getStructuringElement(MORPH_ELLIPSE, Size(15,15));
    testSetup.se_noiseremoval = getStructuringElement(MORPH_ELLIPSE, Size(2,2));
    testSetup.cellNum = 0;
    testSetup.imagePath = "/Users/eskidlbs/Desktop/data/ImgD1/";
    testSetup.textPath = "/Users/eskidlbs/Desktop/data/";

    testSetup.loadImageNames();
    testSetup.selectBackground("SIMPLE", testSetup.bg);

    analyzer test1;
    analyzer::IMG_PROCESS_TYPES testList = analyzer::IMG_PROCESS_TYPES::PRESET;
    test1.IMG_PROCESS_TYPES =

    // Subtracts background and edges
    string imgPath; // initialize path to string

    //int temp = 0;
    //while (patient1.acceptedImages.back() != patient1.acceptedImages[temp]) { // Goes through all accepted images
    //    temp += 1;
    //    imgPath = imagePath + patient1.acceptedImages[temp];
    //    subtractBackground(imgPath, bg, im, testSetup);
    //}
    //return 0;
}
