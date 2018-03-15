#include "../lib/datacontainer.h"

#include <iostream>

int test_datacontainer() {
    auto dataObjectPtr = new DataObject(0x1001);
    delete dataObjectPtr;

    DataContainer container;
    container.setDataFlags(0b1001);
    container.appendNew();
    container.setValue(0, data::Area, 2);
    assert(container.getValue<int>(0, data::Area) == 2);

    std::cout << "test_datacontainer() passed" << std::endl;

    return 0;
}
