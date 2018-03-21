#include "catch.hpp"

#include "../lib/matlab_ext.h"

TEST_CASE("Matlab namespace basic test", "[full], [matlab_ext]") {
    INFO("This test has not been written yet...");
}

TEST_CASE("Regionprops throws and hoes check", "[full], [matlab_ext]") {
    // Initialize output container
    DataContainer output(0xffff);
    // Initialize test "image"
    cv::Mat img;
    SECTION("Take output from empty container") {
        REQUIRE(matlab::regionProps(cv::Mat(), data::Area, output) == 0);
    }
}

TEST_CASE("Regionprops value verify (with simple non-rotated rectangles", "[full], [matlab_ext]") {
    // Initialize output container
    DataContainer output(0xffff);
    // Initialize test "image"
    cv::Mat img = cv::Mat::zeros(120,240,CV_8U);

    SECTION("area") {
        img(cv::Rect(0,0,1,1)) = 255;
        REQUIRE(matlab::regionProps(img, data::Area, output) == 1);
        CHECK(output[0]->getValue<double>(data::Area) == 0);
        img(cv::Rect(0,0,2,2)) = 255;
        REQUIRE(matlab::regionProps(img, data::Area, output) == 1);
        CHECK(output[0]->getValue<double>(data::Area) == 1);
        img(cv::Rect(0,0,4,4)) = 255;
        REQUIRE(matlab::regionProps(img, data::Area, output) == 1);
        CHECK(output[0]->getValue<double>(data::Area) == 9);
        img(cv::Rect(50,50,10,10)) = 255;
        REQUIRE(matlab::regionProps(img, data::Area, output) == 2);
        CHECK(output[0]->getValue<double>(data::Area) == 81);
        CHECK(output[1]->getValue<double>(data::Area) == 9);
    }
    SECTION("boundingBox") {
        img(cv::Rect(0,0,1,1)) = 255;
        REQUIRE(matlab::regionProps(img, data::BoundingBox, output) == 1);
        CHECK(output[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0,0,1,1));
        img(cv::Rect(0,0,5,5)) = 255;
        REQUIRE(matlab::regionProps(img, data::BoundingBox, output) == 1);
        CHECK(output[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0,0,5,5));
        img(cv::Rect(50,50,10,10)) = 255;
        REQUIRE(matlab::regionProps(img, data::BoundingBox, output) == 2);
        CHECK(output[0]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(50,50,10,10));
        CHECK(output[1]->getValue<cv::Rect>(data::BoundingBox) == cv::Rect(0,0,5,5));
    }
    SECTION("centroid (rectangles)") {
        img(cv::Rect(10,10,7,7)) = 255;
        REQUIRE(matlab::regionProps(img, data::Centroid, output) == 1);
        CHECK(output[0]->getValue<cv::Point>(data::Centroid) == cv::Point(13,13));
        img(cv::Rect(10,10,9,9)) = 255;
        REQUIRE(matlab::regionProps(img, data::Centroid, output) == 1);
        CHECK(output[0]->getValue<cv::Point>(data::Centroid) == cv::Point(14,14));
        img(cv::Rect(40,40,13,13)) = 255;
        REQUIRE(matlab::regionProps(img, data::Centroid, output) == 2);
        CHECK(output[0]->getValue<cv::Point>(data::Centroid) == cv::Point(46,46));
        CHECK(output[1]->getValue<cv::Point>(data::Centroid) == cv::Point(14,14));
    }
    SECTION("centroid (other shapes)") {
        cv::Point polygon[1][6];
        polygon[0][0] = cv::Point(10,15);
        polygon[0][1] = cv::Point(40,15);
        polygon[0][2] = cv::Point(45,35);
        polygon[0][3] = cv::Point(20,35);
        polygon[0][4] = cv::Point(20,25);
        polygon[0][5] = cv::Point(10,15);
        const cv::Point* ppt[1] = { polygon[0] };
        int npt[1] = { 6 };
        cv::fillPoly(img, ppt, npt, 1, cv::Scalar(255), 8);
        REQUIRE(matlab::regionProps(img, data::Centroid, output) == 1);
        cv::Point centroid = output[0]->getValue<cv::Point>(data::Centroid);
        CHECK(centroid == cv::Point(29,24));
        cv::circle(img, cv::Point(140,50), 20, cv::Scalar(255), -1);
        REQUIRE(matlab::regionProps(img, data::Centroid, output) == 2);
        CHECK(output[0]->getValue<cv::Point>(data::Centroid) == cv::Point(140,50));
        CHECK(output[1]->getValue<cv::Point>(data::Centroid) == cv::Point(29,24));
    }
    SECTION("circularity") {
        cv::circle(img, cv::Point(5,5), 2, cv::Scalar(255), -1);
        cv::circle(img, cv::Point(20,20), 5, cv::Scalar(255), -1);
        cv::circle(img, cv::Point(50,50), 10, cv::Scalar(255), -1);
        cv::circle(img, cv::Point(130,60), 50, cv::Scalar(255), -1);
        // img(cv::Rect(50,50,5,5)) = 255;
        REQUIRE(matlab::regionProps(img, data::Circularity, output) == 4);
        CHECK(output[0]->getValue<double>(data::Circularity) == Approx(0.9).epsilon(0.1));
        CHECK(output[1]->getValue<double>(data::Circularity) == Approx(0.8).epsilon(0.1));
        CHECK(output[2]->getValue<double>(data::Circularity) == Approx(0.9).epsilon(0.1));
        CHECK(output[3]->getValue<double>(data::Circularity) == Approx(0.6).epsilon(0.1));
    }
    SECTION("convexArea") {
        cv::circle(img, cv::Point(120,60), 10, cv::Scalar(255), -1);
        REQUIRE(matlab::regionProps(img, data::ConvexArea, output) == 1);
        CHECK(output[0]->getValue<double>(data::ConvexArea) == 304);
        // But is this the output we want? do we care?
    }
    SECTION("eccentricity") {
        cv::circle(img, cv::Point(120,60), 25, cv::Scalar(255), -1);
        img(cv::Rect(10,10,7,15)) = 255;
        REQUIRE(matlab::regionProps(img, data::Eccentricity, output) == 2);
        CHECK(output[0]->getValue<double>(data::Eccentricity) == 1);
        CHECK(output[1]->getValue<double>(data::Eccentricity) == Approx(0.38).epsilon(0.1));
        // zero zero is def not the right input..
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