#include "framefinder.h"

#include <QRegularExpression>

bool framefinder::exists(const std::string& path) {
    struct stat buf;
    return stat(path.c_str(), &buf) == 0;
}

/**
 * @warning DEPRECATED - Qt functionality is used instead
 *
 * Function name should explain enough
 * takes address of folder path and the address for the results
 *
 * @param   files   :   path to folder
 * @param   folder  :   vector<string> with file names
 * @return          :   Files found
 */
int framefinder::files_from_folder(std::vector<std::string>& files, const std::string& folder) {
    if (!exists(folder)) {
        return -1;
    }
    try {
        boost::filesystem::path p(folder);
        boost::filesystem::directory_iterator start(p);
        boost::filesystem::directory_iterator end;
        transform(start, end, back_inserter(files), path_leaf_string());
    } catch (...) {
        throw std::string("Error reading from file");
    }
    return (int)files.size();
}

/**
 * Gets string between two delimiters (assuming only one instance of last delimiter)
 * @param src   :   string to search in
 * @param first :   first delimiter (default = "_")
 * @param last  :   last delimiter (default = ".")
 * @return      :   extracted std::string
 */
std::string framefinder::extractBetween(const std::string& src, const std::string& first,
                                        const std::string& last) {
    long a = src.rfind(first);
    if (a < 0)
        return "NOT_FOUND";
    a += first.length();
    long b = src.find(last, a);
    if (b < 0)
        return "NOT_FOUND";
    return src.substr(a, b - a);
}
/**
 * Extension to get_files(), but sorts the output
 *
 * @param files     :   vector<string>  :   Container for found filenames
 * @param folder    :   string          :   Path to folder
 * @param mode      :   FILE_LIST_MODE  :   Set output mode
 * @return          :   int             :   error code
 */
int framefinder::get_files(std::vector<Frame>& files, const std::string& folder) {
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
        auto id = (int)strtol(extractBetween(file_paths[i]).c_str(), nullptr, 10);
        Frame f = {cv::Mat(), file_paths[i], id, false};
        fn[i] = f;
    }

    sort(fn.begin(), fn.end());

    files = fn;

    return 0;
}

/**
 * @brief hasChanged determines whether img2 is different from img1. Given a movement threshold the
 * two images a
 * subtracted from eachother and the largest pixel value are found. _Movement_ is then detected if
 * the max-pixel value
 * is larger than the given threshold.
 * @param img1          :   cv::Mat()   :   old image
 * @param img2          :   cv::Mat()   :   new image
 * @param threshold     :   int         :   movement threshold
 * @return              :   bool        :   true if movement, otherwise false
 */
bool framefinder::hasChanged(const cv::Mat& img1, const cv::Mat& img2, const int& threshold) {
    double crit = 0.0;
    cv::minMaxIdx(img1 - img2, nullptr, &crit);
    return crit > threshold;
}

/**
 * Accept_or_reject
 *
 * @param frames        :   vector<string>  :   full list of images
 * @param img_folder    :   string          :   path to image folder
 * @param path_acc      :   string          :   path to "accepted" txt-file
 * @param path_dis      :   string          :   path to "discarded" txt-file
 */
void framefinder::accept_or_reject(std::vector<Frame>& frames, const std::string& img_folder,
                                   const double& threshold) {
    double crit = 0.0;

    cv::Mat lastMoved = cv::imread(img_folder + "/" + frames[0].filename, cv::IMREAD_GRAYSCALE);
    ;

    // Loop through all pictures
    for (auto& frame : frames) {
        std::string img_path = img_folder + "/" + frame.filename;
        frame.image = cv::imread(img_path, cv::IMREAD_GRAYSCALE);
        cv::minMaxIdx(lastMoved - frame.image, nullptr, &crit);

        int _crit = crit;
        _crit >>= 8;  // crit /= 256

        if (_crit <= threshold) {
            // Discard
            frame.accepted = false;
        } else {
            // Accept
            frame.accepted = true;
            lastMoved = frame.image;
        }
    }
}

void framefinder::get_accepted(const std::vector<Frame>& frames, std::vector<Frame>& output) {
    for (const Frame& f : frames) {
        if (f.accepted) {
            output.emplace_back(f);
        }
    }
}

void framefinder::get_rejected(const std::vector<Frame>& frames, std::vector<Frame>& output) {
    for (const Frame& f : frames) {
        if (!f.accepted) {
            output.push_back(f);
        }
    }
}

/**
 * @brief sorts a qfileinfolist from number found between the two specified delimiters
 * Mening that from the file names found in qfileinfolist there should be an integer id
 * number formatted as following:
 * /filename[del1][id number][del2].[ext]
 * @example
 * filename of qfile: /photo_1342.png
 * where del1 = "_" and del2 ".", the file would get id 1342. The list is then sorted accordingly to
 * those id's
 *
 * @param qfil  :   QFileInfoList&  :   list to be sorted
 * @param del1  :   QString&        :   first delimiter
 * @param del2  :   QString&        :   last delimiter
 */

void framefinder::sort_qfilelist(QFileInfoList& qfil, const std::string& del1,
                                 const std::string& del2) {
    // Allocate vector
    auto n = qfil.size();
    std::vector<Frame_Q> fn(n);

    // Get file-numbers and accept_or_reject
    for (int i = 0; i < n; i++) {
        auto id = (int)strtol(framefinder::extractBetween(qfil[i].fileName().toStdString()).c_str(),
                              nullptr, 10);
        Frame_Q fq = {qfil[i], id};
        fn[i] = fq;
    }
    // Sort
    sort(fn.begin(), fn.end());

    // Reinsert all QFileInfo obejcts in qfil
    for (int i = 0; i < n; i++) {
        qfil[i] = fn[i].fileinfo;
    }
}
