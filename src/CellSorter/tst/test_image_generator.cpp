#include "catch.hpp"
#include <opencv/cv.hpp>
#include <vector>

TEST_CASE("Generate test image", "[full], [image_generator]") {
    INFO("This is not an actual test case.");
    INFO(" - servers as an image generator for proper testing");
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    SECTION("lol") {
        cv::Mat image = cv::Mat::zeros(120,240,CV_8U);
        cv::circle(image, cv::Point(120,60), 10, cv::Scalar(255), -1);
        REQUIRE(cv::imwrite("./test_image.png", image, compression_params));
    }
}