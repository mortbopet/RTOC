#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <assert.h>
#include <map>
#include <typeinfo>
#include <vector>

#include "opencv/cv.hpp"

namespace data {
// Enumeration of the various parameters that can be extracted from an image, using open CV
enum DataFlags {
    Area = 1 << 0,
    BoundingBox = 1 << 1,
    Centroid = 1 << 2,
    Circularity = 1 << 3,
    ConvexArea = 1 << 4,
    Eccentricity = 1 << 5,
    Frame = 1 << 6,
    GradientScore = 1 << 7,
    Inlet = 1 << 8,
    Outlet = 1 << 9,
    Label = 1 << 10,
    Major_axis = 1 << 11,
    Minor_axis = 1 << 12,
    Solidity = 1 << 13,
    Symmetry = 1 << 14,
    Perimeter = 1 << 15,
    PixelIdxList = 1 << 16
};

// Mapping between DataFlags and the corresponding datatype that the openCV operation returns
// This mapping is used by DataObject for memory allocation
static std::map<DataFlags, size_t> typeMap{{Area, sizeof(double)},
                                           {BoundingBox, sizeof(cv::Rect)},
                                           {Centroid, sizeof(cv::Point)},
                                           {Circularity, sizeof(double)},
                                           {ConvexArea, sizeof(int)},
                                           {Eccentricity, sizeof(double)},
                                           {Frame, sizeof(int)},
                                           {GradientScore, sizeof(double)},
                                           {Inlet, sizeof(int)},
                                           {Outlet, sizeof(int)},
                                           {Label, sizeof(int)},
                                           {Major_axis, sizeof(double)},
                                           {Minor_axis, sizeof(double)},
                                           {Solidity, sizeof(double)},
                                           {Symmetry, sizeof(double)},
                                           {Perimeter, sizeof(double)},
                                           {PixelIdxList, sizeof(std::vector<cv::Point>)}};


std::vector<data::DataFlags> extractFlags (int flags) {
    // Returns vector of data::DataFlags
    std::vector<data::DataFlags> returnFlags;
    int flagToGet = 1;
    for (int i = 0; i < 32; i++) { // assumes 32-bit
        if (flagToGet & flags) {
            returnFlags.push_back(data::DataFlags(flagToGet));
        }
        flagToGet = flagToGet << 1;
    }
    return returnFlags;
}

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
    if (!(m_dataFlags & dataFlag)) {
        throw std::runtime_error("requested dataFlag is not set for the object");
    };
    if (sizeof(T) != data::typeMap[dataFlag]) {
        // This should be done with typeId's
        throw std::runtime_error("Type for set-value is different from type of dataFlag");
    }
    size_t bytesToData = getBytesToData(dataFlag);

    // Dereference the memory as the requested type, and return
    return *static_cast<T*>(static_cast<void*>(m_memory + bytesToData));
}

template <typename T>
void DataObject::setValue(data::DataFlags dataFlag, T value) {
    if (!(m_dataFlags & dataFlag)) {
        throw std::runtime_error("requested dataFlag is not set for the object");
    };
    if (sizeof(T) != data::typeMap[dataFlag]) {
        // This should be done with typeId's
        throw std::runtime_error("Type for set-value is different from type of dataFlag");
    }

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
    void setClass(int classnumber);
    int getClass();
    int getDataFlags();
    int getNumberOfSetFlags();
    void addDataFlag(data::DataFlags flag);  // OR's a flag onto the data collection flags
    void clearDataFlags() { m_dataFlags = 0; }
    void clear() { m_data.clear(); }  // called whenever a m_dataFlags is changed

    DataObject* appendNew();
    DataObject* operator[](size_t idx) { return m_data[idx]; }

    std::vector<double> getDataVector();
    int getSize ();

private:
    void calculateObjectSize();

    std::vector<DataObject*> m_data;
    int m_dataFlags = 0;
    int m_class = 0; // Class of container (0 being unknown)
    bool m_locked = false;
    size_t m_objectSize;
};

#endif  // DATACONTAINER_H
