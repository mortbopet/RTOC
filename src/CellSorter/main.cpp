

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
    //image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    string imagePath = "/Users/eskidlbs/Desktop/data/ImgD1/";
    string textPath = "/Users/eskidlbs/Desktop/data/";

    //if (!image.data) {
    //    printf("No image data \n");
    //    return -1;
    //}

    //namedWindow("Display Image", WINDOW_AUTOSIZE);
    //imshow("Display Image", image);

    //waitKey(0);

    // Manual inputs:

    Setup testSetup;

    testSetup.inlet = 65;
    testSetup.outlet = 603;
    testSetup.yref = 52;
    testSetup.edge_thres = 0.272;
    testSetup.se_edge = getStructuringElement(MORPH_RECT, Size(1,30));
    testSetup.cellNum = 0;

    RBC test;

    // Gets accepted/discarded images
    string accepted, discarded, temp;
    AccDis patient1;    // Information about patient1
    Mat bg;             // Background
    patient1 = loadImageNames(textPath, patient1);
    // Selects background of images (comes from the two closest discarded)
    bg = selectBackground("SIMPLE", imagePath, patient1, bg);

    // Displays background
    // namedWindow("Display window", WINDOW_AUTOSIZE);
    // imshow("Display window", bg);
    // waitKey(0);

    // Subtracts background and edges
    Mat im;      //initialize frame
    string imgPath;
    // FOR LOOP BEGIN

    imgPath = imagePath + patient1.acceptedImages[0];    // Replace '0' with iteratable constant
    subtractBackground(imgPath, bg, im, testSetup);


    //accepted, discarded = loadImageNames(textPath);

    return 0;
}
