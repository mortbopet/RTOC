#include <string>
#include "../lib/framefinder.h"
#include "../lib/prettyprint.h"

int test_framefinder() {
    std::string default_testdir = "./";

    PP::header("FrameFinder Test");

    /// Filedetector test
    if (!exists("./Makefile")) {
        throw std::string("Error in exists(const std::string& path)");
    }
    if (!exists("./CMakeFiles/")) {
        throw std::string("Error in exists(const std::string& path)");
    }
    std::cout << "exists() passed" << std::endl;

    /// Filelist getter test
    // Test files_from_folder from build dir
    std::vector<std::string> files;
    if (files_from_folder(files, default_testdir) == 0) {
        throw std::string("Error using files_from_folder()");
    }
    std::cout << "file_from_folder() passed" << std::endl;

    /// String extractor test
    // Test 1
    std::string a = "some_test.here";
    if (extractBetween(a, "_", ".") != "test") {
        throw std::string("Error in extractBetween()");
    }
    if (extractBetween(a) != "test") {
        throw std::string("Error in extractBetween()");
    }
    std::cout << "extractBetween() passed" << std::endl;

    /// Test get_files method
    // Test 1
    std::vector<Frame> f0;
    if (get_files(f0, default_testdir) != 0) {
        throw std::string("Error finding files");
    }

    // Clear vector and test if proper empty
    f0.clear();
    if (!f0.empty()) {
        throw std::string("Error clearing std::vector<Frame>");
    }

    // Test 2
    std::vector<Frame> f0_acc;
    std::vector<Frame> f0_dis;

    // Load vector with empty frames
    for (int i = 0; i < 10; i++) {
        f0.push_back({cv::Mat(), "./", i, (i > 3)});
    }
    if (f0.size() != 10) {
        throw std::string("Wrong size of Frame-vector");
    }
    std::cout << "std::vector<Frame> passed" << std::endl;

    get_accepted(f0, f0_acc);
    get_rejected(f0, f0_dis);
    if (f0_acc.size() != 6) {
        throw std::string("Wrong size of accepted frames");
    }
    std::cout << "get_accepted() passed" << std::endl;
    if (f0_dis.size() != 4) {
        throw std::string("Wrong size of discarded frames");
    }
    std::cout << "get_rejected() passed" << std::endl;

    std::cout << " FrameFinder passed" << std::endl;
    return 0;
}
