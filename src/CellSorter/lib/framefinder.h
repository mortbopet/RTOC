#ifndef FRAMEFINDER_FFHELPERS_H
#define FRAMEFINDER_FFHELPERS_H

#include <boost/filesystem.hpp>
#include <fstream>
#include <opencv/cv.hpp>
#include <vector>

struct Frame {
    cv::Mat image;
    std::string filename;
    int id;
    bool accepted;

    bool operator<(const Frame& rhs) const { return id < rhs.id; }
};

/**
 *  Helper for extracting path leaf from
 *  boost filesystem path
 */
struct path_leaf_string {
    std::string operator()(const boost::filesystem::directory_entry& entry) const {
        return entry.path().leaf().string();
    }
};

int files_from_folder(std::vector<std::string>& files, const std::string& folder);
std::string extractBetween(const std::string& src, const std::string& first,
                           const std::string& last);
int get_files(std::vector<Frame>& files, const std::string& folder);
void accept_or_reject(std::vector<Frame>& frames, const std::string& img_folder,
                      const double& threshold);
void get_accepted(const std::vector<Frame>& frames, std::vector<Frame>& output);
void get_rejected(const std::vector<Frame>& frames, std::vector<Frame>& output);

#endif  // FRAMEFINDER_FFHELPERS_H
