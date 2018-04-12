#include "catch.hpp"

#include "../lib/machinelearning.h"
#include "../lib/datacontainer.h"

TEST_CASE("Add to train set and convert to CV format", "[full], [machinelearning]") {
    // Tests for 2 observations with each 3 attributes

    LogisticRegression ml;
    DataContainer container1(0xffff);
    container1.appendNew();
    container1.appendNew();
    container1.appendNew();

    container1[0]->setValue(data::Area,30);
    container1[1]->setValue(data::Area,40);
    container1[2]->setValue(data::Area,35);

    DataContainer container2(0xffff);
    container2.appendNew();
    container2.appendNew();
    container2.appendNew();
    container2[0]->setValue(data::Area,50);
    container2[1]->setValue(data::Area,55);
    container2[2]->setValue(data::Area,60);

    SECTION("add to model") {
        ml.add_to_trainset(&container1);
        ml.add_to_trainset(&container2);
        // Test
    }

    SECTION("convert into CV format") {
        ml.prepareModel();
        // Test
    }
}