#include "analyzer.h"

void Analyzer::loadRBCPreset() {

    // Subtract background
    auto subtractbgDEFAULT = new SubtractBG();
    subtractbgDEFAULT->m_edgeThreshold.setValue(0.272);
    m_processes.push_back(subtractbgDEFAULT);

    // Normalize
    auto normalizeDEFAULT = new Normalize();
    normalizeDEFAULT->m_normalizeStrength.setValue(0xfff);
    m_processes.push_back(normalizeDEFAULT);

    // Binarize
    auto binarizeDEFAULT = new Binarize();
    binarizeDEFAULT->m_maxVal.setValue(255);
    binarizeDEFAULT->m_edgeThreshold.setValue(50);
    m_processes.push_back(binarizeDEFAULT);

    // Morph open (imopen)
    auto morphDEFAULT = new Morph();
    morphDEFAULT->m_morphType.setValue(cv::MORPH_OPEN);
    morphDEFAULT->m_morphValueX.setValue(3);
    morphDEFAULT->m_morphValueY.setValue(3);
    m_processes.push_back(morphDEFAULT);

    // Morph close (imclose)
    morphDEFAULT = new Morph();
    morphDEFAULT->m_morphType.setValue(cv::MORPH_CLOSE);
    morphDEFAULT->m_morphValueX.setValue(15);
    morphDEFAULT->m_morphValueY.setValue(10);
    m_processes.push_back(morphDEFAULT);

    // Floodfill
    m_processes.push_back(new FloodFill);

    // Clearborder
    m_processes.push_back(new ClearBorder);

    // bwpropfilt ConvexArea [200 1450]
    auto propFilt0 = new PropFilter();
    propFilt0->m_regionPropsTypes.setValue(matlab::regionPropTypes::ConvexArea);
    propFilt0->m_lowerLimit.setValue(200);
    propFilt0->m_upperLimit.setValue(1450);
    m_processes.push_back(propFilt0);

    // bwpropfilt MajorAxisLength [0 65]
    auto propFilt1 = new PropFilter();
    propFilt1->m_regionPropsTypes.setValue(matlab::regionPropTypes::Major_axis);
    propFilt1->m_lowerLimit.setValue(0);
    propFilt1->m_upperLimit.setValue(65);
    m_processes.push_back(propFilt1);
}

void Analyzer::loadExperimentPreset(const std::string& img_path) {
    m_Experiment.defaultSettings(img_path);
}

void Analyzer::loadImageNames() {
    std::vector<Frame> frames;
    std::string img_folder = m_Experiment.imagePath;
    get_files(frames, img_folder);
    accept_or_reject(frames, img_folder, m_Experiment.intensity_threshold);
    get_accepted(frames, m_Experiment.acc);
    get_rejected(frames, m_Experiment.dis);
}
void Analyzer::selectBG() {
    m_bg = m_Experiment.dis[10].image;  // Sets as background
}

void Analyzer::runProcesses() {
    for (const auto& process : m_processes) {
        process->doProcessing(m_img, m_bg, m_Experiment);
    }
}

void Analyzer::resetProcesses() {
    m_processes.clear();
}

void Analyzer::showImg(const int& delay) {
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", m_img);
    cv::waitKey(delay);
}

void Analyzer::showImg(const cv::Mat& img, const int& delay) {
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", img);
    cv::waitKey(delay);
}
