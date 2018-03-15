#include "datacontainer.h"

#include <assert.h>
#include <cstring>

// --------------------- DataObject ---------------------------
DataObject::DataObject(long dataFlags) {
    m_size = 0;
    // Iterate through the typeMap and match against dataFlags. If match is found, increment m_size
    // with the corresponding data type size
    /** @todo move this to DataContainer and pass as a constructor argument
     */
    for (const auto& item : data::typeMap) {
        if (item.first & dataFlags) {
            m_size += item.second.second;
        }
    }

    // Allocate the requested amount of memory
    m_memory = malloc(m_size);
}

DataObject::~DataObject() {
    free(m_memory);
}

size_t DataObject::getBytesToData(data::DataFlags flag) {
    size_t bytesToData = 0;
    // Iterate through the typeMap and match against flag. until flag is found, add up the
    // size of data that lies behind the requested data
    for (const auto& item : data::typeMap) {
        if (!(item.first & flag)) {
            bytesToData += item.second.second;
        } else {
            break;
        }
    }
    return bytesToData;
}

// --------------------- DataContainer ------------------------
DataContainer::DataContainer() {}

void DataContainer::setDataFlags(data::DataFlags flag) {
    clear();
    m_dataFlags = flag;
}

void DataContainer::setDataFlags(long flag) {
    clear();
    m_dataFlags = flag;
}

void DataContainer::addDataFlag(data::DataFlags flag) {
    clear();
    m_dataFlags |= flag;
}

DataObject& DataContainer::appendNew() {
    m_data.push_back(new DataObject(m_dataFlags));
    return **m_data.end();
}

DataContainer::~DataContainer() {
    // Since we allocated the DataObject's on the heap, we need to delete then, before deleting the
    // containing vector
    for (auto object : m_data) {
        delete object;
    }
}
