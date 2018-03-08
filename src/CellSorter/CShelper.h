//
// Created by Eskild Børsting Sørensen on 06/03/2018.
//

#ifndef CELLSORTER_CSHELPER_H
#define CELLSORTER_CSHELPER_H
#include <iostream>
#include <fstream>

#include <opencv/cv.hpp>


namespace CS {
    // Functions for file loading
    struct RBC {
        std::string m_label;
        cv::Point_<int> m_centroid;
        cv::Rect_<int> m_boundingBox;
        int m_frameNo;
        int m_circularity;
        int m_eccentricity;
        int m_mAxisLength;
        int m_symmetry;
        int m_gradient;
    };

    struct AccDis {
        std::vector<std::string> acceptedImages, discardedImages;
    };

    AccDis loadImageNames(std::string path, AccDis ret) {
         std::ifstream inFile;
        std::string temp;
        inFile.open(path + "_Discarded.txt"); // Gets name of accepted files
        while (std::getline(inFile, temp)) {
            ret.discardedImages.push_back(temp);
        }
        inFile.close();
        inFile.open(path + "_Accepted.txt"); // Gets name of accepted files
        while (std::getline(inFile, temp)) {
            ret.acceptedImages.push_back(temp);
        }
        inFile.close();
        return ret;
    }

    cv::Mat selectBackground (std::string mode, std::string path, AccDis patient, cv::Mat bg) {// string discarded) { // mode is method of determining background
        std::string imgPath;
        if (mode == "SIMPLE") {
            imgPath = path + patient.discardedImages[0]; // Loads very first discarded picture
            bg = cv::imread(imgPath, cv::IMREAD_GRAYSCALE); // Sets as background
        }
        return bg;
    }

    void subtractBackground () {
        /* 1) Get difference between image and background
         * 2) Taking all values of background edge, which are lower than edge_thres
         * 3) Doing morphologically close image on background edge
         * 4) Setting all center pixels to 0
         * 5) Incomplement edges
         * 6) Multiply difference with edges
         * 7) Normalize difference
         */
    }
}


#endif //CELLSORTER_CSHELPER_H
