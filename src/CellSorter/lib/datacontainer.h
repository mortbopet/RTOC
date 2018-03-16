#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <assert.h>
#include <map>
#include <typeinfo>
#include <vector>

namespace data {
// Enumeration of the various parameters that can be extracted from an image, using open CV
enum DataFlags {
    Area = 1 << 0,
    Circularity = 1 << 1,
    Eccentricity = 1 << 2,
    Major_axis = 1 << 3,
    Minor_axis = 1 << 4
};

// Mapping between DataFlags and the corresponding datatype that the openCV operation returns
// This mapping is used by DataObject for memory allocation
static std::map<DataFlags, size_t> typeMap{{Area, sizeof(int)},
                                           {Circularity, sizeof(double)},
                                           {Major_axis, sizeof(float)}};
}  // namespace data

/**
 * @brief The DataObject class
 * @details allocates and manages memory that has been requested by the input dataFlags parameter
 */
class DataObject {
public:
    DataObject(long dataFlags, size_t size);
    ~DataObject();

    template <typename T>
    const T& getValue(data::DataFlags dataFlag);

    template <typename T>
    void setValue(data::DataFlags dataFlag, T value);

private:
    size_t getBytesToData(data::DataFlags flag);

    char* m_memory = nullptr;
    int m_dataFlags;
    size_t m_size;  // Total allocated memory space
};

template <typename T>
const T& DataObject::getValue(data::DataFlags dataFlag) {
    size_t bytesToData = getBytesToData(dataFlag);

    // Dereference the memory as the requested type, and return
    return *static_cast<T*>(static_cast<void*>(m_memory + bytesToData));
}

template <typename T>
void DataObject::setValue(data::DataFlags dataFlag, T value) {
    size_t bytesToData = getBytesToData(dataFlag);
    *(static_cast<T*>(static_cast<void*>(m_memory + bytesToData))) = value;
}

/**
 * @brief The DataContainer class
 * @details Class for managing and storing the data collection, collected in an experiment
 *
 * @anchor Dataflags
 * @details Flags for defining which data values that will be extracted for a frame.
 */

class DataContainer {
public:
    DataContainer();
    DataContainer(long flags);
    ~DataContainer();

    void setDataFlags(data::DataFlags flag);  // sets ALL data flags
    void setDataFlags(long flag);
    void addDataFlag(data::DataFlags flag);  // OR's a flag onto the data collection flags
    void clearDataFlags() { m_dataFlags = 0; }
    void clear() { m_data.clear(); }  // called whenever a m_dataFlags is changed

    DataObject& appendNew();

    template <typename T>
    const T& getValue(int i, data::DataFlags dataFlag);

    template <typename T>
    void setValue(int i, data::DataFlags dataFlag, T value);

private:
    void calculateObjectSize();

    std::vector<DataObject*> m_data;
    int m_dataFlags = 0;

    bool m_locked = false;
    size_t m_objectSize;
};

template <typename T>
const T& DataContainer::getValue(int i, data::DataFlags dataFlag) {
    assert(m_dataFlags & dataFlag);
    // Queries the i'th object of m_data to return the requested value
    return m_data[i]->getValue<T>(dataFlag);
}

template <typename T>
void DataContainer::setValue(int i, data::DataFlags dataFlag, T value) {
    assert(m_dataFlags & dataFlag);
    // Queries the i'th object of m_data to return the requested value
    return m_data[i]->setValue<T>(dataFlag, value);
}

#endif  // DATACONTAINER_H
