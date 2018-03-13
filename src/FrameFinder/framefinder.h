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

/**
 * Function name should explain enough
 * takes address of folder path and the address for the results
 *
 * @param   files   :   path to folder
 * @param   folder  :   vector<string> with file names
 * @return          :   Files found
 */
int files_from_folder(std::vector<std::string>& files, const std::string& folder) {
    try {
        boost::filesystem::path p(folder);
        boost::filesystem::directory_iterator start(p);
        boost::filesystem::directory_iterator end;
        transform(start, end, back_inserter(files), path_leaf_string());
    } catch (...) {
        return -1;
    }
    return (int)files.size();
}

/**
 * Gets string between two delimiters (assuming only one instance of last delimiter)
 * @param src   :   string to search in
 * @param first :   first delimiter
 * @param last  :   last delimiter
 * @return      :   extracted std::string
 */
std::string extractBetween(const std::string& src, const std::string& first,
                           const std::string& last) {
    unsigned long a = src.find(first);
    unsigned long b = src.find(last);
    return src.substr(a + 1, b - a);
}

/**
 * Extension to get_files(), but sorts the output
 *
 * @param files     :   vector<string>  :   Container for found filenames
 * @param folder    :   string          :   Path to folder
 * @param mode      :   FILE_LIST_MODE  :   Set output mode
 * @return          :   int             :   error code
 */
int get_files(std::vector<Frame>& files, const std::string& folder) {
    std::vector<std::string> file_paths;
    // Get size and return if empty or error
    int count = files_from_folder(file_paths, folder);
    if (count <= 0) {
        return -1;
    }
    // Allocate vector
    std::vector<Frame> fn((unsigned)count);
    // Get file-numbers and accept_or_reject
    for (int i = 0; i < count; i++) {
        auto id = (int)strtol(extractBetween(file_paths[i], "_", ".").c_str(), nullptr, 10);
        Frame f = {cv::Mat(), file_paths[i], id, false};
        fn[i] = f;
    }

    sort(fn.begin(), fn.end());

    files = fn;

    return 0;
}

/**
 * Accept_or_reject
 *
 * @param frames        :   vector<string>  :   full list of images
 * @param img_folder    :   string          :   path to image folder
 * @param path_acc      :   string          :   path to "accepted" txt-file
 * @param path_dis      :   string          :   path to "discarded" txt-file
 *
 */
void accept_or_reject(std::vector<Frame>& frames, const std::string& img_folder,
                      const double& threshold) {
    double crit = 0.0;

    cv::Mat lastMoved = cv::imread(img_folder + "/" + frames[0].filename, cv::IMREAD_GRAYSCALE);
    ;

    // Loop through all pictures
    for (auto& frame : frames) {
        std::string img_path = img_folder + "/" + frame.filename;
        frame.image = cv::imread(img_path, cv::IMREAD_GRAYSCALE);
        minMaxIdx(lastMoved - frame.image, nullptr, &crit);
        crit /= 255;

        if (crit <= threshold) {
            // Discard
            frame.accepted = false;
        } else {
            // Accept
            frame.accepted = true;
            lastMoved = frame.image;
        }
    }
}

void get_accepted(const std::vector<Frame>& frames, std::vector<Frame>& output) {
    for (const Frame& f : frames) {
        if (f.accepted) {
            output.emplace_back(f);
        }
    }
}

void get_rejected(const std::vector<Frame>& frames, std::vector<Frame>& output) {
    for (const Frame& f : frames) {
        if (!f.accepted) {
            output.push_back(f);
        }
    }
}

#endif  // FRAMEFINDER_FFHELPERS_H
