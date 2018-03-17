#include "catch.hpp"

#include "../lib/datacontainer.h"

TEST_CASE("DataContainer Constructor", "[datacontainer]") {
    SECTION("append ") {
        DataContainer container;
        container.addDataFlag(data::Area);
        REQUIRE_NOTHROW(container.appendNew());
    }
    SECTION("append and setValue") {
        DataContainer container(data::Area);
        REQUIRE_NOTHROW(container.appendNew());
        REQUIRE_NOTHROW(container.setValue(0, data::Area, 12.5));
        REQUIRE(container.getValue<double>(0,data::Area) == 12.3);
    }
    SECTION("size/length of container") {

    }
    SECTION("clear container") {

    }
    SECTION("clear dataFlags") {

    }
}

TEST_CASE("dataFlags and values", "[datacontainer]") {
    int dataFlags;
    SECTION("Area") {
        dataFlags = data::Area;
        DataContainer container(dataFlags);

        container.appendNew();
        container.setValue(0, data::Area, 12.54);
        REQUIRE(container.getValue<double>(0, data::Area) == 12.54);
    }
    SECTION("BoundingBox") {
        dataFlags = data::BoundingBox;
        DataContainer container(dataFlags);

        container.appendNew();
        container.setValue(0, data::BoundingBox, cv::Rect(0,5,10,15));
        cv::Rect res = container.getValue<cv::Rect>(0, data::BoundingBox);
        REQUIRE(res.x == 0);
        REQUIRE(res.y == 5);
        REQUIRE(res.width == 10);
        REQUIRE(res.height == 15);
    }
    SECTION("Centroid") {

    }
    SECTION("Circularity") {

    }
    SECTION("Eccentricity") {

    }
    SECTION("Frame") {

    }
    SECTION("GradientScore") {

    }
    SECTION("Inlet") {

    }
    SECTION("Outlet") {

    }
    SECTION("Label") {

    }
    SECTION("Major_axis") {

    }
    SECTION("Minor_axis") {

    }
    SECTION("Solidity") {

    }
    SECTION("Symmetry") {

    }
    SECTION("Perimeter") {

    }
    SECTION("PixelIdxList") {

    }
}
