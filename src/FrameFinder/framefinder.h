//
// Created by Jens Lindahl on 03/03/2018.
//

#ifndef FRAMEFINDER_FFHELPERS_H
#define FRAMEFINDER_FFHELPERS_H

#include <boost/filesystem.hpp>
#include <fstream>
#include <opencv/cv.hpp>

namespace framefinder {

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
 *  struct to handle filename with corresponding number
 *  makes it possible to sort after number instead of after string
 */
struct file_with_id {
    std::string filename;
    int num;

    bool operator<(const file_with_id& rhs) const { return num < rhs.num; }
};

/**
 * File list mode enum
 *
 *  0   :   Load full path to vector<string>
 *  1   :   Load only file-bame to vector<string>
 *
 *  More options?
 */
enum FILE_LIST_MODE { FF_FULL_PATH, FF_ONLY_FILE };

/**
 * Gets string between two delimiters (assuming only one instance of last delimiter)
 * @param src   :   string to search in
 * @param first :   first delimiter
 * @param last  :   last delimiter
 * @return      :   extracted std::string
 */
std::string extractBetween(const std::string& src, const std::string& first, const std::string& last) {
    unsigned long a = src.find(first);
    unsigned long b = src.find(last);
    return src.substr(a + 1, b - a);
}

/**
 * Function name should explain enough
 * takes address of folder path and the address for the results
 *
 * @param   folder  :   path to folder
 * @param   files   :   vector<string> with file names
 * @return          :   Files found
 */
int get_files(const std::string& folder, std::vector<std::string>& v) {
    try {
        boost::filesystem::path p(folder);
        boost::filesystem::directory_iterator start(p);
        boost::filesystem::directory_iterator end;
        transform(start, end, back_inserter(v), path_leaf_string());
    } catch (...) {
        return -1;
    }
    return (int)v.size();
}
/**
 * Extension to get_files(), but sorts the output
 *
 * @param folder
 * @param files
 */
int get_files_sorted(std::vector<std::string>& files, const std::string& folder, const int mode) {
    // Get size and return if negative
    int count = get_files(folder, files);
    if (count < 0) {
        return -1;
    }
    // Allocate vector
    std::vector<file_with_id> fn((unsigned)count);

    // Get file-numbers and sort
    for (int i = 0; i < count; i++) {
        fn[i].filename = files[i];
        fn[i].num = (int)strtol(extractBetween(files[i], "_", ".").c_str(), nullptr, 10);
    }
    sort(fn.begin(), fn.end());

    // Insert sorted file-names
    for (int i = 0; i < count; i++) {
        if (mode == FF_FULL_PATH) {
            files[i] = folder + "/" + fn[i].filename;
        } else if (mode == FF_ONLY_FILE) {
            files[i] = fn[i].filename;
        } else {
            // Return error finish code
            return -1;
        }
    }
    return 0;
}

/**
 *  For debugging, print contents of stringvec
 */
void print_files(std::vector<std::string>& v) {
    for (std::vector<std::string>::const_iterator i = v.begin(); i != v.end(); ++i)
        std::cout << *i << std::endl;
}

/**
 * Accept_or_reject
 *
 *
 *
 * @param images        :   vector<string>  :   full list of images
 * @param img_folder    :   string          :   path to image folder
 * @param path_acc      :   string          :   path to "accepted" txt-file
 * @param path_dis      :   string          :   path to "discarded" txt-file
 *
 * TODO:
 *  It should be able to easily change <nd> and <lim1> parameters!
 */
void accept_or_reject(const std::vector<std::string>& images, const std::string& img_folder,
                      const std::string& path_acc, const std::string& path_dis) {
    // Initialize streams
    std::fstream fs_acc;
    std::fstream fs_dis;

    // Open files for output | append
    fs_acc.open(path_acc, std::fstream::out | std::fstream::app);
    fs_dis.open(path_dis, std::fstream::out | std::fstream::app);

    unsigned long l = images.size();  // Images in total
    double lim1 = 0.0354;             // intensity threshold

    double crit = 0.0;

    // Initialize path placeholders
    std::string img_path;
    std::string img_path2;

    cv::Mat lastMoved;
    cv::Mat nextFrame;

    // Read first image (grayscale)
    img_path = img_folder + '/' + images[0];
    lastMoved = cv::imread(img_path, cv::IMREAD_GRAYSCALE);

    // Loop through all pictures
    for (int i = 1; i < l; i++) {
        // Set full path
        img_path = img_folder + '/' + images[i];

        nextFrame = cv::imread(img_path, cv::IMREAD_GRAYSCALE);
        minMaxIdx(lastMoved - nextFrame, nullptr, &crit);
        crit /= 255;

        if (crit <= lim1) {
            fs_dis << img_path << std::endl;
        } else {
            fs_acc << img_path << std::endl;
            lastMoved = nextFrame;
        }
    }
    // Close fstreams
    fs_acc.close();
    fs_dis.close();
}

}  // namespace framefinder

#endif  // FRAMEFINDER_FFHELPERS_H
