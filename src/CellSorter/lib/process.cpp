#include "process.h"
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
    cv::normalize(img, img, m_normalizeStrength.getValue(),
                  0);  // 3rd arguemnt controls range of normalization output.
}

void AbsoluteDiff::doProcessing(cv::Mat& img, cv::Mat& bg, const Experiment& props) const {
    cv::absdiff(img, bg, img);  // Outputs absolute difference into 'diff'
}
