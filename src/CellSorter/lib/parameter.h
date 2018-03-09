#ifndef PARAMETER_H
#define PARAMETER_H

#include <iostream>
#include <memory>
#include <typeinfo>
#include <vector>

/** ParameterBase is used to encapsulate parameters through Process type - we use typeid's to
 * differentiate between the
 *  actual templatized type which is chosen in the individual processing functions.
 * This is done because it is unallowed to have a member function that is declared both virtual and
 * templated.
*/
using namespace std;

class ParameterBase {
public:
    enum class Type { ENUMERABLE, VALUE };

    // Through getInfo() we can derive the type of Parameter<T> and know which setters/getters to
    // use
    virtual const char* getInfo() const = 0;

    virtual const pair<int, int>* getIntRange() const { return nullptr; }
    virtual const pair<double, double>* getDblRange() const { return nullptr; }
    virtual const vector<string>* getOptions() const { return nullptr; }
    virtual void setRange(int, int) {}
    virtual void setRange(double, double) {}
    virtual void setOptions(vector<std::pair<int, string>>) {}

    void printInfo() const {
        const string type = string(this->getInfo());
        if (type == "int") {
            cout << this->getIntRange() << endl;
        } else if (type == "double") {
            cout << this->getDblRange() << endl;
        } else {
            // enumerable type;
            cout << this->getOptions() << endl;
        }
    }
};

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
    Parameter(vector<ParameterBase*>& parent, std::string name, T initialValue = T());

    const pair<int, int>* getIntRange() const override { return nullptr; }
    const pair<double, double>* getDblRange() const override { return nullptr; }
    const vector<string>* getOptions() const override { return nullptr; }

    void setRange(int start, int stop) override;
    void setRange(double start, double stop) override;
    void setOptions(vector<std::pair<int, string>>) override;

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
Parameter<T>::Parameter(vector<ParameterBase*>& parentProcessContainer, std::string name, T v)
    : m_name(name), m_val(v) {
    // Upon construction, append parameter to parent process' Parameter container
    parentProcessContainer.push_back(this);
}

template <typename T>
void Parameter<T>::setRange(int start, int stop) {
    m_isRanged = true;
    m_range = std::pair<T, T>(T(start), T(stop));
}
template <typename T>
void Parameter<T>::setRange(double start, double stop) {
    m_isRanged = true;
    // m_range = std::pair<T, T>(T(start), T(stop));
}

template <typename T>
void Parameter<T>::setOptions(vector<std::pair<int, string>> options) {
    m_isRanged = false;
    // m_options = options;
}

// Declare smart pointer types for some common operators
using IntParmPtr = std::unique_ptr<Parameter<int>>;
using DoubleParmPtr = std::unique_ptr<Parameter<double>>;

#endif  // PARAMETER_H
