#ifndef CELLSORTER_PROCESS_H
#define CELLSORTER_PROCESS_H

#include <fstream>
#include <iostream>
#include <opencv/cv.hpp>
#include <string>

#include "parameter.h"
#include "patient.h"

namespace {
#define PARAMETER_CONTAINER m_parameters

// Shorthand macros for creating process parameter member values
#define CREATE_ENUM_PARM(type, name, displayName) \
    EnumParameter<type> name = EnumParameter<type>(PARAMETER_CONTAINER, displayName)

#define CREATE_VALUE_PARM(type, name, displayName) \
    ValueParameter<type> name = ValueParameter<type>(PARAMETER_CONTAINER, displayName)
}

class process {
public:
    process();  // Constructor

    virtual void doProcessing(cv::Mat& img, cv::Mat& bg,
                              patient props) = 0;  // General function for doing processing.

protected:
    std::vector<ParameterBase*> PARAMETER_CONTAINER;
};

class erosion : public process {
public:
    erosion() {}
    void doProcessing(cv::Mat& img, cv::Mat& bg, patient props) override;
};

class dilation : public process {
public:
    void doProcessing(cv::Mat& img, cv::Mat& bg, patient props) override;

    CREATE_ENUM_PARM(cv::MorphTypes, m_morphType, "Morphology type");
};

class binarize : public process {
public:
    void doProcessing(cv::Mat& img, cv::Mat& bg, patient props) override;

    CREATE_VALUE_PARM(double, m_maxVal, "Edge threshold");
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
