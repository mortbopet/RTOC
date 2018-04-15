#ifndef FRAMEFINDER_FFHELPERS_H
#define FRAMEFINDER_FFHELPERS_H

#include <sys/stat.h>
#include <sys/types.h>
#include <boost/filesystem.hpp>
#include <fstream>
#include <opencv/cv.hpp>
#include <vector>
#include <QFileInfoList>

struct Frame {
    cv::Mat image;
    std::string filename;
    int id;
    bool accepted;

    bool operator<(const Frame& rhs) const { return id < rhs.id; }
};

struct Frame_Q {
    cv::Mat image;
    QFileInfo fileinfo;
    int id;

    bool operator<(const Frame_Q& rhs) const { return id < rhs.id; }
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

bool exists(const std::string& path);
int files_from_folder(std::vector<std::string>& files, const std::string& folder);
std::string extractBetween(const std::string& src, const std::string& first,
                           const std::string& last);
std::string extractBetween(const std::string& src);
int get_files(std::vector<Frame>& files, const std::string& folder);
void accept_or_reject(std::vector<Frame>& frames, const std::string& img_folder,
                      const double& threshold);
void get_accepted(const std::vector<Frame>& frames, std::vector<Frame>& output);
void get_rejected(const std::vector<Frame>& frames, std::vector<Frame>& output);

void sort_qfilelist(QFileInfoList& qfil, const std::string& del1, const std::string& del2);

#endif  // FRAMEFINDER_FFHELPERS_H
