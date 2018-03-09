#ifndef PARAMETER_H
#define PARAMETER_H

#include <memory>
#include <typeinfo>
#include <vector>

/** ParameterBase is used to encapsulate parameters through Process type - we use typeid's to
 * differentiate between the
 *  actual templatized type which is chosen in the individual processing functions.
 * This is done because it is unallowed to have a member function that is declared both virtual and
 * templated.
*/
class ParameterBase {
public:
    // Through getInfo() we can derive the type of Parameter<T> and
    // static_cast<Parameter<T>>(ParameterBase)
    virtual const char* getInfo() const = 0;
};

using namespace std;

/**
 * Operators:
 *      Assignment operator is overloaded. This is done since Parameter objets are intended to live
 * in the public scope
 *      of a Process. Doing this, we can easily access parameters throughout code, but only assign
 * them through setValue
 */
template <typename T>
class Parameter : public ParameterBase {
public:
    Parameter(vector<ParameterBase*> parent, std::string name, T initialValue = T());

    const vector<T>& getOptions() { return m_options; }
    void setOptions(vector<pair<T, string>> options);

    const pair<T, T>& getRange() { return m_range; }
    void setRange(T low, T high);

    void setName(string name) { m_name = name; }

    void setValue(T v) { m_val = v; }
    const T& getValue() { return m_val; }

    const char* getInfo() const override { return typeid(T).name(); }

private:
    string m_name;
    T m_val;

    // used for ranged types (int, doubles). Defines valid range to set m_value in
    pair<T, T> m_range;

    // used for enumerated types. Vector of enum value and string value for the given parameter
    vector<pair<T, string>> m_options;

    bool m_isModifiable = true;
    bool m_isRanged = true;  // Parameters defaults to being identified as "ranged" types (integers,
                             // doubles etc.). Changed when calling setOptions

    // Private assignment operator. This ensures us that even though the Parameter is declared as a
    // public member in a Process, it cannot be assigned to
    void operator=(const T& rhs) {}
};

template <typename T>
Parameter<T>::Parameter(vector<ParameterBase*> parentProcessContainer, std::string name, T v)
    : m_name(name), m_val(v) {
    // Upon construction, append parameter to parent process' Parameter container
    parentProcessContainer.push_back(this);
    m_val = v;
}

template <typename T>
void Parameter<T>::setOptions(vector<pair<T, string>> options) {
    m_isRanged = false;
    m_options = options;
}

template <typename T>
void Parameter<T>::setRange(T low, T high) {
    m_isRanged = true;
    m_range = std::pair<T, T>(low, high);
}

// Declare smart pointer types for some common operators
using IntParmPtr = std::unique_ptr<Parameter<int>>;
using DoubleParmPtr = std::unique_ptr<Parameter<double>>;

#endif  // PARAMETER_H
