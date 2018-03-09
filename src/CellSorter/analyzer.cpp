#include "analyzer.h"

void analyzer::loadRBCPreset() {
    m_processes.push_back(new absoluteDiff);
    m_processes.push_back(new normalize);
    m_processes.push_back(new binarize);
    m_processes.push_back(new erosion);
    m_processes.push_back(new dilation);
}

void analyzer::loadPatientPreset(std::string img, std::string txt) {
    patient.defaultSettings(img, txt);
}

void analyzer::loadImageNames() {
    std::ifstream inFile;
    std::string temp;
    inFile.open(patient.imagePath + "_Discarded.txt");  // Gets name of accepted files
    while (std::getline(inFile, temp)) {
        patient.dis.push_back(temp);
    }
    inFile.close();
    inFile.open(patient.imagePath + "_Accepted.txt");  // Gets name of accepted files
    while (std::getline(inFile, temp)) {
        patient.acc.push_back(temp);
    }
    inFile.close();
}
void analyzer::selectBG() {
    std::string imgPath;
    imgPath = patient.imagePath + patient.dis[0];      // Loads very first discarded picture
    m_bg = cv::imread(imgPath, cv::IMREAD_GRAYSCALE);  // Sets as background
}

void analyzer::runProcesses() {
    for (int i = 0; i < (int)m_processes.size(); i++) {
        m_processes[i]->doProcessing(m_img, m_bg, patient);
    }
    m_processes.clear();
}

void analyzer::showImg() {
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", m_img);
    cv::waitKey(0);
}