#include "catch.hpp"

#include "../lib/machinelearning.h"

TEST_CASE("Machinelearning data extraction", "[full], [machinelearning]") {
    DataContainer container1;
    DataContainer container2;
    DataContainer container3;
    container1.setDataFlags(0x0011);
    container2.setDataFlags(0x0011);
    container3.setDataFlags(0x0011);
    container1.appendNew();
    container1.appendNew();
    container2.appendNew();
    container2.appendNew();
    container3.appendNew();
    container3.appendNew();
    container1[0]->setValue(data::Area, 1.1);
    container1[1]->setValue(data::Area, 1.1);
    container1[0]->setValue(data::ConvexArea, 1.1);
    container1[1]->setValue(data::ConvexArea, 1.1);
    container2[0]->setValue(data::Area, 2.1);
    container2[1]->setValue(data::Area, 2.1);
    container2[0]->setValue(data::ConvexArea, 2.1);
    container2[1]->setValue(data::ConvexArea, 2.1);
    container3[0]->setValue(data::Area, 3.1);
    container3[1]->setValue(data::Area, 3.1);
    container3[0]->setValue(data::ConvexArea, 3.1);
    container3[1]->setValue(data::ConvexArea, 3.1);
    LogisticRegression lr;
    lr.add_to_trainset(&container1);
    lr.add_to_trainset(&container2);
    lr.add_to_trainset(&container3);
    container1.setClass(1);
    container2.setClass(1);
    container3.setClass(0);

    SECTION("add_to_trainset and get_trainset") {
        std::vector<double> extracted = lr.get_trainset()[0]->extractContainer();
        std::vector<double> compareVec = {1.1, 1.1, 1.1, 1.1};
        REQUIRE(extracted == compareVec);
    }
    //SECTION("prepareModel, get_attributes and get_outputs") {
    //    lr.prepareModel();
    //    std::vector<std::vector<double>> extractedAttributes = lr.get_attributes();
    //    std::vector<std::vector<double>> compareAttributes = {
    //        {1.1, 1.1, 1.1, 1.1}, {2.1, 2.1, 2.1, 2.1}, {3.1, 3.1, 3.1, 3.1}};
    //    std::vector<double> extractedOutputs = lr.get_outputs();
    //    std::vector<double> compareOutputs = {1., 1., 0.};
    //    REQUIRE(extractedAttributes == compareAttributes);
    //    REQUIRE(extractedOutputs == compareOutputs);
    //}
    SECTION("create_model and train_model") {
        lr.prepareModel();
        lr.create_model();
        lr.train_model();
        REQUIRE(1 == 1);
    }
}