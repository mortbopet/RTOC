//
// Created by Jens Lindahl on 03/03/2018.
//

#ifndef FRAMEFINDER_FFHELPERS_H
#define FRAMEFINDER_FFHELPERS_H

#include <opencv/cv.hpp>
#include <boost/filesystem.hpp>
#include <fstream>


namespace FF {

    /**
     *  Helper for extracting path leaf from
     *  boost filesystem path
     */
    struct path_leaf_string
    {
        std::string operator()(const boost::filesystem::directory_entry& entry) const
        {
            return entry.path().leaf().string();
        }
    };

    /**
     *  struct to handle filename with corresponding number
     *  makes it possible to sort after number instead of after string
     */
    struct file_num
    {
        std::string filename;
        int num;

        bool operator < ( const file_num& rhs ) const { return num < rhs.num; }
    };

    enum FILE_LIST_MODE {
        FF_FULL_PATH,
        FF_ONLY_FILE
    };


    /**
     * Gets string between two delimiters (assuming only one instance of last delimiter)
     * @param src   :   string to search in
     * @param first :   first delimiter
     * @param last  :   last delimiter
     * @return      :   extracted string
     */
    std::string extractBetween(const std::string& src, const std::string& first, const std::string& last) {
        unsigned long a = src.find(first);
        unsigned long b = src.find(last);
        return src.substr(a+1, b - a);

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
            std::transform(start, end, std::back_inserter(v), path_leaf_string());
        } catch (...) {
            return -1;
        }
        return (int) v.size();
    }
    /**
     * Extension to get_files(), but sorts the output
     *
     * @param folder
     * @param files
     */
    int get_files_sorted(const std::string& folder, std::vector<std::string>& files, const int mode) {
        std::vector<file_num> fn;

        int count = get_files(folder, files);

        if (count < 0) {
            return -1;
        }

        for (int i = 0; i < count; i++) {
            fn.push_back(file_num());
            fn[i].filename = files[i];
            fn[i].num = (int) std::strtol(extractBetween(files[i],"_",".").c_str(),nullptr,10);
        }
        std::sort(fn.begin(),fn.end());

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

    cv::Mat imfix(const cv::Mat& img1, const cv::Mat& img2) {
        return img1 - img2;
    }

    void accept_or_reject(std::vector<std::string>& images, const std::string& img_folder, const std::string& path_acc, const std::string& path_dis) {
        std::fstream fs_acc;
        std::fstream fs_dis;

        // Open files for output | append
        fs_acc.open(path_acc, std::fstream::out | std::fstream::app);
        fs_dis.open(path_dis, std::fstream::out | std::fstream::app);

        unsigned long l = images.size();    // Images in total
        int nd = 3;                         // neighbour distance
        double lim1 = 0.0354;               // intensity threshold
        std::vector<double> critbank(l,0);  // intensity array

        // Initialize image arrays
        cv::Mat img_buf1;
        cv::Mat img_buf2;

        // Initialize path placeholders
        std::string img_path1;
        std::string img_path2;

        for (int i = 0; i < l; i++) {
            if (i - nd > 0) {
                // Set full path
                img_path1 = img_folder + '/' + images[i];
                img_path2 = img_folder + '/' + images[i-nd];
                // Read images (grayscale)
                img_buf1 = cv::imread(img_path1, cv::IMREAD_GRAYSCALE);
                img_buf2 = cv::imread(img_path2, cv::IMREAD_GRAYSCALE);
                // Subtract images
                img_buf1 -= img_buf2;
                // Get max/intensity
                cv::minMaxIdx(img_buf1, nullptr, &critbank[i]);
                // Normalize intensity
                critbank[i] /= 255;
            }
            if (i > 10) {
                if (critbank[i] <= lim1 && critbank[i-nd] <= lim1) {
                    // Write to declined
                    fs_dis << img_path1 << std::endl;

                } else {
                    // Write to accepted
                    fs_acc << img_path1 << std::endl;
                }
            }
        }
        // Close files/fstreams
        fs_acc.close();
        fs_dis.close();
    }
}

#endif //FRAMEFINDER_FFHELPERS_H
