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
// Compile time string length generation
// https://stackoverflow.com/a/36390498/6714645
constexpr size_t ct_strlen(const char* s) noexcept {
    return *s ? 1 + ct_strlen(s + 1) : 0;
}
constexpr bool checkDisplayName(char const* name) {
    // Compile time verify that display name does not contain space characters (will break the
    // string stream parsing when loading parameters in gui)
    for (int i = 0; i < ct_strlen(name); i++) {
        if (name[i] == ' ') {
            return false;
        }
    }
    return true;
}

#define PARAMETER_CONTAINER m_parameters

// clang-format off
#define GEN_DISPLAYNAME(displayName) const_ ##displayName ## _
#define ASSERT_DISPLAYNAME(displayName)                        \
    static constexpr char GEN_DISPLAYNAME(displayName)[] = #displayName; \
    static_assert(checkDisplayName(GEN_DISPLAYNAME(displayName)),       \
                  "Display name may not contain ' ' characters");
// clang-format on

// Shorthand macros for creating process parameter member values
#define CREATE_ENUM_PARM(type, name, displayName) \
    ASSERT_DISPLAYNAME(displayName);              \
    EnumParameter<type> name =                    \
        EnumParameter<type>(PARAMETER_CONTAINER, GEN_DISPLAYNAME(displayName))

#define CREATE_ENUM_PARM_DEFAULT(type, name, displayName, default) \
    ASSERT_DISPLAYNAME(displayName);                               \
    EnumParameter<type> name =                                     \
        EnumParameter<type>(PARAMETER_CONTAINER, GEN_DISPLAYNAME(displayName), default)

#define CREATE_VALUE_PARM(type, name, displayName) \
    ASSERT_DISPLAYNAME(displayName);               \
    ValueParameter<type> name =                    \
        ValueParameter<type>(PARAMETER_CONTAINER, GEN_DISPLAYNAME(displayName))

#define CREATE_VALUE_PARM_DEFAULT(type, name, displayName, default) \
    ASSERT_DISPLAYNAME(displayName);                                \
    ValueParameter<type> name =                                     \
        ValueParameter<type>(PARAMETER_CONTAINER, GEN_DISPLAYNAME(displayName) s, default)
}  // namespace

class ProcessBase {
    friend class ProcessNameGenerator;
    friend class ProcessInterface;

public:
    virtual const std::string& getTypeName() = 0;
    ProcessBase();
    virtual void

    doProcessing(cv::Mat& img, cv::Mat& bg,
                 const Experiment& props) const = 0;  // General function for doing processing.
    const std::vector<ParameterBase*>& getParameters() { return PARAMETER_CONTAINER; }

protected:
    static std::vector<std::string>& get_processes();
    std::vector<ParameterBase*> PARAMETER_CONTAINER;
};

// ---------------------------------------------------
/* Process registration and getters
 * The following classes aid in "before runtime" generation of a list of subclasses of Process,
 * which can be accessed by GUI
 * https://stackoverflow.com/questions/34858341/c-compile-time-list-of-subclasses-of-a-class
 *
 */
class ProcessNameGenerator {
public:
    ProcessNameGenerator(const std::string& name) {
        // Notify when the static member is created
        add_process(name);
        m_typeName = name;
    }

    std::string m_typeName;

private:
    void add_process(const std::string& name);
};

template <typename T>
class Process : public ProcessBase {
public:
    const std::string& getTypeName() override { return m.m_typeName; }

protected:
    // Static member in a template class
    Process() { (void)m; }
    static ProcessNameGenerator m;
};

template <typename T>
ProcessNameGenerator Process<T>::m = ProcessNameGenerator(typeid(T).name());
// ---------------------------------------------------

class Morph : public Process<Morph> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    Morph();
    static std::string getName() { return "Morph"; }

    CREATE_ENUM_PARM(cv::MorphTypes, m_morphType, Morphology_type);
    CREATE_VALUE_PARM(int, m_morphValueX, Structural_element_X_axis);
    CREATE_VALUE_PARM(int, m_morphValueY, Structural_element_Y_axis);
};

class Binarize : public Process<Binarize> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    Binarize();
    static std::string getName() { return "Binarize"; }

    CREATE_VALUE_PARM(double, m_edgeThreshold, Edge_threshold);
    CREATE_VALUE_PARM(double, m_maxVal, Maximum_binary_value);
};

class Normalize : public Process<Normalize> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    Normalize();
    static std::string getName() { return "Normalize"; }

    CREATE_VALUE_PARM(int, m_normalizeStrength, Normalize_strength);
};

class SubtractBG : public Process<SubtractBG> {
public:
    void doProcessing(cv::Mat& img, cv::Mat& bg, const Experiment& props) const override;
    SubtractBG();
    static std::string getName() { return "Subtract background"; }

    CREATE_VALUE_PARM(double, m_edgeThreshold, Edge_threshold);
};

class Canny : public Process<Canny> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    static std::string getName() { return "Canny"; }

    CREATE_VALUE_PARM(double, m_lowThreshold, Low_threshold);    // first threshold
    CREATE_VALUE_PARM(double, m_highThreshold, High_threshold);  // second threshold
};

class ClearBorder : public Process<ClearBorder> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    ClearBorder();
    static std::string getName() { return "Clear border"; }

    CREATE_VALUE_PARM(int, m_borderWidth, Border_width);
};

class FloodFillProcess : public Process<FloodFillProcess> {
public:
    static std::string getName() { return "Flood fill"; }
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
};

class PropFilter : public Process<PropFilter> {
public:
    void doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const override;
    PropFilter();
    static std::string getName() { return "Property filter"; }

    CREATE_ENUM_PARM(matlab::regionPropTypes, m_regionPropsTypes, Regionprop_types);

    CREATE_VALUE_PARM(double, m_lowerLimit, Lower_Limit);
    CREATE_VALUE_PARM(double, m_upperLimit, Upper_Limit);
};

typedef std::vector<std::unique_ptr<ProcessBase>>* processContainerPtr;

#endif  // CELLSORTER_PROCESS_H
