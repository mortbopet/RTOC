#include "catch.hpp"

#include "../lib/matlab_ext.h"

TEST_CASE("Matlab namespace basic test", "[matlab_ext]") {
    INFO("This test has not been written yet...");
}

TEST_CASE("Regionprops test", "[matlab_ext]") {
    // Initialize output container
    DataContainer output(0xffff);
    output.appendNew();
    // Initialize test "image"
    cv::Mat img;

    SECTION("area") {
        matlab::regionProps(img,data::Area,output);
        REQUIRE(output[0]->getValue<int>(data::Area) == 10);
    }
    SECTION("boundingBox") {

    }
    SECTION("centroid") {

    }
    SECTION("circularity") {

    }
    SECTION("convexArea") {

    }
    SECTION("eccentricity") {

    }
    SECTION("gradientScore") {

    }
    SECTION("majorAxis") {

    }
    SECTION("minorAxis") {

    }
    SECTION("solidity") {

    }
    SECTION("symmetry") {

    }
    SECTION("perimeter") {

    }
}