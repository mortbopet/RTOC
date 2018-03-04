//
// Created by Jens Lindahl on 03/03/2018.
//

#ifndef FRAMEFINDER_FFHELPERS_H
#define FRAMEFINDER_FFHELPERS_H

#include <opencv/cv.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

using namespace std;
using namespace cv;


namespace FF {

    /**
     *  Helper for extracting path leaf from
     *  boost filesystem path
     */
    struct path_leaf_string
    {
        string operator()(const boost::filesystem::directory_entry& entry) const
        {
            return entry.path().leaf().string();
        }
    };

    /**
     *  struct to handle filename with corresponding number
     *  makes it possible to sort after number instead of after string
     */
    struct file_with_id
    {
        string filename;
        int num;

        bool operator < ( const file_with_id& rhs ) const { return num < rhs.num; }
    };

    /**
     * File list mode enum
     *
     *  0   :   Load full path to vector<string>
     *  1   :   Load only file-bame to vector<string>
     *
     *  More options?
     */
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
    string extractBetween(const string& src, const string& first, const string& last) {
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
    int get_files(const string& folder, vector<string>& v) {
        try {
            boost::filesystem::path p(folder);
            boost::filesystem::directory_iterator start(p);
            boost::filesystem::directory_iterator end;
            transform(start, end, back_inserter(v), path_leaf_string());
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
    int get_files_sorted(vector<string>& files, const string& folder, const int mode) {
        int count = get_files(folder, files);
        if (count < 0) {
            return -1;
        }

        vector<file_with_id> fn(count);
        
        for (int i = 0; i < count; i++) {
            fn[i].filename = files[i];
            fn[i].num = (int) strtol(extractBetween(files[i],"_",".").c_str(),nullptr,10);
        }
        sort(fn.begin(),fn.end());

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
    void print_files(vector<string>& v) {
        for (vector<string>::const_iterator i = v.begin(); i != v.end(); ++i)
            cout << *i << endl;
    }


    /**
     *
     * DESCRIPTION HERE
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
    void accept_or_reject(const vector<string>& images,
                          const string& img_folder,
                          const string& path_acc,
                          const string& path_dis) {

        // Initialize streams
        fstream fs_acc;
        fstream fs_dis;

        // Open files for output | append
        fs_acc.open(path_acc, fstream::out | fstream::app);
        fs_dis.open(path_dis, fstream::out | fstream::app);

        unsigned long l = images.size();    // Images in total
        int nd = 3;                         // neighbour distance
        double lim1 = 0.0354;               // intensity threshold
        vector<double> critbank(l,0);  // intensity array

        // Initialize image arrays
        Mat img_buf1;
        Mat img_buf2;

        // Initialize path placeholders
        string img_path1;
        string img_path2;

        // Loop through all pictures
        for (int i = 0; i < l; i++) {
            if (i - nd > 0) {
                // Set full path
                img_path1 = img_folder + '/' + images[i];
                img_path2 = img_folder + '/' + images[i-nd];
                // Read images (grayscale)
                img_buf1 = imread(img_path1, IMREAD_GRAYSCALE);
                img_buf2 = imread(img_path2, IMREAD_GRAYSCALE);
                // Subtract images
                img_buf1 -= img_buf2;
                // Get max/intensity
                minMaxIdx(img_buf1, nullptr, &critbank[i]);
                // Normalize intensity
                critbank[i] /= 255;
            }
            if (i > 10) {
                if (critbank[i] <= lim1 && critbank[i-nd] <= lim1) {
                    // Write to declined
                    fs_dis << img_path1 << endl;
                } else {
                    // Write to accepted
                    fs_acc << img_path1 << endl;
                }
            } else {
                // Write to declined
                fs_dis << img_path1 << endl;
            }
        }

        // Close fstreams
        fs_acc.close();
        fs_dis.close();
    }
}

#endif //FRAMEFINDER_FFHELPERS_H
