#include "process.h"

process::process(void) {}

void process::doProcessing(cv::Mat& img, cv::Mat& bg, patient props) {}

void erosion::doProcessing(cv::Mat& img, cv::Mat& bg, patient props) {
    cv::morphologyEx(img, img, cv::MORPH_OPEN, props.se_noiseremoval);
}

void erosion::setBorderType(cv::BorderTypes type) {}

void dilation::doProcessing(cv::Mat& img, cv::Mat& bg, patient props) {
    cv::morphologyEx(img, img, cv::MORPH_CLOSE, props.se_RBC);
}

void binarize::doProcessing(cv::Mat& img, cv::Mat& Null, patient props) {
    cv::threshold(img, img, props.edge_thres, 255, cv::THRESH_BINARY);
}

void normalize::doProcessing(cv::Mat& img, cv::Mat& bg, patient props) {
    cv::normalize(img, img, 255, 0);  // 3rd arguemnt controls range of normalization output.
}

void absoluteDiff::doProcessing(cv::Mat& img, cv::Mat& bg, patient props) {
    cv::absdiff(img, bg, img);  // Outputs absolute difference into 'diff'
}