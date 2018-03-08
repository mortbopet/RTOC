#ifndef CELLSORTER_CSHELPER_H
#define CELLSORTER_CSHELPER_H
#include <fstream>
#include <iostream>
#include "process.h"

#include <opencv/cv.hpp>

class analyzer {
public:
    void runProcesses();
    enum class IMG_PROCESS_TYPES { PRESET, SUB_BG, ABS_DIFF, MAKE_BIN, RM_NOISE, FILL_CELL };

private:

    process list; // List of processes to pass

    cv::Mat m_img;

    void makeList(std::vector<IMG_PROCESS_TYPES>);

    void setupRBCPreset();
};

struct Properties {
    int inlet;    // Bottom corner of constriction inlet
    int outlet;   // Bottom corner of constriction outlet
    int yref;     // Vertical center of inlet
    int cellNum;  // Used for cell registration
    // int tracker[] = {}; // Used for cell registration
    float edge_thres;         // Threshold for extracting channel edge
    cv::Mat se_edge;          // SE for extracting channel edge (SE = structuring element).
    cv::Mat se_RBC;           // SE for extracting channel edge
    cv::Mat se_noiseremoval;  // SE for removing noise
    cv::Mat bg, im;           // Background and current image
    std::vector<std::string> acceptedImages, discardedImages;
    std::string imagePath, textPath;  // Holds information about path to images and to _Accepted.txt and _Discarded.txt

    // struct RBC {
    //    std::string m_label;
    //    cv::Point_<int> m_centroid;
    //    cv::Rect_<int> m_boundingBox;
    //    int m_frameNo;
    //    int m_circularity;
    //    int m_eccentricity;
    //    int m_mAxisLength;
    //    int m_symmetry;
    //    int m_gradient;
    //};

    void loadImageNames() {
        std::ifstream inFile;
        std::string temp;
        inFile.open(imagePath + "_Discarded.txt");  // Gets name of accepted files
        while (std::getline(inFile, temp)) {
            discardedImages.push_back(temp);
        }
        inFile.close();
        inFile.open(imagePath + "_Accepted.txt");  // Gets name of accepted files
        while (std::getline(inFile, temp)) {
            acceptedImages.push_back(temp);
        }
        inFile.close();
    }

    cv::Mat selectBackground(std::string mode,
                             cv::Mat bg) {  // string discarded) { // mode is method of determining background
        std::string imgPath;
        if (mode == "SIMPLE") {
            imgPath = imagePath + discardedImages[0];        // Loads very first discarded picture
            bg = cv::imread(imgPath, cv::IMREAD_GRAYSCALE);  // Sets as background
        }
        return bg;
    }
};

#endif  // CELLSORTER_CSHELPER_H