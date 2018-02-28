#ifndef RBC_H
#define RBC_H

#include <opencv2/core.hpp>

using namespace cv;

class RBC {
private:
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

#endif
