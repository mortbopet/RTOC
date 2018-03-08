

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
    int inlet = 65;             // Bottom corner of constriction inlet
    int outlet = 603;           // Bottom corner of constriction outlet
    int yref = 52;              // Vertical center of inlet
    float edge_thres = 0.272;   // Threshold for extracting channel edge
    Mat se_edge = getStructuringElement(MORPH_RECT, Size(1,20));    // SE for extracting channel edge (SE = structuring element).
    int cellNum = 0;            // Used for cell registration
    int tracker[] = {};         // Used for cell registration

    RBC test;

    // Gets accepted/discarded images
    string accepted, discarded, temp;
    AccDis patient1;    // Information about patient1
    Mat bg;             // Background
    patient1 = loadImageNames(textPath, patient1);

    // Selects background of images (comes from the two closest discarded)
    bg = selectBackground("SIMPLE", imagePath, patient1, bg);

    // Displays background
    namedWindow("Display window", WINDOW_AUTOSIZE);
    imshow("Display window", bg);
    waitKey(0);

    // Subtracts background and edges





    //accepted, discarded = loadImageNames(textPath);

    return 0;
}
