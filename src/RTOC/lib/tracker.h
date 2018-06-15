#ifndef RTOC_TRACKER_H
#define RTOC_TRACKER_H

struct Tracker {
    cv::Point centroid;
    int cell_no;
    int frame_no;
    bool found;

    Tracker() {
        centroid = cv::Point(0,0);
        cell_no = 0;
        frame_no = 0;
        found = false;
    }
    Tracker(int c) {
        centroid = cv::Point(0,0);
        cell_no = 0;
        frame_no = c;
        found = false;
    }

    bool operator==(Tracker& rhs) { return frame_no == rhs.frame_no; }
};

#endif  // RTOC_TRACKER_H
