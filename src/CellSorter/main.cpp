#include <iostream>

//#include "rbc.h"
#include "CShelper.h"
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;
using namespace CS;

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }

    Mat image;
    //image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    image = imread("/Users/eskidlbs/Desktop/data/ImgD1/081116D1_28932.png", CV_LOAD_IMAGE_COLOR);

    if (!image.data) {
        printf("No image data \n");
        return -1;
    }

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", image);

    waitKey(0);

    // Manual inputs:
    int inlet = 65;             // Bottom corner of constriction inlet
    int outlet = 603;           // Bottom corner of constriction outlet
    int yref = 52;              // Vertical center of inlet
    float edge_thres = 0.272;   // Threshold for extracting channel edge
    cv::Mat se_edge = cv::getStructuringElement(MORPH_RECT, cv::Size(1,20));    // SE for extracting channel edge
                                                                                // (SE = structuring element)
    int cellNum = 0;            // Used for cell registration
    int tracker[] = {};         // Used for cell registration

    RBC test;

    //bg =

    return 0;
}
