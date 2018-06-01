#include "catch.hpp"

#include "../lib/analyzer.h"
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

TEST_CASE("Individual dataFlags and values (basic)", "[full], [datacontainer]") {
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

TEST_CASE("All dataFlags set and values", "[full], [datacontainer]") {
    DataContainer container;

    SECTION("set all data-types, read chronologic") {
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
    SECTION("set all data-types, read chronologic") {
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
        REQUIRE(container[0]->getValue<double>(data::Perimeter) == 3.433);
        REQUIRE(container[0]->getValue<double>(data::Symmetry) == 34.943);
        REQUIRE(container[0]->getValue<double>(data::Solidity) == 0.9948);
        REQUIRE(container[0]->getValue<double>(data::Minor_axis) == 3.43);
        REQUIRE(container[0]->getValue<double>(data::Major_axis) == 5.342);
        REQUIRE(container[0]->getValue<int>(data::Label) == 21950);
        REQUIRE(container[0]->getValue<int>(data::Outlet) == 180);
        REQUIRE(container[0]->getValue<int>(data::Inlet) == 80);
        REQUIRE(container[0]->getValue<double>(data::GradientScore) == 5.005);
        REQUIRE(container[0]->getValue<int>(data::Frame) == 5);
        REQUIRE(container[0]->getValue<double>(data::Eccentricity) == 2.43);
        REQUIRE(container[0]->getValue<double>(data::ConvexArea) == 25.5);
        REQUIRE(container[0]->getValue<double>(data::Circularity) == 0.543);
        REQUIRE(container[0]->getValue<cv::Point>(data::Centroid) == cv::Point(11, 22));
        REQUIRE(container[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(container[0]->getValue<double>(data::Area) == 254.89);
        // assert get.PixelIdxList here
    }
    SECTION("set some data-types") {}
    SECTION("set some other data-types") {}
}

// AREA COMBO

TEST_CASE("All combinations with data::Area (1+1)", "[full], [datacontainer]") {
    DataContainer dc;
    dc.setDataFlags(data::Area);
    
    SECTION("Area + BoundingBox") {
        dc.addDataFlag(data::BoundingBox);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
    }
    SECTION("Area + Centroid") {
        dc.addDataFlag(data::Centroid);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Centroid, cv::Point(11, 22));
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<cv::Point>(data::Centroid) == cv::Point(11, 22));
    }
    SECTION("Area + Circularity") {
        dc.addDataFlag(data::Circularity);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Circularity, 0.543);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<double>(data::Circularity) == 0.543);
    }
    SECTION("Area + ConvexArea") {
        dc.addDataFlag(data::ConvexArea);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::ConvexArea, 25.5);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<double>(data::ConvexArea) == 25.5);
    }
    SECTION("Area + Eccentricity") {
        dc.addDataFlag(data::Eccentricity);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Eccentricity, 25.59);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<double>(data::Eccentricity) == 25.59);
    }
    SECTION("Area + Frame") {
        dc.addDataFlag(data::Frame);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Frame, 5);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<int>(data::Frame) == 5);
    }
    SECTION("Area + GradientScore") {
        dc.addDataFlag(data::GradientScore);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::GradientScore, 5.005);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<double>(data::GradientScore) == 5.005);
    }
    SECTION("Area + Inlet") {
        dc.addDataFlag(data::Inlet);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Inlet, 80);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<int>(data::Inlet) == 80);
    }
    SECTION("Area + Outlet") {
        dc.addDataFlag(data::Outlet);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Outlet, 180);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<int>(data::Outlet) == 180);
    }
    SECTION("Area + Label") {
        dc.addDataFlag(data::Label);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Label, 21950);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<int>(data::Label) == 21950);
    }
    SECTION("Area + Major_axis") {
        dc.addDataFlag(data::Major_axis);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Major_axis, 5.342);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<double>(data::Major_axis) == 5.342);
    }
    SECTION("Area + Minor_axis") {
        dc.addDataFlag(data::Minor_axis);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Minor_axis, 3.43);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<double>(data::Minor_axis) == 3.43);
    }
    SECTION("Area + Solidity") {
        dc.addDataFlag(data::Solidity);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Solidity, 0.9948);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<double>(data::Solidity) == 0.9948);
    }
    SECTION("Area + Symmetry") {
        dc.addDataFlag(data::Symmetry);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Symmetry, 34.943);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<double>(data::Symmetry) == 34.943);
    }
    SECTION("Area + Perimeter") {
        dc.addDataFlag(data::Perimeter);
        dc.appendNew();
        dc[0]->setValue(data::Area, 254.89);
        dc[0]->setValue(data::Perimeter, 3.433);
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
        REQUIRE(dc[0]->getValue<double>(data::Perimeter) == 3.433);
    }
}

// BOUNDINGBOX COMBO

TEST_CASE("All combinations with data::BoundingBox (1+1)", "[full], [datacontainer]") {
    DataContainer dc;
    dc.setDataFlags(data::BoundingBox);
    
    SECTION("BoundingBox + Area") {
        dc.addDataFlag(data::Area);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Area, 254.89);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<double>(data::Area) == 254.89);
    }
    SECTION("BoundingBox + Centroid") {
        dc.addDataFlag(data::Centroid);
        dc.appendNew();
        dc[0]->setValue(data::Centroid, cv::Point(11, 22));
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<cv::Point>(data::Centroid) == cv::Point(11, 22));
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
    }
    SECTION("BoundingBox + Circularity") {
        dc.addDataFlag(data::Circularity);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Circularity, 0.543);
        REQUIRE(dc[0]->getValue<double>(data::Circularity) == 0.543);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
    }
    SECTION("BoundingBox + ConvexArea") {
        dc.addDataFlag(data::ConvexArea);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::ConvexArea, 25.5);
        REQUIRE(dc[0]->getValue<double>(data::ConvexArea) == 25.5);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
    }
    SECTION("BoundingBox + Eccentricity") {
        dc.addDataFlag(data::Eccentricity);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Eccentricity, 25.59);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<double>(data::Eccentricity) == 25.59);
    }
    SECTION("BoundingBox + Frame") {
        dc.addDataFlag(data::Frame);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Frame, 5);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<int>(data::Frame) == 5);
    }
    SECTION("BoundingBox + GradientScore") {
        dc.addDataFlag(data::GradientScore);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::GradientScore, 5.005);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<double>(data::GradientScore) == 5.005);
    }
    SECTION("BoundingBox + Inlet") {
        dc.addDataFlag(data::Inlet);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Inlet, 80);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<int>(data::Inlet) == 80);
    }
    SECTION("BoundingBox + Outlet") {
        dc.addDataFlag(data::Outlet);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Outlet, 180);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<int>(data::Outlet) == 180);
    }
    SECTION("BoundingBox + Label") {
        dc.addDataFlag(data::Label);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Label, 21950);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<int>(data::Label) == 21950);
    }
    SECTION("BoundingBox + Major_axis") {
        dc.addDataFlag(data::Major_axis);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Major_axis, 5.342);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<double>(data::Major_axis) == 5.342);
    }
    SECTION("BoundingBox + Minor_axis") {
        dc.addDataFlag(data::Minor_axis);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Minor_axis, 3.43);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<double>(data::Minor_axis) == 3.43);
    }
    SECTION("BoundingBox + Solidity") {
        dc.addDataFlag(data::Solidity);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Solidity, 0.9948);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<double>(data::Solidity) == 0.9948);
    }
    SECTION("BoundingBox + Symmetry") {
        dc.addDataFlag(data::Symmetry);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Symmetry, 34.943);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<double>(data::Symmetry) == 34.943);
    }
    SECTION("BoundingBox + Perimeter") {
        dc.addDataFlag(data::Perimeter);
        dc.appendNew();
        dc[0]->setValue(data::BoundingBox, cv::Rect(0, 5, 10, 15));
        dc[0]->setValue(data::Perimeter, 3.433);
        REQUIRE(dc[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0, 5, 10, 15));
        REQUIRE(dc[0]->getValue<double>(data::Perimeter) == 3.433);
    }
}

TEST_CASE("typeMap", "[full], [datacontainer]") {
    REQUIRE(data::typeMap[data::Area].second == 8);
    REQUIRE(data::typeMap[data::BoundingBox].second == 16);
    REQUIRE(data::typeMap[data::Centroid].second == 8);
    REQUIRE(data::typeMap[data::Circularity].second == 8);
    REQUIRE(data::typeMap[data::ConvexArea].second == 8);
    REQUIRE(data::typeMap[data::Eccentricity].second == 8);
    REQUIRE(data::typeMap[data::Frame].second == 4);
    REQUIRE(data::typeMap[data::GradientScore].second == 8);
    REQUIRE(data::typeMap[data::Inlet].second == 4);
    REQUIRE(data::typeMap[data::Outlet].second == 4);
    REQUIRE(data::typeMap[data::Label].second == 4);
    REQUIRE(data::typeMap[data::Major_axis].second == 8);
    REQUIRE(data::typeMap[data::Minor_axis].second == 8);
    REQUIRE(data::typeMap[data::Solidity].second == 8);
    REQUIRE(data::typeMap[data::Symmetry].second == 8);
    REQUIRE(data::typeMap[data::Perimeter].second == 8);
    REQUIRE(data::typeMap[data::PixelIdxList].second == 8);
    REQUIRE(data::typeMap[data::OutputValue].second == 8);
}

TEST_CASE("Bytes to data from dataFlag", "[full], [datacontainter]") {
    DataContainer dc;
    size_t distanceKeeper = 0;

    SECTION("All flags set") {
        dc.setDataFlags(0x000fffff);
        dc.appendNew();
        for (auto const& dataType : data::typeMap) {
            if (dc.m_dataFlags & dataType.first) {
                REQUIRE(dc[0]->getBytesToData(dataType.first) == distanceKeeper);
                distanceKeeper += std::get<1>(dataType.second);
            }
        }
    }

    SECTION("Some flags set (0x0001abf3)") {
        dc.setDataFlags(0x0001abf3);
        dc.appendNew();
        for (auto const& dataType : data::typeMap) {
            if (dc.m_dataFlags & dataType.first) {
                REQUIRE(dc[0]->getBytesToData(dataType.first) == distanceKeeper);
                distanceKeeper += std::get<1>(dataType.second);
            }
        }
    }

    SECTION("Some flags set (0x00013b2f)") {
        dc.setDataFlags(0x00013b2f);
        dc.appendNew();
        for (auto const& dataType : data::typeMap) {
            if (dc.m_dataFlags & dataType.first) {
                REQUIRE(dc[0]->getBytesToData(dataType.first) == distanceKeeper);
                distanceKeeper += std::get<1>(dataType.second);
            }
        }
    }

    SECTION("Starting from data::Area") {
        dc.setDataFlags(0x000f);
        dc.appendNew();

        REQUIRE(dc[0]->getBytesToData(data::Area) == 0);
        REQUIRE(dc[0]->getBytesToData(data::BoundingBox) == 8);
        REQUIRE(dc[0]->getBytesToData(data::Centroid) == 24);
        REQUIRE(dc[0]->getBytesToData(data::Circularity) == 32);
    }

    SECTION("Specific combination: data::BoundingBox | data::Centroid") {
        dc.setDataFlags(data::BoundingBox | data::Centroid);
        dc.appendNew();

        REQUIRE(dc[0]->getBytesToData(data::BoundingBox) == 0);
        REQUIRE(dc[0]->getBytesToData(data::Centroid) == 16);
    }

    SECTION("Specific combination: data::BoundingBox | data::ConvexArea") {
        dc.setDataFlags(data::BoundingBox | data::ConvexArea);
        dc.appendNew();

        REQUIRE(dc[0]->getBytesToData(data::BoundingBox) == 0);
        REQUIRE(dc[0]->getBytesToData(data::ConvexArea) == 16);
    }

    SECTION("Specific combination: data::Centroid | data::Inlet") {
        dc.setDataFlags(data::Centroid | data::Inlet);
        dc.appendNew();

        REQUIRE(dc[0]->getBytesToData(data::Centroid) == 0);
        REQUIRE(dc[0]->getBytesToData(data::Inlet) == 8);
    }
}
