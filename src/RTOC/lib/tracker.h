#ifndef RTOC_TRACKER_H
#define RTOC_TRACKER_H

struct Tracker {
    cv::Point centroid{};
    int cell_no;
    int frame_no;

    Tracker() {
        centroid = cv::Point(0,0);
        cell_no = 0;
        frame_no = 0;
    }
    explicit Tracker(int c) {
        centroid = cv::Point(0,0);
        cell_no = 0;
        frame_no = c;
    }
    bool operator==(Tracker& rhs) { return frame_no == rhs.frame_no; }
};

#endif  // RTOC_TRACKER_H
