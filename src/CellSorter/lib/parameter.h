#ifndef PARAMETER_H
#define PARAMETER_H

#include <assert.h>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <typeinfo>
#include <vector>

/** \brief ParameterBase is used to encapsulate parameters used in a Process.
 *  Through getOptions, we can deduce the type of the parameter (see below) - which will enable the
 * GUI to spawn the correct editor for the value.
 * To maintain a unified interface into Parameter subclasses, getters and setters all have a string
 * based variant..
 */

/** \brief Interfacing into a ParameterBase
 * calling ParameterBase::getOptions() will return a string vector
 *
 * stringstream a:
 *  a[0] = type (ie. int, float, ENUM type
 *  the values following the type is dependant on the type.
 *  for float/int:
 *      a[1..2] = min, max values
 *  for enums:
 *      A[1..n] = strings denoting the valid options for the enum
 *
 */

using namespace std;

class ParameterBase {
public:
    ParameterBase(string name) : m_name(name) {}
    void setModifiable(bool val) { m_isModifiable = val; }
    stringstream getOptions() const {
        stringstream s;
        for (const auto& i : m_options) {
            s << i << ' ';
        }
        return s;
    }

    // String based setters and getters, used through GUI
    virtual const string getValueStr() const = 0;
    virtual void setValueStr(string) = 0;

protected:
    // Function that is called to update m_options whenever a range is changed/new options are
    // added. Must be implemented by subclass
    virtual void updateOptions() = 0;

    std::vector<string> m_options;
    string m_name;
    bool m_isModifiable = true;
    bool m_isInitialized = false;  // Set when setting range or options. Assert is thrown if this is
                                   // not done, to force implementer to set range or options

private:
    // Private assignment operator. This ensures us that even though the Parameter is declared as a
    // public member in a Process, it cannot be assigned to
    void operator=(const ParameterBase&) {}
};

/** \anchor ValueParameter
 *  \brief Class for containing parameters which are numbers (floats, doubles, char, int).
 *  Contains range-information for the valid range of m_val
 */
template <typename T>
class ValueParameter : public ParameterBase {
public:
    ValueParameter(vector<ParameterBase*>& parent, std::string name, T initialValue = T());

    void setRange(T start, T stop);

    void setValue(T v) { m_val = v; }
    const T& getValue() const {
        assert(m_isInitialized);
        return m_val;
    }

    const string getValueStr() const override;
    void setValueStr(string) override;

    void updateOptions() override;

private:
    T m_val;
    pair<T, T> m_range;
};

template <typename T>
const string ValueParameter<T>::getValueStr() const {
    return std::to_string(getValue());
}

template <typename T>
void ValueParameter<T>::setValueStr(string value) {
    if (typeid(T).name() == typeid(int).name()) {
        m_val = stoi(value);
    } else if (typeid(T).name() == typeid(double).name()) {
        m_val = stod(value);
    } else if (typeid(T).name() == typeid(float).name()) {
        m_val = stof(value);
    } else {
        m_val = T();
    }
}

template <typename T>
ValueParameter<T>::ValueParameter(vector<ParameterBase*>& parentProcessContainer, std::string name,
                                  T v)
    : ParameterBase(name), m_val(v) {
    // Upon construction, append parameter to parent process' Parameter container
    parentProcessContainer.push_back(this);
}

template <typename T>
void ValueParameter<T>::setRange(T start, T stop) {
    m_isInitialized = true;
    m_range = std::pair<T, T>(start, stop);
    updateOptions();
}

template <typename T>
void ValueParameter<T>::updateOptions() {
    m_options.resize(3);
    m_options[0] = typeid(T).name();
    m_options[1] = to_string(m_range.first);
    m_options[2] = to_string(m_range.second);
}

/** \anchor EnumParameter
 *  \brief Used for options which are enumerated. Template argument type must be enumerateable - ie.
 *  an int must be constructible from T.
 */
template <typename T>
class EnumParameter : public ParameterBase {
public:
    EnumParameter(vector<ParameterBase*>& parent, std::string name, T initialValue = T());

    void setOptions(map<T, string>);

    void setValue(T v) { m_val = v; }
    const T& getValue() const {
        assert(m_isInitialized);
        return m_val;
    }

    const string getValueStr() const override;
    void setValueStr(string) override;

    void updateOptions() override;

private:
    T m_val;
    map<T, string> m_enumOptions;
};

template <typename T>
EnumParameter<T>::EnumParameter(vector<ParameterBase*>& parentProcessContainer, std::string name,
                                T v)
    : ParameterBase(name), m_val(v) {
    // Check if an int is constructable from T.
    static_assert(std::is_constructible<int, T>::value,
                  "An integer must be constructable from the input template argument type.");
    // Upon construction, append parameter to parent process' Parameter container
    parentProcessContainer.push_back(this);
}

template <typename T>
void EnumParameter<T>::setOptions(map<T, string> options) {
    m_isInitialized = true;
    m_enumOptions = options;
    updateOptions();
}

template <typename T>
void EnumParameter<T>::updateOptions() {
    m_options.resize(1 + m_enumOptions.size());
    m_options[0] = typeid(T).name();
    int i = 1;
    for (const auto& item : m_enumOptions) {
        m_options[i] = item.second;
        i++;
    }
}

template <typename T>
const string EnumParameter<T>::getValueStr() const {
    assert(m_isInitialized);
    for (const auto& v : m_enumOptions) {
        if (v.first == m_val) {
            return v.second;
        }
    }
    return string("ERROR");
}

template <typename T>
void EnumParameter<T>::setValueStr(string value) {
    for (const auto& v : m_enumOptions) {
        if (v.second == value) {
            m_val = v.first;
        }
    }
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
