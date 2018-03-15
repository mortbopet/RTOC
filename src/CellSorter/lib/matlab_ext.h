#ifndef CELLSORTER_MATLAB_EXT_H
#define CELLSORTER_MATLAB_EXT_H

#include <opencv/cv.hpp>
namespace matlab {
    void bwareaopen(cv::Mat &im, double size) {
        // Only accept CV_8UC1
        if (im.channels() != 1 || im.type() != CV_8U) {
            return;
        }

        // Find all contours
        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(im.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < contours.size(); i++) {
            // Calculate contour area
            double area = cv::contourArea(contours[i]);

            // Remove small objects by drawing the contour with black color
            if (area <= size) {
                cv::drawContours(im, contours, i, CV_RGB(0, 0, 0), -1);
            }
        }
    }
}

#endif //CELLSORTER_MATLAB_EXT_H
