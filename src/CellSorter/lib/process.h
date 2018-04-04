#ifndef CELLSORTER_PROCESS_H
#define CELLSORTER_PROCESS_H

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <opencv/cv.hpp>
#include <string>

#include "datacontainer.h"

#include "experiment.h"
#include "matlab_ext.h"
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
}  // namespace

class ProcessBase {
public:
    ProcessBase();
    static std::vector<std::string>& get_processes();
    virtual void
    doProcessing(cv::Mat& img, cv::Mat& bg,
                 const Experiment& props) const = 0;  // General function for doing processing.

protected:
    std::vector<ParameterBase*> PARAMETER_CONTAINER;
    std::string m_name;
};

// ---------------------------------------------------
/* Process registration and getters
 * The following classes aid in "before runtime" generation of a list of subclasses of Process,
 * which can be accessed by GUI
 * https://stackoverflow.com/questions/34858341/c-compile-time-list-of-subclasses-of-a-class
 *
 */
namespace {
void add_process(const std::string& name);
}
class ProcessNameGenerator {
public:
    ProcessNameGenerator(const std::string& name) {
        // Notify when the static member is created
        add_process(name);
    }
};

template <typename T>
class Process : public ProcessBase {
protected:
    // Static member in a template class
    Process() { (void)m; }
    static ProcessNameGenerator m;
};

template <typename T>
ProcessNameGenerator Process<T>::m = ProcessNameGenerator(typeid(Process<T>).name());
// ---------------------------------------------------

class Morph : public Process<Morph> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    Morph();

    CREATE_ENUM_PARM(cv::MorphTypes, m_morphType, "Morphology type");
    CREATE_VALUE_PARM(int, m_morphValueX, "Structural element X-axis");
    CREATE_VALUE_PARM(int, m_morphValueY, "Structural element Y-axis");
};

class Binarize : public Process<Binarize> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    Binarize();

    CREATE_VALUE_PARM(double, m_edgeThreshold, "Edge threshold");
    CREATE_VALUE_PARM(double, m_maxVal, "Maximum binary value");
};

class Normalize : public Process<Normalize> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    Normalize();

    CREATE_VALUE_PARM(int, m_normalizeStrength, "Normalize strength");
};

class SubtractBG : public Process<SubtractBG> {
public:
    void doProcessing(cv::Mat& img, cv::Mat& bg, const Experiment& props) const override;
    SubtractBG();

    CREATE_VALUE_PARM(double, m_edgeThreshold, "Edge threshold");
};

class Canny : public Process<Canny> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;

    CREATE_VALUE_PARM(double, m_lowThreshold, "Low threshold");    // first threshold
    CREATE_VALUE_PARM(double, m_highThreshold, "High threshold");  // second threshold
};

class ClearBorder : public Process<ClearBorder> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    ClearBorder();

    CREATE_VALUE_PARM(int, m_borderWidth, "Border width");
};

class FloodFill : public Process<FloodFill> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
};

class PropFilter : public Process<PropFilter> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    PropFilter();

    CREATE_ENUM_PARM(matlab::regionPropTypes, m_regionPropsTypes, "Regionprop types");

    CREATE_VALUE_PARM(double, m_lowerLimit, "Lower Limit");
    CREATE_VALUE_PARM(double, m_upperLimit, "Upper Limit");
};

#endif  // CELLSORTER_PROCESS_H
