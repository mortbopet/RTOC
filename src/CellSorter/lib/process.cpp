#include "process.h"

void ProcessNameGenerator::add_process(const std::string& name) {
    ProcessBase::get_processes().push_back(name);
}

std::vector<std::string>& ProcessBase::get_processes() {
    static std::vector<std::string> processes;
    return processes;
}
ProcessBase::ProcessBase(void) {}

void ProcessBase::doProcessing(cv::Mat& img, cv::Mat& bg, const Experiment& props) const {}

Morph::Morph() {
    m_morphType.setOptions(map<cv::MorphTypes, string>{{cv::MorphTypes::MORPH_CLOSE, "Closing"},
                                                       {cv::MorphTypes::MORPH_OPEN, "Opening"}});
    m_morphType.setValue(cv::MORPH_CLOSE);

    m_morphValueX.setRange(1, 100);
    m_morphValueX.setValue(1);
    m_morphValueY.setRange(1, 100);
    m_morphValueY.setValue(1);
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
    m_maxVal.setValue(255);
    m_edgeThreshold.setRange(0, 255);
    m_edgeThreshold.setValue(0);
}

void Binarize::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    cv::threshold(img, img, m_edgeThreshold.getValue(), m_maxVal.getValue(), cv::THRESH_BINARY);
}

Normalize::Normalize() {
    m_normalizeStrength.setRange(0, 0xffff);
    m_normalizeStrength.setValue(4096);
}

void Normalize::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    cv::normalize(img, img, m_normalizeStrength.getValue(), 0);
}

SubtractBG::SubtractBG() {
    m_edgeThreshold.setRange(0, 1);
    m_edgeThreshold.setValue(0.272);
}

void SubtractBG::doProcessing(cv::Mat& img, cv::Mat& bg, const Experiment& props) const {
    cv::Mat bg_edge, diff;
    cv::Mat se_edge = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(30, 1));

    // Get difference from actual image and selected background
    cv::absdiff(img, bg, diff);
    // Binarize background
    cv::threshold(bg, bg_edge, m_edgeThreshold.getValue() * 255, 255, cv::THRESH_BINARY_INV);
    // Morphologic close background
    cv::morphologyEx(bg_edge, bg_edge, cv::MORPH_CLOSE, se_edge);
    // Insert black vertical rectangle at (inlet - 10 : inlet + 10)
    cv::rectangle(bg_edge, cv::Rect(props.inlet - 10, 0, 20, bg.cols), cv::Scalar(0), cv::FILLED);
    // Open up (bwareaopen equivalent)
    matlab::bwareaopen(bg_edge, 100);
    // Invert background cut and bitwise 'and' with image
    cv::bitwise_not(bg_edge, bg_edge);
    cv::bitwise_and(diff, bg_edge, img);
}

void Canny::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    cv::Canny(img, img, m_lowThreshold.getValue(), m_highThreshold.getValue());
}

void ClearBorder::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(img, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Rect bounding_rect = cv::boundingRect(contours[i]);
        cv::Rect test_rect = bounding_rect & cv::Rect(1, 1, img.cols - m_borderWidth.getValue(),
                                                      img.rows - m_borderWidth.getValue());
        if (bounding_rect != test_rect) {
            cv::drawContours(img, contours, (int)i, cv::Scalar(0), -1);
        }
    }
}
ClearBorder::ClearBorder() {
    m_borderWidth.setRange(0, 255);
    m_borderWidth.setValue(2);
}

Canny::Canny() {
    m_lowThreshold.setRange(0, 255);
    m_lowThreshold.setValue(0);
    m_highThreshold.setRange(0, 255);
    m_highThreshold.setValue(255);
}
FloodFillProcess::FloodFillProcess() {}

void FloodFillProcess::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    matlab::floodFill(img);
}

PropFilter::PropFilter() {
    m_regionPropsTypes.setOptions(map<matlab::regionPropTypes, string>{
        {matlab::regionPropTypes::Area, "Area"},
        {matlab::regionPropTypes::ConvexArea, "ConvexArea"},
        {matlab::regionPropTypes::Major_axis, "MajorAxisLength"},
        {matlab::regionPropTypes::Minor_axis, "MinorAxisLength"},
        {matlab::regionPropTypes::Solidity, "Solidity"}});
    m_regionPropsTypes.setValue(matlab::regionPropTypes::Area);

    m_lowerLimit.setRange(0, DBL_MAX);
    m_lowerLimit.setValue(0);
    m_upperLimit.setRange(0, DBL_MAX);
    m_upperLimit.setValue(0);
}

void PropFilter::doProcessing(cv::Mat& img, cv::Mat&, const Experiment& props) const {
    int flags = m_regionPropsTypes.getValue();
    double l[2] = {m_lowerLimit.getValue(), m_upperLimit.getValue()};
    DataContainer blobs(0xffffff);

    // Get the number of found connected components and their data
    int count = matlab::regionProps(img, 0xffffff, blobs);
    // Loop through all blobs
    for (int i = 0; i < count; i++) {
        double res = blobs[i]->getValue<double>(static_cast<data::DataFlags>(flags));
        // If criteria met - erase blob
        if (res < l[0] || res > l[1]) {
            std::vector<cv::Point>* vector2;
            vector2 = blobs[i]->getValue<std::vector<cv::Point>*>(data::PixelIdxList);
            matlab::removePixels(img, vector2);
        }
    }
}

// Export all process types for serialization
BOOST_CLASS_EXPORT_GUID(Morph, "Morph")
BOOST_CLASS_EXPORT_GUID(Binarize, "Binarize")
BOOST_CLASS_EXPORT_GUID(SubtractBG, "SubtractBG")
BOOST_CLASS_EXPORT_GUID(Normalize, "Normalize")
BOOST_CLASS_EXPORT_GUID(ClearBorder, "ClearBorder")
BOOST_CLASS_EXPORT_GUID(FloodFillProcess, "FloodFillProcess")
BOOST_CLASS_EXPORT_GUID(PropFilter, "PropFilter")
BOOST_CLASS_EXPORT_GUID(Canny, "Canny")
