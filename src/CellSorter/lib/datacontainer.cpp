#include "datacontainer.h"

#include <assert.h>
#include <cstring>

// --------------------- DataObject ---------------------------
DataObject::DataObject(long dataFlags, size_t size) : m_dataFlags(dataFlags), m_size(size) {
    // Allocate the requested amount of memory
    m_memory = static_cast<char*>(malloc(m_size));
}

DataObject::~DataObject() {
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

std::vector<double> DataContainer::getDataVector() {
    // Returns a 1D-array of datacontainer, i.e. all parameters of all dataobjects.

    // Initialze vector with length of parameters * entries(frames)
    int numberOfEntries = getSize();
    int numberOfParameters = getNumberOfSetFlags();

    std::vector<double> returnVector(numberOfEntries * numberOfParameters);
    // Goes through all frames / entries
    for (int i = 0; i < numberOfEntries; i++) {
        // Goes through all parameters in the given entry. Goes through an integer, being the size
        // of the flag
        std::vector<data::DataFlags> dataFlags = data::extractFlags(m_dataFlags);
        for (int j = 0; j < dataFlags.size(); j++) {
            returnVector.push_back(m_data[i]->getValue(dataFlags[j]));
        }
    }
    return returnVector;
}

int DataContainer::getDataFlags() {
    return m_dataFlags;
}

int DataContainer::getNumberOfSetFlags() {
    // From https://stackoverflow.com/questions/109023/how-to-count-the-number-of-set-bits-in-a-32-bit-integer
    m_dataFlags = m_dataFlags - ((m_dataFlags >> 1) & 0x55555555);
    m_dataFlags = (m_dataFlags & 0x33333333) + ((m_dataFlags >> 2) & 0x33333333);
    return (((m_dataFlags + (m_dataFlags >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void DataContainer::addDataFlag(data::DataFlags flag) {
    clear();
    m_dataFlags |= flag;
    calculateObjectSize();
}

DataObject* DataContainer::appendNew() {
    m_data.push_back(new DataObject(m_dataFlags, m_objectSize));
    return *m_data.end();
}

int DataContainer::getSize() {
    return m_data.size();
}

DataContainer::~DataContainer() {
    // Since we allocated the DataObject's on the heap, we need to delete then, before deleting the
    // containing vector
    for (auto object : m_data) {
        delete object;
    }
}
