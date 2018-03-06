//
// Created by Eskild Børsting Sørensen on 06/03/2018.
//

#ifndef CELLSORTER_CSHELPER_H
#define CELLSORTER_CSHELPER_H

#include <opencv2/core.hpp>

using namespace cv;

namespace CS {
    // Functions for file loading
    struct RBC {
        std::string m_label;
        Point_<int> m_centroid;
        Rect_<int> m_boundingBox;
        int m_frameNo;
        int m_circularity;
        int m_eccentricity;
        int m_mAxisLength;
        int m_symmetry;
        int m_gradient;
    };

    void selectBackground () {
        /* 1) Select all frames discarded before given image
         * 2) Select all frames discarded after given image
         * 3) Take the closest of those images and choose the one closest to the image on both sides
         * 4) Take the average of those backgrounds as a gray image
         */
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
