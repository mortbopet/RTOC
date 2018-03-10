#ifndef PARAMETER_H
#define PARAMETER_H

#include <iostream>
#include <map>
#include <memory>
#include <typeinfo>
#include <vector>

/** ParameterBase is used to encapsulate parameters used in a Process
 *  Through ParameterType, we can deduce the type of the parameter, and do a static_cast to the
 * correct template type.
*/
using namespace std;

enum class ParameterType { INT, DOUBLE, CHAR, FLOAT, ENUM };

class ParameterBase {
public:
    ParameterBase(string name) : m_name(name) {}
    virtual const ParameterType getType() { return m_type; }
    void setName(string name) { m_name = name; }
    void setModifiable(bool val) { m_isModifiable = val; }

protected:
    string m_name;
    ParameterType m_type;  // each subclass sets its own type in its constructor
    bool m_isModifiable = true;

private:
    // Private assignment operator. This ensures us that even though the Parameter is declared as a
    // public member in a Process, it cannot be assigned to
    void operator=(const ParameterBase&) {}
};

/* ValueParameter
 * Class for containing parameters which are numbers (floats, doubles, char, int).
 * Contains range-information for the valid range of m_val
*/
template <typename T>
class ValueParameter : public ParameterBase {
public:
    ValueParameter(vector<ParameterBase*>& parent, std::string name, T initialValue = T());

    const pair<T, T>& getRange() const;
    void setRange(T start, T stop);

    void setValue(T v) { m_val = v; }
    const T& getValue() const { return m_val; }

private:
    T m_val;
    pair<T, T> m_range;
};

template <typename T>
ValueParameter<T>::ValueParameter(vector<ParameterBase*>& parentProcessContainer, std::string name,
                                  T v)
    : ParameterBase(name), m_val(v) {
    // Constrain template argument type to the following types
    static_assert(is_same<T, int>::value || is_same<T, char>::value || is_same<T, float>::value ||
                      is_same<T, double>::value,
                  "ValueParameter may only be int, double, char or float");
    if (is_same<T, int>::value) {
        m_type = ParameterType::INT;
    } else if (is_same<T, char>::value) {
        m_type = ParameterType::CHAR;
    } else if (is_same<T, float>::value) {
        m_type = ParameterType::FLOAT;
    } else if (is_same<T, double>::value) {
        m_type = ParameterType::DOUBLE;
    }

    // Upon construction, append parameter to parent process' Parameter container
    parentProcessContainer.push_back(this);
}

template <typename T>
void ValueParameter<T>::setRange(T start, T stop) {
    m_range = std::pair<T, T>(start, stop);
}

template <typename T>
const pair<T, T>& ValueParameter<T>::getRange() const {
    return m_range;
}

/* EnumParameter
 * Used for options which are enumerated. Template argument type must be enumerateable - ie. an int
 * must be constructible from T. This is because, when we find an ENUM parameter through gui, we do
 * not know the underlying enum, but instead cast see it as an EnumParameter<int>, and use
 * getOptions to decide on the allowable parameter options
 *
 * Contains Options information - a mapping between the valid options for the given parameter, as
 * well as a description for each option
*/
template <typename T>
class EnumParameter : public ParameterBase {
public:
    EnumParameter(vector<ParameterBase*>& parent, std::string name, T initialValue = T());

    const vector<string> getOptions() const;
    void setOptions(map<T, string>);

    void setValue(T v) { m_val = v; }
    const T& getValue() const { return m_val; }

private:
    T m_val;
    map<T, string> m_options;
};

template <typename T>
EnumParameter<T>::EnumParameter(vector<ParameterBase*>& parentProcessContainer, std::string name,
                                T v)
    : ParameterBase(name), m_val(v) {
    // Check if an int is constructable from the input type.
    static_assert(std::is_constructible<int, T>::value,
                  "An integer must be constructable from the input template argument type.");
    // Upon construction, append parameter to parent process' Parameter container
    m_type = ParameterType::ENUM;
    parentProcessContainer.push_back(this);
}

template <typename T>
void EnumParameter<T>::setOptions(map<T, string> options) {
    m_options = options;
}

template <typename T>
const vector<string> EnumParameter<T>::getOptions() const {
    vector<string> options;
    for (const auto& option : m_options) {
        options.push_back(option.second);
    }
    return options;
}

// ----------------------
// Declare some smart pointer types for value and enum parameters
template <typename T>
using UniqueValuePtr = std::unique_ptr<ValueParameter<T>>;
template <typename T>
using SharedValuePtr = std::shared_ptr<ValueParameter<T>>;

template <typename T>
using UniqueEnumPtr = std::unique_ptr<EnumParameter<T>>;
template <typename T>
using SharedEnumPtr = std::shared_ptr<EnumParameter<T>>;

#endif  // PARAMETER_H
