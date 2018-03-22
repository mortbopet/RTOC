#ifndef CELLSORTER_MATLAB_EXT_H
#define CELLSORTER_MATLAB_EXT_H

#include <opencv/cv.hpp>
#include <cmath>
#include <iostream>

#include "datacontainer.h"

namespace matlab {
    enum regionPropTypes {
        Area = data::Area,
        BoundingBox = data::BoundingBox,
        Centroid = data::Centroid,
        Circularity = data::Circularity,
        ConvexArea = data::ConvexArea,
        Eccentricity = data::Eccentricity,
        GradientScore = data::GradientScore,
        Major_axis = data::Major_axis,
        Minor_axis = data::Minor_axis,
        Solidity = data::Solidity,
        Symmetry = data::Symmetry,
        Perimeter = data::Perimeter,
        PixelIdxList = data::PixelIdxList
    };

    void bwareaopen(cv::Mat &im, double size);

    int regionProps(const cv::Mat& img, const int& dataFlags, DataContainer& dc);

} // namespace matlab

#endif //CELLSORTER_MATLAB_EXT_H
