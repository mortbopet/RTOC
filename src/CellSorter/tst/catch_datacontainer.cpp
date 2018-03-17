#include "catch.hpp"

#include "../lib/datacontainer.h"

TEST_CASE("DataContainer Class Test", "[datacontainer]") {
    SECTION("append ") {
        DataContainer container;
        container.addDataFlag(data::Area);
        REQUIRE_NOTHROW(container.appendNew());
    }
    SECTION("append and setValue") {
        DataContainer container(data::Area);
        REQUIRE_NOTHROW(container.appendNew());
        REQUIRE_NOTHROW(container.setValue(0, data::Area, 12.3));
        REQUIRE(container.getValue<double>(0,data::Area) == 12.3);
    }
    SECTION("illegal calls / test for exceptions") {

    }
    SECTION("size/length of container") {

    }
    SECTION("clear container") {

    }
    SECTION("clear dataFlags") {

    }
}

TEST_CASE("dataFlags and values (basic)", "[datacontainer]") {
    SECTION("Area") {
        DataContainer container(data::Area);

        container.appendNew();
        container.setValue(0, data::Area, 12.54);
        REQUIRE(container.getValue<double>(0, data::Area) == 12.54);
    }
    SECTION("BoundingBox") {
        DataContainer container(data::BoundingBox);

        container.appendNew();
        container.setValue(0, data::BoundingBox, cv::Rect(0,5,10,15));
        cv::Rect res = container.getValue<cv::Rect>(0, data::BoundingBox);
        REQUIRE(res.x == 0);
        REQUIRE(res.y == 5);
        REQUIRE(res.width == 10);
        REQUIRE(res.height == 15);
    }
    SECTION("Centroid") {
        DataContainer container(data::Centroid);

        container.appendNew();
        container.setValue(0, data::Centroid, std::pair<int,int>(11,22));
        std::pair<int,int> centroid = container.getValue<std::pair<int,int>>(0, data::Centroid);
        REQUIRE(centroid.first == 11);
        REQUIRE(centroid.second == 22);
    }
    SECTION("Circularity") {
        DataContainer container(data::Circularity);

        container.appendNew();
        container.setValue(0, data::Circularity, 0.957);
        double res = container.getValue<double>(0,data::Circularity);
        REQUIRE(res == 0.957);
    }
    SECTION("Eccentricity") {
        DataContainer container(data::Eccentricity);

        container.appendNew();
        container.setValue(0, data::Eccentricity, 2.57);
        double res = container.getValue<double>(0,data::Eccentricity);
        REQUIRE(res == 2.57);
    }
    SECTION("Frame") {
        DataContainer container(data::Frame);

        container.appendNew();
        container.setValue(0, data::Frame, 1234);
        int res = container.getValue<int>(0,data::Frame);
        REQUIRE(res == 1234);
    }
    SECTION("GradientScore") {
        DataContainer container(data::GradientScore);

        container.appendNew();
        container.setValue(0, data::GradientScore, 2.257);
        double res = container.getValue<double>(0,data::GradientScore);
        REQUIRE(res == 2.257);
    }
    SECTION("Inlet") {
        DataContainer container(data::Inlet);

        container.appendNew();
        container.setValue(0, data::Inlet, 80);
        int res = container.getValue<int>(0,data::Inlet);
        REQUIRE(res == 80);
    }
    SECTION("Outlet") {
        DataContainer container(data::Outlet);

        container.appendNew();
        container.setValue(0, data::Outlet, 180);
        int res = container.getValue<int>(0,data::Outlet);
        REQUIRE(res == 180);
    }
    SECTION("Label") {
        DataContainer container(data::Label);

        container.appendNew();
        container.setValue(0, data::Label, 28109);
        int res = container.getValue<int>(0,data::Label);
        REQUIRE(res == 28109);
    }
    SECTION("Major_axis") {
        DataContainer container(data::Major_axis);

        container.appendNew();
        container.setValue(0, data::Major_axis, 25.349);
        double res = container.getValue<double>(0,data::Major_axis);
        REQUIRE(res == 25.349);
    }
    SECTION("Minor_axis") {
        DataContainer container(data::Minor_axis);

        container.appendNew();
        container.setValue(0, data::Minor_axis, 15.994);
        double res = container.getValue<double>(0,data::Minor_axis);
        REQUIRE(res == 15.994);
    }
    SECTION("Solidity") {
        DataContainer container(data::Solidity);

        container.appendNew();
        container.setValue(0, data::Solidity, 0.994);
        double res = container.getValue<double>(0,data::Solidity);
        REQUIRE(res == 0.994);
    }
    SECTION("Symmetry") {
        DataContainer container(data::Symmetry);

        container.appendNew();
        container.setValue(0, data::Symmetry, 1.994);
        double res = container.getValue<double>(0,data::Symmetry);
        REQUIRE(res == 1.994);
    }
    SECTION("Perimeter") {
        DataContainer container(data::Perimeter);

        container.appendNew();
        container.setValue(0, data::Perimeter, 1.394);
        double res = container.getValue<double>(0,data::Perimeter);
        REQUIRE(res == 1.394);
    }
    SECTION("PixelIdxList") {
        DataContainer container(data::PixelIdxList);

        container.appendNew();
        /// Write proper setValue here
        // container.setValue(0, data::PixelIdxList, 1.394);
        // container.getValue<std::vector<cv::Point>>(0,data::PixelIdxList);
        /// Insert requirement here
    }
}

TEST_CASE("dataFlag and values (all flags set)", "[datacontainer") {
    // Write test sections here
}