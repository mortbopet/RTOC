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

std::vector<double> DataContainer::extractObject(int objIndex) {
    std::vector<double> returnVector;
    if (data::Area & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Area));
    }
    if (data::Circularity & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Circularity));
    }
    if (data::ConvexArea & m_dataFlags) {
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
    return returnVector;
}

std::vector<double> DataContainer::extractContainer() {
    std::vector<double> returnVector;
    long sizeOfContainer = size();
    for (int i = 0; i < sizeOfContainer; i++) {
        std::vector<double> returnVectorTemp = extractObject(i);
        returnVector.insert(returnVector.end(), returnVectorTemp.begin(), returnVectorTemp.end());
    }

    return returnVector;
}

std::vector<std::string> DataContainer::extractAttributeNames() {
    std::vector<std::string> returnVector;
    // Goes through all DataObjects
    for (int i = 0; i < size(); i++) {
        // Goes through all attributes for that given object (Need to exclude non-double values)
        for (int j = 0; j < numberOfFlags(); j++) {
            returnVector.push_back
                ("C" + std::to_string(i+1) + ": " +
                 "Attribute " + std::to_string(j+1));  // REWRITE LAST PART, so specific name is given
        }
    }
    return returnVector;
}

int DataContainer::numberOfFlags() {
    int number = 0;
    for (int i = 0; i < 32; i++) {
        if (0b1 & (m_dataFlags >> i)) {
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
