#ifndef PARAMETER_H
#define PARAMETER_H

#include <typeinfo>
#include <vector>

// ParameterBase is used to encapsulate parameters through Process type - we use typeid's to differentiate between the
// actual templatized type which is chosen in the individual processing functions
class ParameterBase {
public:
    virtual const char* getInfo() const;
};

// Parameter class
/**
 * Operators:
 *      Assignment operator is overloaded. This is done since Parameter objets are intended to live in the public scope
 * of a Process. Doing this, we can easily access parameters throughout code, but only assign them through setValue
 */
template <typename T>
class Parameter : public ParameterBase {
public:
    Parameter(std::vector<ParameterBase*> parent, T v = T());
    const std::vector<T>& getOptions() { return m_options; }
    const std::pair<T, T>& getRange() { return m_range; }
    void setValue(T v) { m_val = v; }
    const T& getValue() { return m_val; }
    const char* getInfo() const override { return typeid(T).name(); }

private:
    std::pair<T, T> m_range;   // used for ranged types (int, doubles)
    std::vector<T> m_options;  // used for enumerated types
    T m_val;

    bool m_isModifiable = true;

    void operator=(const T& rhs) {}
};

template <typename T>
Parameter<T>::Parameter(std::vector<ParameterBase*> parentProcessContainer, T v) {
    // Upon construction, append parameter to parent process' Parameter container
    parentProcessContainer.push_back(this);
    m_val = v;
}

#endif  // PARAMETER_H
