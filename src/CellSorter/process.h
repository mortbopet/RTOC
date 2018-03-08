#ifndef CELLSORTER_PROCESS_H
#define CELLSORTER_PROCESS_H

#include <opencv/cv.hpp>

class process {

public:


//private:
    std::vector<float> returnValues;  // vector of returnvalues
    std::vector<std::string> list;    // vector of commands
    cv:: Mat diff; // Declartion of image objects used throughout processing
};

class ImageProcess : public process {
    void subtractBackground(std::string path, cv::Mat bg, cv::Mat im, Properties setup) {
        cv::Mat diff, bg_edge, temp, absdiff;

        im = cv::imread(path, cv::IMREAD_GRAYSCALE);  //, cv::IMREAD_GRAYSCALE); // Loads image

        cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
        cv::imshow("Display window", im);
        // cv::waitKey(0);

        cv::absdiff(im, bg, diff);  // Outputs absolute difference into 'diff'
        cv::imshow("Display window", diff);
        // cv::waitKey(0);

        cv::normalize(diff, absdiff, 8192, 0);  // 3rd arguemnt controls range of normalization output.
        cv::imshow("Display window", absdiff);
        // cv::waitKey(0);

        cv::threshold(diff, diff, setup.edge_thres, 255, cv::THRESH_BINARY);
        cv::imshow("Display window", diff);
        // cv::waitKey(0);
        cv::morphologyEx(diff, diff, cv::MORPH_OPEN, setup.se_noiseremoval);
        cv::morphologyEx(diff, diff, cv::MORPH_CLOSE, setup.se_RBC);
        cv::imshow("Display window", diff);
        cv::waitKey(0);

        // BACKGROUND MANIPULATION
        cv::threshold(bg, bg_edge, setup.edge_thres * 255, 255, cv::THRESH_BINARY_INV);
        cv::imshow("Display window", bg_edge);
        // cv::waitKey(0);

        cv::morphologyEx(bg_edge, bg_edge, cv::MORPH_CLOSE, setup.se_edge);
        cv::imshow("Display window", bg_edge);
        // cv::waitKey(0);

        const int xLength = 120;  // Half of horizontal length

        for (int i = 0; i < xLength; i++) {  //
            for (int j = 0; j < 27; j++) {
                bg_edge.at<float>(i, setup.inlet + j - 10) = 0;
            }
        }
        cv::imshow("Display window", bg_edge);
        // cv::waitKey(0);

        cv::morphologyEx(bg_edge, bg_edge, cv::MORPH_OPEN, 100);
        cv::imshow("Display window", bg_edge);
        // cv::waitKey(0);

        diff = diff.mul(bg_edge);
        cv::imshow("Display window", diff);
        // cv::waitKey(0);
    }
};

class DataProcess : public process {};

#endif  // CELLSORTER_PROCESS_H