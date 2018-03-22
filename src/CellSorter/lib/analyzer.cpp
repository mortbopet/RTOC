#include "analyzer.h"
#include "framefinder.h"
#include "machinelearning.h"

void analyzer::loadRBCPreset() {
    // Subtract background
    auto subtractbgDEFAULT = new SubtractBG();
    subtractbgDEFAULT->m_edgeThreshold.setValue(0.272);
    m_processes.push_back(new SubtractBG);

    // Normalize
    auto normalizeDEFAULT = new Normalize();
    normalizeDEFAULT->m_normalizeStrength.setValue(0xfff);
    // m_processes.push_back(normalizeDEFAULT);

    // Binarize
    auto binarizeDEFAULT = new Binarize();
    binarizeDEFAULT->m_maxVal.setValue(255);
    binarizeDEFAULT->m_edgeThreshold.setValue(50);
    // m_processes.push_back(binarizeDEFAULT);

    // Morph open (imopen)
    auto morphDEFAULT = new Morph();
    morphDEFAULT->m_morphType.setValue(cv::MORPH_OPEN);
    morphDEFAULT->m_morphValueX.setValue(3);
    morphDEFAULT->m_morphValueY.setValue(3);
    // m_processes.push_back(morphDEFAULT);

    // Morph close (imclose)
    morphDEFAULT = new Morph();
    morphDEFAULT->m_morphType.setValue(cv::MORPH_CLOSE);
    morphDEFAULT->m_morphValueX.setValue(15);
    morphDEFAULT->m_morphValueY.setValue(10);
    // m_processes.push_back(morphDEFAULT);

    // Floodfill
    // m_processes.push_back(new Fill);

    // Clearborder
    // m_processes.push_back(new ClearBorder);
}

void analyzer::loadExperimentPreset(const std::string& img) {
    m_Experiment.defaultSettings(img);
}

void analyzer::loadImageNames() {
    std::vector<Frame> frames;
    std::string img_folder = m_Experiment.imagePath;
    get_files(frames, img_folder);
    accept_or_reject(frames, img_folder, m_Experiment.intensity_threshold);
    get_accepted(frames, m_Experiment.acc);
    get_rejected(frames, m_Experiment.dis);
}
void analyzer::selectBG() {
    m_bg = m_Experiment.dis[10].image;  // Sets as background
}

void analyzer::runProcesses() {
    for (const auto& process : m_processes) {
        process->doProcessing(m_img, m_bg, m_Experiment);
    }
}

void analyzer::resetProcesses() {
    m_processes.clear();
}

void analyzer::showImg() {
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", m_img);
    cv::waitKey(0);
}
