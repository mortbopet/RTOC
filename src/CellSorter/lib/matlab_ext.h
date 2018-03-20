#ifndef CELLSORTER_MATLAB_EXT_H
#define CELLSORTER_MATLAB_EXT_H

#include <opencv/cv.hpp>

#include "datacontainer.h"

namespace matlab {
    void bwareaopen(cv::Mat &im, double size) {
        // Only accept CV_8UC1
        if (im.channels() != 1 || im.type() != CV_8U) {
            return;
        }

        // Find all contours
        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(im.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < contours.size(); i++) {
            // Calculate contour area
            double area = cv::contourArea(contours[i]);

            // Remove small objects by drawing the contour with black color
            if (area <= size) {
                cv::drawContours(im, contours, i, CV_RGB(0, 0, 0), -1);
            }
        }
    }

    std::pair<double,double> findCannyThreshold(const cv::Mat& img) {
        double percentOfPixelsNotEdges = 0.7;
        double thresholdRatio = 0.4;
        double low = percentOfPixelsNotEdges * img.rows * img.cols;
        // Initialize variables used by calcHist
        cv::Mat hist;
        int bins = 64;
        float range[] = {0, 256};
        const float* ranges = range;
        cv::calcHist(&img, 1, nullptr, cv::Mat(), hist, 1, &bins, &ranges);

        // Loop through histogram until threshold is met
        double highThreshold = 0;
        int c = 0;
        while(highThreshold < low) {
            highThreshold += hist.at<float>(0, c);
            c++;
        }
        std::pair<double,double> threshold;
        threshold.first = highThreshold / 64; //  * 16320;
        threshold.second = threshold.first * thresholdRatio;

        return threshold;
    }

    void regionProps(const cv::Mat& img, const int& dataFlags, DataObject& data) {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        // for every contour -> new regionprops object with m_parameters
        for (const std::vector<cv::Point>& contour : contours) {
            int area = 0;
            cv::Rect boundingBox;
            cv::Point centroid;
            double circularity = 0.0;
            int convexArea = 0;
            double eccentricity = 0.0;
            double gradientScore = 0.0;
            double majorAxis = 0.0;
            double minorAxis = 0.0;
            double solidity = 0.0;
            double symmetry = 0.0;
            double perimeter = 0.0;

            /// Area
            if (dataFlags & (data::Area | data::Circularity | data::Solidity)) {
                area = (int) contour.size();

                data.setValue(data::Area, area);
            }
            /// Centroid
            if (dataFlags & (data::Centroid | data::GradientScore | data::Symmetry)) {
                cv::Moments m = cv::moments(contour, true);
                centroid = cv::Point(int(m.m10/m.m00),int(m.m01/m.m00));

                data.setValue(data::Centroid, centroid);
            }
            /// Convex Area
            if (dataFlags & (data::ConvexArea | data::Solidity)) {
                std::vector<int> hull;
                cv::convexHull(contour, hull, false, true);
                convexArea = (int) hull.size();

                data.setValue(data::ConvexArea, convexArea);
            }
            /// Major Axis
            if (dataFlags & (data::Major_axis | data::Minor_axis | data::Eccentricity | data::Symmetry)) {
                cv::RotatedRect fit = cv::fitEllipse(contour);
                majorAxis = cv::max(fit.size.width,fit.size.height);
                minorAxis = cv::min(fit.size.width,fit.size.height);

                data.setValue(data::Major_axis, majorAxis);
                data.setValue(data::Minor_axis, minorAxis);
            }
            /// Perimeter
            if (dataFlags & (data::Perimeter | data::Circularity)) {
                perimeter = cv::arcLength(contour, true);

                data.setValue(data::Perimeter, perimeter);
            }
            /// Bounding Box
            if (dataFlags & data::BoundingBox) {
                boundingBox = cv::boundingRect(contour);

                data.setValue(data::BoundingBox, boundingBox);
            }
            /// Circularity
            if (dataFlags & data::Circularity) {
                circularity = 4 * M_PI * area / perimeter;

                data.setValue(data::Circularity, circularity);
            }
            /// Eccentricity
            if (dataFlags & data::Eccentricity) {
                eccentricity = (majorAxis * majorAxis) - (minorAxis * minorAxis);

                data.setValue(data::Eccentricity, eccentricity);
            }
            /// Gradient Score
            if (dataFlags & data::GradientScore) {
                if ((round(centroid.x - 5.5) < 0 && round(centroid.x + 5.5) > img.cols) &&
                    (round(centroid.y - 22.5) < 0 && round(centroid.y + 22.5) > img.rows)) {

                    cv::Rect roi = cv::Rect((int) round(centroid.x - 5.5),(int) round(centroid.x + 5.5),
                                            (int) round(centroid.y - 22.5),(int) round(centroid.y + 22.5));

                    cv::Mat temp = cv::Mat(img,roi);
                    cv::Canny(temp, temp, 0.20, 0.6);
                    gradientScore = cv::sum(temp(cv::Range(6,7),cv::Range::all()))[0];
                }

                data.setValue(data::GradientScore, gradientScore);
            }
            /// Solidity
            if (dataFlags & data::Solidity) {
                solidity = (double) area / (double) convexArea;

                data.setValue(data::Solidity, solidity);
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

                data.setValue(data::Symmetry, symmetry);
            }
        }
    } // end regionProps

} // namespace matlab

#endif //CELLSORTER_MATLAB_EXT_H
