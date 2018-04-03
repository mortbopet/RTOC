#include "matlab_ext.h"


namespace matlab {
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

    int regionProps(const cv::Mat &img, const int &dataFlags, DataContainer &dc) {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        double area = 0.0;
        cv::Rect boundingBox;
        cv::Point centroid;
        double circularity = 0.0;
        double convexArea = 0.0;
        double eccentricity = 0.0;
        double gradientScore = 0.0;
        double majorAxis = 0.0;
        double minorAxis = 0.0;
        double solidity = 0.0;
        double symmetry = 0.0;
        double perimeter = 0.0;

        int i = 0; // dataContainer index
        // for every contour -> new regionprops object with m_parameters
        for (const std::vector<cv::Point> &contour : contours) {
            dc.appendNew();

            /// Area
            if (dataFlags & (data::Area | data::Circularity | data::Solidity)) {
                // Compute the area of the contour (not exact pixel count!)
                area = cv::contourArea(contour, false);

                if (dataFlags & data::Area) dc[i]->setValue(data::Area, area);
            }
            /// Centroid
            if (dataFlags & (data::Centroid | data::GradientScore | data::Symmetry)) {
                cv::Moments m = cv::moments(contour, false);
                centroid = cv::Point(int(m.m10 / m.m00), int(m.m01 / m.m00));

                if (dataFlags & data::Centroid) dc[i]->setValue(data::Centroid, centroid);
            }
            /// Convex Area
            if (dataFlags & (data::ConvexArea | data::Solidity)) {
                std::vector<cv::Point> hull;
                cv::convexHull(contour, hull);
                convexArea = cv::contourArea(hull, false);

                if (dataFlags & data::ConvexArea) dc[i]->setValue(data::ConvexArea, convexArea);
            }
            /// Major Axis
            if (dataFlags & (data::Major_axis | data::Minor_axis | data::Eccentricity | data::Symmetry)) {
                if (contour.size() > 5) {
                    cv::RotatedRect fit = cv::fitEllipse(contour);
                    majorAxis = cv::max(fit.size.width, fit.size.height);
                    minorAxis = cv::min(fit.size.width, fit.size.height);

                    if (dataFlags & data::Major_axis) dc[i]->setValue(data::Major_axis, majorAxis);
                    if (dataFlags & data::Minor_axis) dc[i]->setValue(data::Minor_axis, minorAxis);

                }
            }
            /// Perimeter
            if (dataFlags & (data::Perimeter | data::Circularity)) {
                perimeter = cv::arcLength(contour, true);

                if (dataFlags & data::Perimeter) dc[i]->setValue(data::Perimeter, perimeter);
            }
            /// Bounding Box
            if (dataFlags & data::BoundingBox) {
                boundingBox = cv::boundingRect(contour);

                dc[i]->setValue(data::BoundingBox, boundingBox);
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

                circularity = area / (pow((double) radius, 2) * M_PI);

                dc[i]->setValue(data::Circularity, circularity);
            }
            /// Eccentricity
            if (dataFlags & data::Eccentricity) {
                eccentricity = minorAxis / majorAxis;

                dc[i]->setValue(data::Eccentricity, eccentricity);
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

                    dc[i]->setValue(data::GradientScore, gradientScore);
                }
            }
            /// Solidity
            if (dataFlags & data::Solidity) {
                solidity = area / convexArea;

                dc[i]->setValue(data::Solidity, solidity);
            }
            /// Symmetry
            if (dataFlags & data::Symmetry) {
                if ((round(centroid.x - 5.5) < 0 && round(centroid.x + 5.5) > img.cols) &&
                    (round(centroid.y - 22.5) < 0 && round(centroid.y + 22.5) > img.rows)) {

                    cv::Rect roi = cv::Rect((int) round(centroid.x - 4.5), (int) round(centroid.x + 4.5),
                                            (int) round(centroid.y - 22.5), (int) round(centroid.y + 22.5));

                    cv::Mat temp = cv::Mat(img, roi);
                    double im_min;
                    double im_max;
                    cv::minMaxIdx(temp, &im_min, &im_max);
                    temp -= im_min / (im_max - im_min);
                    cv::Mat flipped;
                    cv::flip(temp, flipped, 1);
                    cv::Mat d1;
                    cv::absdiff(temp, flipped, d1);
                    symmetry = cv::sum(d1)[0] / majorAxis;
                }

                dc[i]->setValue(data::Symmetry, symmetry);
            }
            /// PixelIdxList
            if (dataFlags & data::PixelIdxList) {
                cv::Mat filledImage(img.rows, img.cols, CV_8U);
                cv::fillConvexPoly(filledImage, contour, cv::Scalar(255));
                std::vector<cv::Point> c_0;
                cv::findNonZero(filledImage, c_0);
                // Create new PixelIdxList and pass to datacontainer
                // We assume that datacontainer will delete the object
                auto contour_out = new std::vector<cv::Point>;
                *contour_out = c_0;
                dc[i]->setValue(data::PixelIdxList, contour_out);
            }
            i++;
        }
        return i;
    } // regionProps

    int removePixels(cv::Mat img, std::vector<cv::Point>* points) {
        int count = 0;
        for (const cv::Point& p : *points) {
            img.at<uchar>(p) = 0;
            count++;
        }
        return count;
    }
}