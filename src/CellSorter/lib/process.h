#ifndef CELLSORTER_PROCESS_H
#define CELLSORTER_PROCESS_H

#include <fstream>
#include <iostream>
#include <opencv/cv.hpp>
#include <string>

#include "experiment.h"
#include "parameter.h"

namespace {
#define PARAMETER_CONTAINER m_parameters

// Shorthand macros for creating process parameter member values
#define CREATE_ENUM_PARM(type, name, displayName) \
    EnumParameter<type> name = EnumParameter<type>(PARAMETER_CONTAINER, displayName)
#define CREATE_ENUM_PARM_DEFAULT(type, name, displayName, default) \
    EnumParameter<type> name = EnumParameter<type>(PARAMETER_CONTAINER, displayName, default)

#define CREATE_VALUE_PARM(type, name, displayName) \
    ValueParameter<type> name = ValueParameter<type>(PARAMETER_CONTAINER, displayName)
#define CREATE_VALUE_PARM_DEFAULT(type, name, displayName, default) \
    ValueParameter<type> name = ValueParameter<type>(PARAMETER_CONTAINER, displayName, default)
}

class Process {
public:
    Process();  // Constructor

    virtual void
    doProcessing(cv::Mat& img, cv::Mat& bg,
                 const Experiment& props) const = 0;  // General function for doing processing.

protected:
    std::vector<ParameterBase*> PARAMETER_CONTAINER;
};

class Morph : public Process {
public:
    void doProcessing(cv::Mat& img, cv::Mat&,
                      const Experiment& props) const override;  // CHANGE TO CONST OVERRIDE
    Morph();

    CREATE_ENUM_PARM(cv::MorphTypes, m_morphType, "Morphology type");
    CREATE_VALUE_PARM(int, m_morphValueX, "Structural element X-axis");
    CREATE_VALUE_PARM(int, m_morphValueY, "Structural element Y-axis");
};

class Binarize : public Process {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    Binarize();

    CREATE_VALUE_PARM(double, m_edgeThreshold, "Edge threshold");
    CREATE_VALUE_PARM(double, m_maxVal, "Maximum binary value");
};

class Normalize : public Process {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    Normalize();

    CREATE_VALUE_PARM(int, m_normalizeStrength, "Normalize strength");
};

class AbsoluteDiff : public Process {
public:
    void doProcessing(cv::Mat& img, cv::Mat& bg, const Experiment& props) const override;
};

#endif  // CELLSORTER_PROCESS_H
