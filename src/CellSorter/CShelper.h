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

    struct Properties {
        int inlet; // Bottom corner of constriction inlet
        int outlet; // Bottom corner of constriction outlet
        int yref; // Vertical center of inlet
        int cellNum; // Used for cell registration
        //int tracker[] = {}; // Used for cell registration
        float edge_thres; // Threshold for extracting channel edge
        cv::Mat se_edge; // SE for extracting channel edge (SE = structuring element).
        cv::Mat se_RBC; // SE for extracting channel edge
        cv::Mat se_noiseremoval; // SE for removing noise

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

    void subtractBackground (std::string path, cv::Mat bg, cv::Mat im, Properties setup) {
        cv::Mat diff, bg_edge, temp, absdiff;

        im = cv::imread(path, cv::IMREAD_GRAYSCALE);//, cv::IMREAD_GRAYSCALE); // Loads image

        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
        cv::imshow("Display window", im);
        //cv::waitKey(0);

        cv::absdiff(im, bg, diff); // Outputs absolute difference into 'diff'
        cv::imshow("Display window", diff);
        //cv::waitKey(0);

        cv::normalize(diff, absdiff, 8192, 0); // 3rd arguemnt controls range of normalization output.
        cv::imshow("Display window", absdiff);
        //cv::waitKey(0);

        cv::threshold(diff, diff, setup.edge_thres, 255, cv::THRESH_BINARY);
        cv::imshow("Display window", diff);
        //cv::waitKey(0);
        cv::morphologyEx(diff,diff, cv::MORPH_OPEN, setup.se_noiseremoval);
        cv::morphologyEx(diff, diff, cv::MORPH_CLOSE, setup.se_RBC);
        cv::imshow("Display window", diff);
        cv::waitKey(0);




        // BACKGROUND MANIPULATION
        cv::threshold(bg, bg_edge, setup.edge_thres*255, 255, cv::THRESH_BINARY_INV);
        cv::imshow("Display window", bg_edge);
        //cv::waitKey(0);

        cv::morphologyEx(bg_edge, bg_edge, cv::MORPH_CLOSE, setup.se_edge);
        cv::imshow("Display window", bg_edge);
        //cv::waitKey(0);

        const int xLength = 120; // Half of horizontal length

        for (int i = 0; i < xLength; i++) { //
            for (int j = 0; j < 27; j++) {
                bg_edge.at<float>(i, setup.inlet+j-10) = 0;
            }
        }
        cv::imshow("Display window", bg_edge);
        //cv::waitKey(0);

        cv::morphologyEx(bg_edge,bg_edge, cv::MORPH_OPEN,100);
        cv::imshow("Display window", bg_edge);
        //cv::waitKey(0);

        diff = diff.mul(bg_edge);
        cv::imshow("Display window", diff);
        //cv::waitKey(0);







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
