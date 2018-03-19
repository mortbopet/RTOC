#include "process.h"
#include "matlab_ext.h"
#include <math.h>
#include <opencv/cv.hpp>

Process::Process(void) {}

void Process::doProcessing(cv::Mat& img, cv::Mat& bg, const Experiment& props) const {}

Morph::Morph() {
    m_morphType.setOptions(map<cv::MorphTypes, string>{{cv::MorphTypes::MORPH_CLOSE, "Closing"},
                                                       {cv::MorphTypes::MORPH_OPEN, "Opening"}});
    m_morphValueX.setRange(0, 100);
    m_morphValueY.setRange(0, 100);
}

void Morph::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    if ((m_morphType.getValue() == cv::MORPH_CLOSE) ||
        (m_morphType.getValue() == cv::MORPH_OPEN)) {  // if-else statement for choosing correct
                                                       // structuring element for operation
        cv::morphologyEx(
            img, img, m_morphType.getValue(),
            cv::getStructuringElement(
                cv::MORPH_ELLIPSE, cv::Size(m_morphValueX.getValue(), m_morphValueY.getValue())));
    }
}

Binarize::Binarize() {
    m_maxVal.setRange(0, 255);
    m_edgeThreshold.setRange(0, 255);
}

void Binarize::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    cv::threshold(img, img, m_edgeThreshold.getValue(), m_maxVal.getValue(), cv::THRESH_BINARY);
}

Normalize::Normalize() {
    m_normalizeStrength.setRange(0, 0xffff);
}

void Normalize::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    cv::normalize(img, img, m_normalizeStrength.getValue(), 0);
}

SubtractBG::SubtractBG() {
    m_edgeThreshold.setRange(0,1);
}

void SubtractBG::doProcessing(cv::Mat& img, cv::Mat& bg, const Experiment& props) const {
    cv::Mat bg_edge, diff;
    cv::absdiff(img, bg, diff);
    cv::threshold(bg, bg_edge, 69, 255, cv::THRESH_BINARY_INV);
    cv::Mat se_edge = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 30));
    cv::morphologyEx(bg_edge,bg_edge,cv::MORPH_CLOSE,se_edge);
    cv::rectangle(bg_edge,cv::Rect(props.inlet-10,0,20,bg.cols),cv::Scalar(0),cv::FILLED);
    //matlab::bwareaopen(bg_edge,100);
    cv::bitwise_not(bg_edge,bg_edge);
    // cv::bitwise_and(diff,bg_edge,img);

    img = bg_edge;

}

void Canny::doProcessing(cv::Mat &img, cv::Mat &, const Experiment &props) const {
    cv::Canny(img, img, m_lowThreshold.getValue(), m_highThreshold.getValue());
}

void ClearBorder::doProcessing(cv::Mat &img, cv::Mat &, const Experiment &props) const {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::Rect bounding_rect = cv::boundingRect(contours[i]);
        cv::Rect test_rect = bounding_rect & cv::Rect(1, 1, img.cols - 2, img.rows - 2);
        if (bounding_rect != test_rect)
        {
            cv::drawContours(img, contours, (int)i, cv::Scalar(0),-1);
        }
    }
}

void Fill::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    // Assert that the image has been binarized
    cv::Mat img_inv;
    cv::bitwise_not(img,img_inv);
    cv::floodFill(img_inv, cv::Point(0,0), cv::Scalar(0));
    cv::bitwise_or(img, img_inv, img);
}

RegionProps::RegionProps() {

}

/** PROTO / DEMO / NOT IMPLEMENTED / WARNING / HEJ / !!!
    Naive written:
    Very redudant, how to make this very super smart(?)
 */
void RegionProps::doProcessing(cv::Mat &img, cv::Mat &, const Experiment &props) const {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // for every contour -> new regionprops object with m_parameters
    for (const std::vector<cv::Point>& contour : contours) {
        if ("Area") {
            double area = cv::contourArea(contour);
            // return area
        }
        if ("BoundingBox") {
            cv::Rect bbox = cv::boundingRect(contour);
            // return bbox
        }
        if ("Centroid") {
            cv::Point centroid; // cv::POint
            cv::Moments m = cv::moments(contour, true);
            centroid.x = int(m.m10/m.m00);
            centroid.y = int(m.m01/m.m00);
            // return centroid
        }
        if ("Circularity") {
            auto area = cv::contourArea(contour);
            auto perimeter = cv::arcLength(contour, true);
            auto circ = 4 * M_PI * area / perimeter;
            // return circ
        }
        if ("Eccentricity") {
            cv::RotatedRect fit = cv::fitEllipse(contour);
            double majoraxis = max(fit.size.width,fit.size.height);
            double minoraxis = min(fit.size.width,fit.size.height);
            double eccentricity = (majoraxis * majoraxis) - (minoraxis * minoraxis);
            // return eccentricity
        }
        if ("GradientScore") {
            cv::Point centroid; // cv::POint
            cv::Moments m = cv::moments(contour, true);
            centroid.x = int(m.m10/m.m00);
            centroid.y = int(m.m01/m.m00);
            double gradient_score;
            if ((round(centroid.x - 5.5) < 0 && round(centroid.x + 5.5) > img.cols) &&
                (round(centroid.y - 22.5) < 0 && round(centroid.y + 22.5) > img.rows)) {

                cv::Rect roi = cv::Rect((int) round(centroid.x - 5.5),(int) round(centroid.x + 5.5),
                                        (int) round(centroid.y - 22.5),(int) round(centroid.y + 22.5));

                cv::Mat temp = cv::Mat(img,roi);
                cv::Canny(temp, temp, 0.20, 0.6);
                gradient_score = cv::sum(cv::Mat(temp,cv::Rect(0,6,temp.cols,2)))[0];
            } else {
                gradient_score = 0;
            }
            // return gradient_score
        }
        if ("MajorAxisLength") {
            cv::RotatedRect fit = cv::fitEllipse(contour);
            double majoraxis = max(fit.size.width,fit.size.height);
            // return majoraxis
        }
        if ("MinorAxisLength") {
            cv::RotatedRect fit = cv::fitEllipse(contour);
            double minoraxis = min(fit.size.width,fit.size.height);
            // return majoraxis
        }
        if ("Solidity") {
            auto area = cv::contourArea(contour);
            cv::Mat hull;
            cv::convexHull(img,hull);
            auto hull_area = cv::contourArea(hull);
            double solidity = area / hull_area;
            // return solidity;
        }
        if("Symmetry") {
            cv::RotatedRect fit = cv::fitEllipse(contour);
            double majoraxis = max(fit.size.width,fit.size.height);
            cv::Point centroid; // cv::POint
            cv::Moments m = cv::moments(contour, true);
            centroid.x = int(m.m10/m.m00);
            centroid.y = int(m.m01/m.m00);
            double symmetry;
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
                symmetry = cv::sum(d1)[0] / majoraxis;

            } else {
                symmetry = 0;
            }
            // return symmetry
        }
        if ("Perimeter") {
            double perimeter = cv::arcLength(contour, true);
            // return perimeter
        }
    }
}
