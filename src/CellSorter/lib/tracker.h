#ifndef CELLSORTER_TRACKER_H
#define CELLSORTER_TRACKER_H

struct Tracker {
    cv::Point centroid;
    int cell_no;
    int frame_no;

    Tracker() = default;
    Tracker(int c) {
        cell_no = 0;
        frame_no = c;
    }
    bool operator==(Tracker& rhs) { return frame_no == rhs.frame_no; }
};

#endif  // CELLSORTER_TRACKER_H
