#include "datacontainer.h"

// --------------------- DataObject ---------------------------
DataObject::DataObject(unsigned long dataFlags, size_t size) : m_dataFlags(dataFlags), m_size(size) {
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
        if (item.first & flag) {
            break;
        }
        if (item.first & m_dataFlags) {
            bytesToData += std::get<1>(item.second);
        }
    }
    return bytesToData;
}

// --------------------- DataContainer ------------------------
DataContainer::DataContainer() {}
DataContainer::DataContainer(unsigned long flags) : m_dataFlags(flags) {
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
            m_objectSize += std::get<1>(item.second);
        }
    }
}

void DataContainer::setDataFlags(data::DataFlags flag) {
    clear();
    m_dataFlags = flag;
    calculateObjectSize();
}

void DataContainer::setDataFlags(unsigned long flag) {
    clear();
    m_dataFlags = flag;
    calculateObjectSize();
}

void DataContainer::addDataFlag(data::DataFlags flag) {
    clear();
    m_dataFlags |= flag;
    calculateObjectSize();
}

std::vector<double> DataContainer::extractObjectInDoubles(int objIndex) {
    std::vector<double> returnVector;
    if (data::Area & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::Area));
    }
    if (data::BoundingBox & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<cv::Rect>(data::BoundingBox).height);
        returnVector.push_back(m_data[objIndex]->getValue<cv::Rect>(data::BoundingBox).width);
        returnVector.push_back(m_data[objIndex]->getValue<cv::Rect>(data::BoundingBox).x);
        returnVector.push_back(m_data[objIndex]->getValue<cv::Rect>(data::BoundingBox).y);
    }
    if (data::Centroid & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<cv::Point>(data::Centroid).x);
        returnVector.push_back(m_data[objIndex]->getValue<cv::Point>(data::Centroid).y);
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
    if (data::Frame & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<int>(data::Frame));
    }
    if (data::GradientScore & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::GradientScore));
    }
    if (data::Inlet & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<cv::Point>(data::Inlet).x);
        returnVector.push_back(m_data[objIndex]->getValue<cv::Point>(data::Inlet).y);
    }
    if (data::Outlet & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<cv::Point>(data::Outlet).x);
        returnVector.push_back(m_data[objIndex]->getValue<cv::Point>(data::Outlet).y);
    }
    if (data::Label & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<int>(data::Label));
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
    if (data::OutputValue & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::OutputValue));
    }
    if (data::RelativeXpos & m_dataFlags) {
        returnVector.push_back(m_data[objIndex]->getValue<double>(data::RelativeXpos));
    }
    return returnVector;
}

std::vector<std::string> DataContainer::extractAttributeName() {
    std::vector<std::string> returnVector;
    for (const auto& item : data::guiMap) {
        if (std::get<1>(item.first) != data::PixelIdxList) {
            returnVector.push_back(item.second);
        }
    }
    return returnVector;
}

std::vector<int> DataContainer::extractAttributeLengths() {
    std::vector<int> returnVector;
    for (const auto& item : data::typeMap) {
        returnVector.push_back(std::get<0>(item.second));
    }
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

unsigned long DataContainer::getDataFlags() {
    return m_dataFlags;
}
