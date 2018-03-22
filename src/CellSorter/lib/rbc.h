#ifndef RBC_H
#define RBC_H

#include <opencv/cv.hpp>

class RBC {
public:
    RBC(); // Constructor

    std::string m_label;
    cv::Point_<int> m_centroid;
    cv::Rect_<int> m_boundingBox;
    int m_frameNo;
    int m_cellID;
    int m_type; // Used for specifying adult or foster
    int m_circularity;
    int m_eccentricity;
    int m_mAxisLength;
    int m_symmetry;
    int m_gradient;
private:

};

#endif
