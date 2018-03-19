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

    std::pair<double,double> findCannyThreshold(const cv::Mat& img) {
        double percentOfPixelsNotEdges = 0.7;
        double thresholdRatio = 0.4;
        double low = percentOfPixelsNotEdges * img.rows * img.cols;
        // Initialize variables used by calcHist
        cv::Mat hist;
        int bins = 64;
        float range[] = {0, 256};
        const float* ranges = range;
        cv::calcHist(&img, 1, nullptr, cv::Mat(), hist, 1, &bins, &ranges);

        // Loop through histogram until threshold is met
        double highThreshold = 0;
        int c = 0;
        while(highThreshold < low) {
            highThreshold += hist.at<float>(0, c);
            c++;
        }
        std::pair<double,double> threshold;
        threshold.first = highThreshold / 64; //  * 16320;
        threshold.second = threshold.first * thresholdRatio;

        return threshold;
    };

}

#endif //CELLSORTER_MATLAB_EXT_H
