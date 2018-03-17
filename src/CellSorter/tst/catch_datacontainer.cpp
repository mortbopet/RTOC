#include "catch.hpp"

#include "../lib/datacontainer.h"

TEST_CASE("DataContainer Class Test", "[datacontainer]") {
    DataContainer container;
    SECTION("append ") {
        container.addDataFlag(data::Area);
        REQUIRE_NOTHROW(container.appendNew());
    }
    SECTION("append and setValue") {
        container.setDataFlags(data::Area);
        REQUIRE_NOTHROW(container.appendNew());
        REQUIRE_NOTHROW(container[0]->setValue(data::Area, 12.3));
        REQUIRE(container[0]->getValue<double>(data::Area) == 12.3);
    }

    SECTION("illegal calls / test for exceptions") {
        container.setDataFlags(data::Area | data::Circularity | data::Inlet);
        container.appendNew();
        REQUIRE_THROWS(container[0]->setValue(data::Centroid, std::pair<int, int>(1, 3)));
        REQUIRE_THROWS(container[0]->setValue(data::Eccentricity, -143.395834));
        REQUIRE_THROWS(container[0]->setValue(data::Centroid, 13413));
        REQUIRE_THROWS(container[0]->setValue(data::Eccentricity, true));
    }
    SECTION("size/length of container") {}
    SECTION("clear container") {}
    SECTION("clear dataFlags") {}
}

TEST_CASE("dataFlags and values (basic)", "[datacontainer]") {
    SECTION("Area") {
        DataContainer container(data::Area);

        container.appendNew();
        container[0]->setValue(data::Area, 12.54);
        REQUIRE(container[0]->getValue<double>(data::Area) == 12.54);
    }
    SECTION("BoundingBox") {
        DataContainer container(data::BoundingBox);

        container.appendNew();
        container[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        cv::Rect res = container[0]->getValue<cv::Rect>(data::BoundingBox);
        REQUIRE(res.x == 0);
        REQUIRE(res.y == 5);
        REQUIRE(res.width == 10);
        REQUIRE(res.height == 15);
    }
    SECTION("Centroid") {
        DataContainer container(data::Centroid);

        container.appendNew();
        container[0]->setValue(data::Centroid, std::pair<int, int>(11, 22));
        std::pair<int, int> centroid = container[0]->getValue<std::pair<int, int>>(data::Centroid);
        REQUIRE(centroid.first == 11);
        REQUIRE(centroid.second == 22);
    }
    SECTION("Circularity") {
        DataContainer container(data::Circularity);

        container.appendNew();
        container[0]->setValue(data::Circularity, 0.957);
        double res = container[0]->getValue<double>(data::Circularity);
        REQUIRE(res == 0.957);
    }
    SECTION("Eccentricity") {
        DataContainer container(data::Eccentricity);

        container.appendNew();
        container[0]->setValue(data::Eccentricity, 2.57);
        double res = container[0]->getValue<double>(data::Eccentricity);
        REQUIRE(res == 2.57);
    }
    SECTION("Frame") {
        DataContainer container(data::Frame);

        container.appendNew();
        container[0]->setValue(data::Frame, 1234);
        int res = container[0]->getValue<int>(data::Frame);
        REQUIRE(res == 1234);
    }
    SECTION("GradientScore") {
        DataContainer container(data::GradientScore);

        container.appendNew();
        container[0]->setValue(data::GradientScore, 2.257);
        double res = container[0]->getValue<double>(data::GradientScore);
        REQUIRE(res == 2.257);
    }
    SECTION("Inlet") {
        DataContainer container(data::Inlet);

        container.appendNew();
        container[0]->setValue(data::Inlet, 80);
        int res = container[0]->getValue<int>(data::Inlet);
        REQUIRE(res == 80);
    }
    SECTION("Outlet") {
        DataContainer container(data::Outlet);

        container.appendNew();
        container[0]->setValue(data::Outlet, 180);
        int res = container[0]->getValue<int>(data::Outlet);
        REQUIRE(res == 180);
    }
    SECTION("Label") {
        DataContainer container(data::Label);

        container.appendNew();
        container[0]->setValue(data::Label, 28109);
        int res = container[0]->getValue<int>(data::Label);
        REQUIRE(res == 28109);
    }
    SECTION("Major_axis") {
        DataContainer container(data::Major_axis);

        container.appendNew();
        container[0]->setValue(data::Major_axis, 25.349);
        double res = container[0]->getValue<double>(data::Major_axis);
        REQUIRE(res == 25.349);
    }
    SECTION("Minor_axis") {
        DataContainer container(data::Minor_axis);

        container.appendNew();
        container[0]->setValue(data::Minor_axis, 15.994);
        double res = container[0]->getValue<double>(data::Minor_axis);
        REQUIRE(res == 15.994);
    }
    SECTION("Solidity") {
        DataContainer container(data::Solidity);

        container.appendNew();
        container[0]->setValue(data::Solidity, 0.994);
        double res = container[0]->getValue<double>(data::Solidity);
        REQUIRE(res == 0.994);
    }
    SECTION("Symmetry") {
        DataContainer container(data::Symmetry);

        container.appendNew();
        container[0]->setValue(data::Symmetry, 1.994);
        double res = container[0]->getValue<double>(data::Symmetry);
        REQUIRE(res == 1.994);
    }
    SECTION("Perimeter") {
        DataContainer container(data::Perimeter);

        container.appendNew();
        container[0]->setValue(data::Perimeter, 1.394);
        double res = container[0]->getValue<double>(data::Perimeter);
        REQUIRE(res == 1.394);
    }
    SECTION("PixelIdxList") {
        DataContainer container(data::PixelIdxList);

        container.appendNew();
        /// Write proper setValue here
        // container[0]->setValue( data::PixelIdxList, 1.394);
        // container[0]->getValue<std::vector<cv::Point>>(0,data::PixelIdxList);
        /// Insert requirement here
    }
}

TEST_CASE("dataFlag and values (multiple at once)", "[datacontainer") {
    DataContainer container;

    SECTION("set all data-types") {
        container.setDataFlags(0xffff);
        container.appendNew();
        container[0]->setValue(data::Area,0.254);
        container[0]->setValue(data::BoundingBox, cv::Rect(0,5,10,15));
        container[0]->setValue(data::Centroid, std::pair<int,int>(11,22));

    }
    SECTION("set some data-types") {

    }
    SECTION("set some other data-types") {
s
    }
}
