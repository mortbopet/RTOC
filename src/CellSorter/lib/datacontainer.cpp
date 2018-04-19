#include "datacontainer.h"

// --------------------- DataObject ---------------------------
DataObject::DataObject(long dataFlags, size_t size) : m_dataFlags(dataFlags), m_size(size) {
    // Allocate the requested amount of memory
    m_memory = static_cast<char*>(malloc(m_size));
}

DataObject::~DataObject() {
    if (m_dataFlags & data::PixelIdxList) {
        std::vector<cv::Point>* vector;
        vector = static_cast<std::vector<cv::Point>*>(
            static_cast<void*>(m_memory + getBytesToData(data::PixelIdxList)));
    }
    free(m_memory);
}

size_t DataObject::getBytesToData(data::DataFlags flag) {
    size_t bytesToData = 0;
    // Iterate through the typeMap and match against flag. Until flag is found, if other flags in
    // the map is found in m_dataFlags, increment bytesToData with the corresponding data size.
    for (const auto& item : data::typeMap) {
        if (!(item.first & flag) && (item.first & m_dataFlags)) {
            bytesToData += item.second;
        } else {
            break;
        }
    }
    return bytesToData;
}

// --------------------- DataContainer ------------------------
DataContainer::DataContainer() {}
DataContainer::DataContainer(long flags) : m_dataFlags(flags) {
    calculateObjectSize();
}

void DataContainer::calculateObjectSize() {
    // Called each time DataContainer's m_dataFlags are edited. m_objectSize is parsed to the
    // DataObject constructor, such that it knows the amt. of memory to allocate

    // Iterate through the typeMap and match against dataFlags. If match is found, increment m_size
    // with the corresponding data type size
    m_objectSize = 0;
    for (const auto& item : data::typeMap) {
        if (item.first & m_dataFlags) {
            m_objectSize += item.second;
        }
    }
}

void DataContainer::setDataFlags(data::DataFlags flag) {
    clear();
    m_dataFlags = flag;
    calculateObjectSize();
}

void DataContainer::setDataFlags(long flag) {
    clear();
    m_dataFlags = flag;
    calculateObjectSize();
}

void DataContainer::addDataFlag(data::DataFlags flag) {
    clear();
    m_dataFlags |= flag;
    calculateObjectSize();
}

std::vector<double> DataContainer::extractObject(int objIndex, int lastObject) {
    std::vector<double> returnVector;
    if (data::Area & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Area));
    }
    if (data::Circularity & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Circularity));
    }
    if (data::Area & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::ConvexArea));
    }
    if (data::Eccentricity & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Eccentricity));
    }
    if (data::GradientScore & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::GradientScore));
    }
    if (data::Major_axis & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Major_axis));
    }
    if (data::Minor_axis & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Minor_axis));
    }
    if (data::Solidity & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Solidity));
    }
    if (data::Symmetry & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Symmetry));
    }
    if (data::Perimeter & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Perimeter));
    }
    if ((data::OutputValue & m_dataFlags) &&
        ((lastObject - 1) == objIndex)) {  // Checks if last object is hit, returns output value
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::OutputValue));
    }
    return returnVector;
}

std::vector<double> DataContainer::extractContainer() {
    std::vector<double> returnVector;
    long sizeOfContainer = size();
    for (int i = 0; i < sizeOfContainer; i++) {
        std::vector<double> returnVectorTemp = extractObject(i, size());
        returnVector.insert(returnVector.end(), returnVectorTemp.begin(), returnVectorTemp.end());
    }
    return returnVector;
}

std::vector<std::string> DataContainer::extractAttributeNames() {
    std::vector<std::string> returnVector;
    for (int i = 0; i < size(); i++) {
        for (const auto& item : data::guiMap) {
            if ((item.first & m_dataFlags) &&
                (item.first != data::Frame && item.first != data::Inlet &&
                 item.first != data::Outlet && item.first != data::Label &&
                 item.first != data::OutputValue)) {
                returnVector.emplace_back("C" + std::to_string(i + 1) + ": " + item.second);
            }
        }
    }
    returnVector.emplace_back("Output");
    return returnVector;
}

int DataContainer::numberOfFlags() {
    int number = 0;
    for (int i = 0; i < 32; i++) {
        if ((0b1 & (m_dataFlags >> i)) && (0b1 << i) != data::BoundingBox &&
            (0b1 << i) != data::Centroid && (0b1 << i) != data::Frame &&
            (0b1 << i) != data::Inlet && (0b1 << i) != data::Outlet && (0b1 << i) != data::Label &&
            (0b1 << i) != data::PixelIdxList) {
            number++;
        }
    }
    return number;
}

DataObject* DataContainer::appendNew() {
    m_data.push_back(new DataObject(m_dataFlags, m_objectSize));
    return *m_data.rbegin();
}

DataContainer::~DataContainer() {
    // Since we allocated the DataObject's on the heap, we need to delete then, before deleting the
    // containing vector
    for (auto object : m_data) {
        delete object;
    }
}
