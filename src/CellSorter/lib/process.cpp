#include "process.h"
#include "matlab_ext.h"
#include <math.h>
#include <opencv/cv.hpp>
#include "datacontainer.h"

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
    m_dataFlags.setRange(0, 0xffff);
}

void RegionProps::doProcessing(cv::Mat &img, cv::Mat &, const Experiment &props) const {
    int dataFlags = m_dataFlags.getValue();

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    // for every contour -> new regionprops object with m_parameters
    for (const std::vector<cv::Point>& contour : contours) {
        int area;
        cv::Point centroid;
        int convexArea;
        double majorAxis;
        double minorAxis;
        double perimeter;

        /// Area
        if (dataFlags & (data::Area | data::Circularity | data::Solidity)) {
            area = (int) contour.size();
        }
        /// Centroid
        if (dataFlags & (data::Centroid | data::GradientScore | data::Symmetry)) {
            cv::Moments m = cv::moments(contour, true);
            centroid = cv::Point(int(m.m10/m.m00),int(m.m01/m.m00));
        }
        /// Convex Area
        if (dataFlags & (data::ConvexArea | data::Solidity)) {
            std::vector<int> hull;
            cv::convexHull(contour, hull, false, true);
            convexArea = (int) hull.size();
        }
        /// Major Axis
        if (dataFlags & (data::Major_axis | data::Minor_axis | data::Eccentricity | data::Symmetry)) {
            cv::RotatedRect fit = cv::fitEllipse(contour);
            majorAxis = max(fit.size.width,fit.size.height);
            minorAxis = min(fit.size.width,fit.size.height);
        }
        /// Perimeter
        if (dataFlags & (data::Perimeter | data::Circularity)) {
            perimeter = cv::arcLength(contour, true);
        }
        /// Bounding Box
        if (dataFlags & data::BoundingBox) {
            cv::Rect boundingBox = cv::boundingRect(contour);
        }
        /// Circularity
        if (dataFlags & data::Circularity) {
            double circularity = 4 * M_PI * area / perimeter;
        }
        /// Eccentricity
        if (dataFlags & data::Eccentricity) {
            double eccentricity = (majorAxis * majorAxis) - (minorAxis * minorAxis);
            // return eccentricity
        }
        /// Gradient Score
        if (dataFlags & data::GradientScore) {
            double gradientScore;
            if ((round(centroid.x - 5.5) < 0 && round(centroid.x + 5.5) > img.cols) &&
                (round(centroid.y - 22.5) < 0 && round(centroid.y + 22.5) > img.rows)) {

                cv::Rect roi = cv::Rect((int) round(centroid.x - 5.5),(int) round(centroid.x + 5.5),
                                        (int) round(centroid.y - 22.5),(int) round(centroid.y + 22.5));

                cv::Mat temp = cv::Mat(img,roi);
                cv::Canny(temp, temp, 0.20, 0.6);
                gradientScore = cv::sum(cv::Mat(temp,cv::Rect(0,6,temp.cols,2)))[0];
            } else {
                gradientScore = 0;
            }
        }
        /// Solidity
        if (dataFlags & data::Solidity) {
            double solidity = (double) area / (double) convexArea;
        }
        /// Symmetry
        if(dataFlags & data::Symmetry) {
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
                symmetry = cv::sum(d1)[0] / majorAxis;
            } else {
                symmetry = 0;
            }
        }
    }
}
