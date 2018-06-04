#ifndef DATACONTAINER_H
#define DATACONTAINER_H

#include <cassert>
#include <iterator>
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
    PixelIdxList = 1 << 16,
    OutputValue = 1 << 17
};

// Mapping between DataFlags and the corresponding datatype that the openCV operation returns
// This mapping is used by DataObject for memory allocation
static std::map<DataFlags, std::pair<int, size_t>> typeMap{{Area, std::make_pair(1, sizeof(double))},
                                           {BoundingBox, std::make_pair(4, sizeof(cv::Rect))},
                                           {Centroid, std::make_pair(2, sizeof(cv::Point))},
                                           {Circularity, std::make_pair(1, sizeof(double))},
                                           {ConvexArea, std::make_pair(1, sizeof(double))},
                                           {Eccentricity, std::make_pair(1, sizeof(double))},
                                           {Frame, std::make_pair(1, sizeof(int))},
                                           {GradientScore, std::make_pair(1, sizeof(double))},
                                           {Inlet, std::make_pair(1, sizeof(int))},
                                           {Outlet, std::make_pair(1, sizeof(int))},
                                           {Label, std::make_pair(1, sizeof(int))},
                                           {Major_axis, std::make_pair(1, sizeof(double))},
                                           {Minor_axis, std::make_pair(1, sizeof(double))},
                                           {Solidity, std::make_pair(1, sizeof(double))},
                                           {Symmetry, std::make_pair(1, sizeof(double))},
                                           {Perimeter, std::make_pair(1, sizeof(double))},
                                           {PixelIdxList, std::make_pair(1, sizeof(std::vector<cv::Point>*))},
                                           {OutputValue, std::make_pair(1, sizeof(double))}};

enum Export {
    Always = 1 << 0,
    Gui = 1 << 1,
    Never = 1 << 2
};

// Data types which can be extracted through GUI - gui uses this map to generate available data
// points for data extraction. The boolean value is set True, when the name should be displayed in GUI.
static std::map<std::pair<bool, DataFlags>, std::string> guiMap{{std::make_pair(Export::Gui, Area), "Area"},
                                                                {std::make_pair(Export::Always, BoundingBox), "BoundingBox"},
                                                                {std::make_pair(Export::Always, Centroid), "Centroid"},
                                                                {std::make_pair(Export::Gui, Circularity), "Circularity"},
                                                                {std::make_pair(Export::Gui, ConvexArea), "Convex area"},
                                                                {std::make_pair(Export::Gui, Eccentricity), "Eccentricity"},
                                                                {std::make_pair(Export::Always, Frame), "Frame"},
                                                                {std::make_pair(Export::Gui, GradientScore), "Gradient score"},
                                                                {std::make_pair(Export::Always, Inlet), "Inlet"},
                                                                {std::make_pair(Export::Always, Outlet), "Outlet"},
                                                                {std::make_pair(Export::Always, Label), "Label"},
                                                                {std::make_pair(Export::Gui, Major_axis), "Major axis"},
                                                                {std::make_pair(Export::Gui, Minor_axis), "Minor axis"},
                                                                {std::make_pair(Export::Gui, Solidity), "Solidity"},
                                                                {std::make_pair(Export::Gui, Symmetry), "Symmetry"},
                                                                {std::make_pair(Export::Gui, Perimeter), "Perimeter"},
                                                                {std::make_pair(Export::Always, OutputValue), "Output value"}};

}  // namespace data

/**
 * @brief The DataObject class
 * @details Allocates and manages memory that has been requested by the input dataFlags parameter
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
    if (sizeof(T) != std::get<1>(data::typeMap[dataFlag])) {
        // This should be done with typeId's
        throw std::runtime_error("Type for set-value is different from type of dataFlag");
    }
    size_t bytesToData = getBytesToData(dataFlag);

    // Dereference the memory as the requested type, and return
    return *(static_cast<T*>(static_cast<void*>(m_memory + bytesToData)));
}

template <typename T>
void DataObject::setValue(data::DataFlags dataFlag, T value) {
    if (!(m_dataFlags & dataFlag)) {
        throw std::runtime_error("requested dataFlag is not set for the object");
    };
    if (sizeof(T) != std::get<1>(data::typeMap[dataFlag])) {
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
    void addDataFlag(data::DataFlags flag);  // OR's a flag onto the data collection flags

    std::vector<double> extractObjectInDoubles(int objIndex);
    std::vector<std::string> extractAttributeName();
    std::vector<int> extractAttributeLengths();

    int numberOfFlags();
    void clearDataFlags() { m_dataFlags = 0; }
    void clear() { m_data.clear(); }  // called whenever a m_dataFlags is changed
    size_t size() const { return m_data.size(); }

    DataObject* appendNew();
    DataObject* operator[](size_t idx) { return m_data[idx]; }

    DataObject* front() const { return m_data.front(); }
    DataObject* back() const { return m_data.back(); }

    std::vector<DataObject*>::iterator begin() { return m_data.begin(); }
    std::vector<DataObject*>::iterator end() { return m_data.end(); }
    std::vector<DataObject*>::const_iterator begin() const { return m_data.begin(); }
    std::vector<DataObject*>::const_iterator end() const { return m_data.end(); }
    std::vector<DataObject*>::const_iterator cbegin() const { return m_data.cbegin(); }
    std::vector<DataObject*>::const_iterator cend() const { return m_data.cend(); }

private:
    std::vector<DataObject*> m_data;
    int m_dataFlags = 0;

    bool m_locked = false;
    size_t m_objectSize;

protected:
    void calculateObjectSize();
};

#endif  // DATACONTAINER_H
