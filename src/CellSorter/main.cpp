

//#include "rbc.h"
#include "CShelper.h"
#include <opencv/cv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>

using namespace cv;
using namespace CS;
using namespace std;

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    string imagePath = "/Users/eskidlbs/Desktop/data/ImgD1/";
    string textPath = "/Users/eskidlbs/Desktop/data/";

    // Manual inputs:

    Properties testSetup;

    testSetup.inlet = 65;
    testSetup.outlet = 603;
    testSetup.yref = 52;
    testSetup.edge_thres = 0.272*15;
    testSetup.se_edge = getStructuringElement(MORPH_RECT, Size(1,30));
    testSetup.se_RBC = getStructuringElement(MORPH_ELLIPSE, Size(15,15));
    testSetup.se_noiseremoval = getStructuringElement(MORPH_ELLIPSE, Size(2,2));
    testSetup.cellNum = 0;

    RBC test;

    // Gets accepted/discarded images
    string accepted, discarded;
    AccDis patient1;    // Information about patient1
    Mat bg;             // Background
    patient1 = loadImageNames(textPath, patient1);
    // Selects background of images (comes from the two closest discarded)
    bg = selectBackground("SIMPLE", imagePath, patient1, bg);


    // Subtracts background and edges
    Mat im;         // initialize frame
    string imgPath; // initialize path to string

    int temp = 0;
    while (patient1.acceptedImages.back() != patient1.acceptedImages[temp]) { // Goes through all accepted images
        temp += 1;
        imgPath = imagePath + patient1.acceptedImages[temp];
        subtractBackground(imgPath, bg, im, testSetup);
    }
    return 0;
}
