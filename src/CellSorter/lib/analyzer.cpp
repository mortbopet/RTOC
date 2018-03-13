#include "analyzer.h"

void analyzer::loadRBCPreset() {
    m_processes.push_back(new AbsoluteDiff);
    m_processes.push_back(new Normalize);
    m_processes.push_back(new Binarize);

    // Doing morph open and close
    auto morphDEFAULT = new Morph();
    morphDEFAULT->m_morphType.setValue(cv::MORPH_OPEN);
    morphDEFAULT->m_morphValueX.setValue(2);
    morphDEFAULT->m_morphValueY.setValue(2);
    m_processes.push_back(morphDEFAULT);

    morphDEFAULT = new Morph();
    morphDEFAULT->m_morphType.setValue(cv::MORPH_CLOSE);
    morphDEFAULT->m_morphValueX.setValue(15);
    morphDEFAULT->m_morphValueY.setValue(15);
    m_processes.push_back(morphDEFAULT);
}

void analyzer::loadPatientPreset(std::string img, std::string txt) {
    m_Experiment.defaultSettings(img, txt);
}

void analyzer::loadImageNames() {
    std::ifstream inFile;
    std::string temp;
    inFile.open(m_Experiment.imagePath + "_Discarded.txt");  // Gets name of accepted files
    while (std::getline(inFile, temp)) {
        m_Experiment.dis.push_back(temp);
    }
    inFile.close();
    inFile.open(m_Experiment.imagePath + "_Accepted.txt");  // Gets name of accepted files
    while (std::getline(inFile, temp)) {
        m_Experiment.acc.push_back(temp);
    }
    inFile.close();
}
void analyzer::selectBG() {
    std::string imgPath;
    imgPath = m_Experiment.imagePath + m_Experiment.dis[0];  // Loads very first discarded picture
    m_bg = cv::imread(imgPath, cv::IMREAD_GRAYSCALE);  // Sets as background
}

void analyzer::runProcesses() {
    for(const auto& process : m_processes){
        process->doProcessing(m_img, m_bg, m_Experiment);
    }
    resetProcesses();
}

void analyzer::resetProcesses() {
    m_processes.clear();
}

void analyzer::showImg() {
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", m_img);
    cv::waitKey(1);
}
