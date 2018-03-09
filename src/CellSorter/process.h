#ifndef CELLSORTER_PROCESS_H
#define CELLSORTER_PROCESS_H

#include <fstream>
#include <iostream>
#include <opencv/cv.hpp>
#include "patient.h"

class process {
public:
    process();  // Constructor

    virtual void doProcessing(cv::Mat& img, cv::Mat& bg, patient props) = 0;  // General function for doing processing.
};

class erosion : public process {
public:
    void doProcessing(cv::Mat& img, cv::Mat& bg, patient props) override;

    void setBorderType(cv::BorderTypes type);
};

class dilation : public process {
    void doProcessing(cv::Mat& img, cv::Mat& bg, patient props) override;
};

class binarize : public process {
public:
    void doProcessing(cv::Mat& img, cv::Mat& bg, patient props) override;
};

class normalize : public process {
public:
    void doProcessing(cv::Mat& img, cv::Mat& bg, patient props) override;
};

class absoluteDiff : public process {
public:
    void doProcessing(cv::Mat& img, cv::Mat& bg, patient props) override;
};

#endif  // CELLSORTER_PROCESS_H