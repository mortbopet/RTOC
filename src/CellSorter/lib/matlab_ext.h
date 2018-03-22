#ifndef CELLSORTER_MATLAB_EXT_H
#define CELLSORTER_MATLAB_EXT_H

#include <opencv/cv.hpp>
#include <cmath>
#include <iostream>

#include "datacontainer.h"

namespace matlab {
    double PI = 3.14159265358979;

    void bwareaopen(cv::Mat &im, double size) {
        // Only accept CV_8UC
        if (im.channels() != 1 || im.type() != CV_8U) {
            return;
        }

        // Find all contours
        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(im.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        for (int i = 0; i < contours.size(); i++) {
            // Calculate contour area
            double area = cv::contourArea(contours[i]);

            // Remove small objects by drawing the contour with black color
            if (area <= size) {
                cv::drawContours(im, contours, i, CV_RGB(0, 0, 0), -1);
            }
        }
    }
    

    /**
     * \brief
     *
     *
     *
     * @param img
     * @param dataFlags
     * @param dataContainer
     *
     * @return seperate contours found
     */
    int regionProps(const cv::Mat& img, const int& dataFlags, DataContainer& dataContainer) {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        double area = 0;
        cv::Rect boundingBox;
        cv::Point centroid;
        double circularity;
        double convexArea = 0;
        double eccentricity = 0.0;
        double gradientScore = 0.0;
        double majorAxis = 0.0;
        double minorAxis = 0.0;
        double solidity = 0.0;
        double symmetry = 0.0;
        double perimeter = 0.0;

        int i = 0; // dataContainer index
        // for every contour -> new regionprops object with m_parameters
        for (const std::vector<cv::Point>& contour : contours) {
            dataContainer.appendNew();

            /// Area
            if (dataFlags & (data::Area | data::Circularity | data::Solidity)) {
                // Compute the area of the contour (not exact pixel count!)
                area = cv::contourArea(contour, false);

                dataContainer[i]->setValue(data::Area, area);
            }
            /// Centroid
            if (dataFlags & (data::Centroid | data::GradientScore | data::Symmetry)) {
                cv::Moments m = cv::moments(contour, false);
                centroid = cv::Point(int(m.m10/m.m00), int(m.m01/m.m00));

                dataContainer[i]->setValue(data::Centroid, centroid);
            }
            /// Convex Area
            if (dataFlags & (data::ConvexArea | data::Solidity)) {
                std::vector<cv::Point> hull;
                cv::convexHull(contour, hull);
                convexArea = cv::contourArea(hull, false);

                dataContainer[i]->setValue(data::ConvexArea, convexArea);
            }
            /// Major Axis
            if (dataFlags & (data::Major_axis | data::Minor_axis | data::Eccentricity | data::Symmetry)) {
                cv::RotatedRect fit = cv::fitEllipse(contour);
                majorAxis = cv::max(fit.size.width,fit.size.height);
                minorAxis = cv::min(fit.size.width,fit.size.height);

                dataContainer[i]->setValue(data::Major_axis, majorAxis);
                dataContainer[i]->setValue(data::Minor_axis, minorAxis);
            }
            /// Perimeter
            if (dataFlags & (data::Perimeter | data::Circularity)) {
                perimeter = cv::arcLength(contour, true);

                dataContainer[i]->setValue(data::Perimeter, perimeter);
            }
            /// Bounding Box
            if (dataFlags & data::BoundingBox) {
                boundingBox = cv::boundingRect(contour);

                dataContainer[i]->setValue(data::BoundingBox, boundingBox);
            }
            /// Circularity
            if (dataFlags & data::Circularity) {
                // Fits a circle and compare theoretical areas of the circle and the actual area
                // As the resolution of the actual object increases, so does this function
                cv::Point2f center;
                float radius;
                cv::minEnclosingCircle(contour, center, radius);
                cv::Mat temp0 = cv::Mat::zeros(img.rows, img.cols, CV_8U);
                cv::circle(temp0, center, (int) radius, cv::Scalar(255), 1);

                circularity = area / (pow((double) radius, 2) * PI);

                dataContainer[i]->setValue(data::Circularity, circularity);
            }
            /// Eccentricity
            if (dataFlags & data::Eccentricity) {
                eccentricity = minorAxis / majorAxis;

                dataContainer[i]->setValue(data::Eccentricity, eccentricity);
            }
            /// Gradient Score
            if (dataFlags & data::GradientScore) {
                if ((round(centroid.x - 5.5) < 0 && round(centroid.x + 5.5) > img.cols) &&
                    (round(centroid.y - 22.5) < 0 && round(centroid.y + 22.5) > img.rows)) {

                    cv::Rect roi = cv::Rect((int) round(centroid.x - 5.5), (int) round(centroid.x + 5.5),
                                            (int) round(centroid.y - 22.5), (int) round(centroid.y + 22.5));

                    cv::Mat temp = cv::Mat(img, roi);
                    cv::Canny(temp, temp, 0.2, 0.7);    // arbitrery threshold (picture should be binary at this point)
                    gradientScore = cv::sum(temp(cv::Range(6, 7), cv::Range::all()))[0];

                    dataContainer[i]->setValue(data::GradientScore, gradientScore);
                }
            }
            /// Solidity
            if (dataFlags & data::Solidity) {
                solidity = area / convexArea;

                dataContainer[i]->setValue(data::Solidity, solidity);
            }
            /// Symmetry
            if(dataFlags & data::Symmetry) {
                if ((round(centroid.x - 5.5) < 0 && round(centroid.x + 5.5) > img.cols) &&
                    (round(centroid.y - 22.5) < 0 && round(centroid.y + 22.5) > img.rows)) {

                    cv::Rect roi = cv::Rect((int) round(centroid.x - 4.5),(int) round(centroid.x + 4.5),
                                            (int) round(centroid.y - 22.5),(int) round(centroid.y + 22.5));

                    cv::Mat temp = cv::Mat(img,roi);
                    double im_min;
                    double im_max;
                    cv::minMaxIdx(temp, &im_min, &im_max);
                    temp -= im_min / (im_max - im_min);
                    cv::Mat flipped;
                    cv::flip(temp,flipped,1);
                    cv::Mat d1;
                    cv::absdiff(temp, flipped, d1);
                    symmetry = cv::sum(d1)[0] / majorAxis;
                }

                dataContainer[i]->setValue(data::Symmetry, symmetry);
            }
            i++;
        }
        return i;
    } // regionProps

} // namespace matlab

#endif //CELLSORTER_MATLAB_EXT_H
