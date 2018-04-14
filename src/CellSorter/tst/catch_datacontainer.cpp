#include "catch.hpp"

#include "../lib/datacontainer.h"

TEST_CASE("DataContainer Class Test", "[full], [datacontainer]") {
    DataContainer container;
    SECTION("append ") {
        container.addDataFlag(data::Area);
        REQUIRE_NOTHROW(container.appendNew());
    }
    SECTION("append and setValue") {
        container.setDataFlags(data::Area);
        REQUIRE_NOTHROW(container.appendNew());
        REQUIRE_NOTHROW(container[0]->setValue(data::Area, 540.43));
        REQUIRE(container[0]->getValue<double>(data::Area) == 540.43);
    }

    SECTION("illegal calls / test for non-set dataFlags") {
        container.setDataFlags(data::Area | data::Circularity | data::Inlet);
        container.appendNew();
        REQUIRE_THROWS(container[0]->setValue(data::Centroid, cv::Point(1, 3)));
        REQUIRE_THROWS(container[0]->setValue(data::Eccentricity, -143.395834));
        REQUIRE_THROWS(container[0]->setValue(data::Centroid, 13413));
        REQUIRE_THROWS(container[0]->setValue(data::Eccentricity, true));
    }
    SECTION("size/length of container") {}
    SECTION("clear container") {}
    SECTION("clear dataFlags") {}
}

TEST_CASE("dataFlags and values (basic)", "[full], [datacontainer]") {
    SECTION("Area") {
        DataContainer container(data::Area);

        container.appendNew();
        container[0]->setValue(data::Area, 12.43);
        REQUIRE(container[0]->getValue<double>(data::Area) == 12.43);
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
        container[0]->setValue(data::Centroid, cv::Point(11, 22));
        cv::Point centroid = container[0]->getValue<cv::Point>(data::Centroid);
        REQUIRE(centroid.x == 11);
        REQUIRE(centroid.y == 22);
    }
    SECTION("Circularity") {
        DataContainer container(data::Circularity);

        container.appendNew();
        container[0]->setValue(data::Circularity, 0.957);
        double res = container[0]->getValue<double>(data::Circularity);
        REQUIRE(res == 0.957);
    }
    SECTION("ConvexArea") {
        DataContainer container(data::ConvexArea);

        container.appendNew();
        container[0]->setValue(data::ConvexArea, 15.3);
        double res = container[0]->getValue<double>(data::ConvexArea);
        REQUIRE(res == 15.3);
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
        // Write proper setValue here
        // container[0]->setValue( data::PixelIdxList, /* PixelIdxList value here */);
        // container[0]->getValue<std::vector<cv::Point>>(0,data::PixelIdxList);
        // Insert assertion here
    }
}

TEST_CASE("dataFlag and values (multiple at once)", "[full], [datacontainer]") {
    DataContainer container;

    SECTION("set all data-types") {
        container.setDataFlags(0xffff);
        container.appendNew();
        container[0]->setValue(data::Area, 254.89);
        container[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        container[0]->setValue(data::Centroid, cv::Point(11, 22));
        container[0]->setValue(data::Circularity, 0.543);
        container[0]->setValue(data::ConvexArea, 25.5);
        container[0]->setValue(data::Eccentricity, 2.43);
        container[0]->setValue(data::Frame, 5);
        container[0]->setValue(data::GradientScore, 5.005);
        container[0]->setValue(data::Inlet, 80);
        container[0]->setValue(data::Outlet, 180);
        container[0]->setValue(data::Label, 21950);
        container[0]->setValue(data::Major_axis, 5.342);
        container[0]->setValue(data::Minor_axis, 3.43);
        container[0]->setValue(data::Solidity, 0.9948);
        container[0]->setValue(data::Symmetry, 34.943);
        container[0]->setValue(data::Perimeter, 3.433);
        // set PixelIdxList here
        REQUIRE(container[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(container[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(container[0]->getValue<cv::Point>(data::Centroid) == cv::Point(11, 22));
        REQUIRE(container[0]->getValue<double>(data::Circularity) == 0.543);
        REQUIRE(container[0]->getValue<double>(data::ConvexArea) == 25.5);
        REQUIRE(container[0]->getValue<double>(data::Eccentricity) == 2.43);
        REQUIRE(container[0]->getValue<int>(data::Frame) == 5);
        REQUIRE(container[0]->getValue<double>(data::GradientScore) == 5.005);
        REQUIRE(container[0]->getValue<int>(data::Inlet) == 80);
        REQUIRE(container[0]->getValue<int>(data::Outlet) == 180);
        REQUIRE(container[0]->getValue<int>(data::Label) == 21950);
        REQUIRE(container[0]->getValue<double>(data::Major_axis) == 5.342);
        REQUIRE(container[0]->getValue<double>(data::Minor_axis) == 3.43);
        REQUIRE(container[0]->getValue<double>(data::Solidity) == 0.9948);
        REQUIRE(container[0]->getValue<double>(data::Symmetry) == 34.943);
        REQUIRE(container[0]->getValue<double>(data::Perimeter) == 3.433);
        // assert get.PixelIdxList here
    }
    SECTION("set some data-types") {

    }
    SECTION("set some other data-types") {

    }
}

TEST_CASE("numberOfFlags") {
    DataContainer container1;
    DataContainer container2;
    DataContainer container3;

    SECTION("numberOfFlags") {
        container1.setDataFlags(0x0f0f);
        container2.setDataFlags(0xff5c);
        REQUIRE(container1.numberOfFlags() == 8);
        REQUIRE(container2.numberOfFlags() == 12);
        REQUIRE(container3.numberOfFlags() == 0);
    }
}

TEST_CASE("extractDataObjectVector and extractDataContainerVector") {
    DataContainer container;
    container.setDataFlags(0x0011);
    container.appendNew();
    container.appendNew();
    container.appendNew();
    container[0]->setValue(data::Area, 20.1);
    container[1]->setValue(data::Area, 23.1);
    container[2]->setValue(data::Area, 26.1);
    container[0]->setValue(data::ConvexArea, 3.1);
    container[1]->setValue(data::ConvexArea, 4.1);
    container[2]->setValue(data::ConvexArea, 12.1);

    SECTION("Extract all data from a single data object") {
        std::vector<double> compareVector1 = {20.1, 3.1};
        std::vector<double> compareVector2 = {23.1, 4.1};
        std::vector<double> compareVector3 = {26.1, 12.1};
        REQUIRE (container.extractObject(0) == compareVector1);
        REQUIRE (container.extractObject(1) == compareVector2);
        REQUIRE (container.extractObject(2) == compareVector3);
    }

    SECTION("Extract all data from multiple data objects within same container") {
        std::vector<std::vector<double>> compareMatrix = {{20.1, 3.1},{23.1, 4.1},{26.1, 12.1}};
        REQUIRE (container.extractContainer() == compareMatrix);
    }
}