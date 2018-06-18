#include "mathlab.h"

namespace mathlab {
void bwareaopen(cv::Mat& im, double size) {
    // Only accept CV_8UC
    if (im.channels() != 1 || im.type() != CV_8U) {
        return;
    }

    // Find all contours
    std::vector<std::vector<cv::Point>> contours;
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
 *
 * @param img
 * @param dataFlags
 * @param dc
 * @return
 */
int regionProps(const cv::Mat& img, const unsigned long& dataFlags, DataContainer& dc) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    // Return if zero contours found
    if (contours.empty()) {
        return 0;
    }

    double area = 0.0;
    cv::Rect boundingBox;
    cv::Point centroid;
    double circularity = 0.0;
    double convexArea = 0.0;
    double eccentricity = 0.0;
    double majorAxis = 0.0;
    double minorAxis = 0.0;
    double solidity = 0.0;
    double perimeter = 0.0;

    int i = 0;  // dataContainer index
    // for every contour -> new regionprops object with m_parameters
    for (const std::vector<cv::Point>& contour : contours) {
        dc.appendNew();

        /// Area
        if (dataFlags & (data::Area | data::Circularity | data::Solidity)) {
            // Compute the area of the contour (not exact pixel count!)
            area = cv::contourArea(contour, false);

            if (dataFlags & data::Area)
                dc[i]->setValue(data::Area, area);
        }
        /// Centroid
        if (dataFlags & (data::Centroid | data::Symmetry)) {
            cv::Moments m = cv::moments(contour, false);
            centroid = cv::Point(int(m.m10 / m.m00), int(m.m01 / m.m00));

            if (dataFlags & data::Centroid)
                dc[i]->setValue(data::Centroid, centroid);
        }
        /// Convex Area
        if (dataFlags & (data::ConvexArea | data::Solidity)) {
            std::vector<cv::Point> hull;
            cv::convexHull(contour, hull);
            convexArea = cv::contourArea(hull, false);

            if (dataFlags & data::ConvexArea)
                dc[i]->setValue(data::ConvexArea, convexArea);
        }
        /// Major Axis and Minor Axis
        if (dataFlags &
            (data::Major_axis | data::Minor_axis | data::Eccentricity | data::Symmetry)) {
            if (contour.size() > 5) {
                cv::RotatedRect fit = cv::fitEllipse(contour);
                majorAxis = cv::max(fit.size.width, fit.size.height);
                minorAxis = cv::min(fit.size.width, fit.size.height);

                if (dataFlags & data::Major_axis)
                    dc[i]->setValue(data::Major_axis, majorAxis);
                if (dataFlags & data::Minor_axis)
                    dc[i]->setValue(data::Minor_axis, minorAxis);
            } else {
                if (dataFlags & data::Major_axis)
                    dc[i]->setValue(data::Major_axis, 0.0);
                if (dataFlags & data::Minor_axis)
                    dc[i]->setValue(data::Minor_axis, 0.0);
            }
        }
        /// Perimeter
        if (dataFlags & (data::Perimeter | data::Circularity)) {
            perimeter = cv::arcLength(contour, true);

            if (dataFlags & data::Perimeter)
                dc[i]->setValue(data::Perimeter, perimeter);
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
            cv::circle(temp0, center, (int)radius, cv::Scalar(255), 1);

            circularity = area / (pow((double)radius, 2) * M_PI);

            dc[i]->setValue(data::Circularity, circularity);
        }
        /// Eccentricity
        if (dataFlags & data::Eccentricity) {
            eccentricity = minorAxis / majorAxis;

            dc[i]->setValue(data::Eccentricity, eccentricity);
        }
        /// Solidity
        if (dataFlags & data::Solidity) {
            solidity = area / convexArea;

            dc[i]->setValue(data::Solidity, solidity);
        }
        /// PixelIdxList
        if (dataFlags & data::PixelIdxList) {
            // Init black canvas
            cv::Mat filledImage(img.rows, img.cols, CV_8U, cv::Scalar(0));
            // Reorder pixels from found contour
            std::vector<cv::Point> contour2;
            cv::convexHull(contour,contour2);
            // Paint filled contour
            cv::fillConvexPoly(filledImage, contour2, cv::Scalar(255));
            // Get pixel list
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
}  // regionProps


// -----------------------------
//  Grayscale parameteres
// -----------------------------

/**
 *
 * @param img
 * @param roi
 * @return
 */
double gradientScore(const cv::Mat& img, const cv::Rect& roi) {
    if ((roi.x > 0 && (roi.x + roi.width) < img.cols) && (roi.y > 0 && roi.y + roi.width < img.rows)) {
        // Load image from ROI
        cv::Mat temp;
        img(roi).copyTo(temp);
        // Get edges
        cv::Canny(temp, temp, 0.2, 0.7);
        // Set midrows
        auto midPoint = temp.rows / 2;
        // Check if row range exist
        if (midPoint+1 <= temp.rows && midPoint-1 >= 0) {
            return cv::sum(temp(cv::Range(midPoint-1, midPoint+1), cv::Range::all()))[0];
        }
    }
    return -1;
}

/**
 *
 * @param img
 * @param roi
 * @param majorAxisLength
 * @return
 */
double verticalSymmetry(const cv::Mat& img, const cv::Rect& roi, const double& majorAxisLength) {
    if ((roi.x > 0 && (roi.x + roi.width) < img.cols) && (roi.y > 0 && roi.y + roi.width < img.rows)) {
        // Load image from ROI
        cv::Mat temp;
        img(roi).copyTo(temp);
        // Get min/max values
        double tMin;
        double tMax;
        cv::minMaxIdx(temp, &tMin, &tMax);
        // Recalculate image
        temp = (temp - tMin) / (tMax - tMin);
        // Get difference from fliplr image
        cv::Mat flipped;
        cv::flip(temp,flipped,1);
        cv::absdiff(temp, flipped, temp);
        // Normalize with majorAxisLength
        return (cv::sum(temp)[0] / majorAxisLength);
    }
    return -1;
}


// -----------------------------
//  endof Grayscale parameteres
// -----------------------------


void removePixels(cv::Mat img, std::vector<cv::Point>* points) {
    for (const cv::Point& p : *points) {
        img.at<uchar>(p) = 0;
    }
}
void floodFill(cv::Mat& img) {
    // Assert that the image has been binarized
    cv::Mat img_inv;
    cv::bitwise_not(img, img_inv);
    cv::floodFill(img_inv, cv::Point(0, 0), cv::Scalar(0));
    cv::bitwise_or(img, img_inv, img);
}
void floodFill(cv::Mat& img, const cv::Point& p) {
    // Assert that the image has been binarized
    cv::Mat img_inv;
    cv::bitwise_not(img, img_inv);
    cv::floodFill(img_inv, p, cv::Scalar(0));
    cv::bitwise_or(img, img_inv, img);
}

double dist(const cv::Point& p0, const cv::Point& p1) {
    cv::Point d = p0 - p1;
    return cv::sqrt(d.x * d.x + d.y * d.y);
}

double relativeX(cv::Point &point, mathlab::Line &line) {
    if (line.straight) {
        return point.x - line.x;
    } else {
        double num = line.m * point.x + line.q - point.y;
        double denom = sqrt(line.m*line.m + 1);
        return num/denom;
    }
}

}  // namespace mathlab