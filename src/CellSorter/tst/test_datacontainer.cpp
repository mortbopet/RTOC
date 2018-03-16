#include "../lib/datacontainer.h"

#include <iostream>

int test_datacontainer() {
    int dataFlags = data::Area | data::Circularity;

    DataContainer container(dataFlags);

    container.appendNew();
    container.setValue(0, data::Area, 12345678);

    assert(container.getValue<int>(0, data::Area) == 12345678);

    std::cout << "test_datacontainer() passed" << std::endl;

    return 0;
}
