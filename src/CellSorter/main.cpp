#include <stdio.h>
#include <fstream>
#include <opencv/cv.hpp>
#include "lib/analyzer.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

    //// Set path to image data and to _Accepted.txt and _Discarded.txt
    string localImagePath = "../../../data/ImgD1/";
    string localTextPath = "../../../data/";

    // Initializing
    analyzer testAnalyze;

    // Sets Experiment to default. More functions to come to change parameters
    testAnalyze.loadExperimentPreset(localImagePath, localTextPath);

    // Loads lists of images and gets background
    testAnalyze.loadImageNames();
    testAnalyze.selectBG();

    //// CHOSE OPTION FOR OPERATION
    int option = 2;  // temporary solution
    int n = 0;

    // OPTION 1: Load 1 picture
    if (option == 1) {
        int testPicture = 65;  // Number of accepted picture to show
        testAnalyze.m_img = testAnalyze.m_Experiment.acc[testPicture].image;

        // Loads and runs commands
        testAnalyze.loadRBCPreset();
        testAnalyze.runProcesses();
        testAnalyze.showImg();
    } else if (option == 2) {
        // OPTION 2: Loop through all pictures
        testAnalyze.loadRBCPreset();
        while (testAnalyze.m_Experiment.acc.back().filename != testAnalyze.m_Experiment.acc[n].filename) {
            n += 1;
            // cout << n << endl;
            testAnalyze.m_img = testAnalyze.m_Experiment.acc[n].image;
            testAnalyze.runProcesses();

            Mat canny_output;
            vector<vector<Point>> contours;
            vector<Vec4i> hierarchy;
            Canny(testAnalyze.m_img, canny_output, 10, 20);
            findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE,
                         Point(0, 0));

            /// Get the moments
            vector<Moments> mu(contours.size());
            for (int i = 0; i < contours.size(); i++) {
                mu[i] = moments(contours[i], false);
            }

            vector<Point2f> mc(contours.size());
            for (int i = 0; i < contours.size(); i++) {
                mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
            }

            Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
            Scalar color = Scalar(255, 255, 255);

            for (int i = 0; i < contours.size(); i++) {
                drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
                circle(drawing, mc[i], 4, color, -1, 8, 0);
            }

            // length = contour perimeter

            /// Calculate the area with the moments 00 and compare with the result of the OpenCV
            /// function
            printf("\t Info: Area and Contour Length \n");
            for (int i = 0; i < contours.size(); i++) {
                printf(" * Contour[%d] - Area (M_00) = %.2f - Area OpenCV: %.2f - Length: %.2f \n",
                       i, mu[i].m00, contourArea(contours[i]), arcLength(contours[i], true));
                //drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
                //circle(drawing, mc[i], 4, color, -1, 8, 0);
            }

            namedWindow("Contours", WINDOW_AUTOSIZE);
            imshow("Contours", drawing);
            waitKey(1);

            // testAnalyze.showImg();
        }
    }
}
