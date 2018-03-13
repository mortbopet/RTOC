#include <sys/stat.h>
#include <chrono>
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/core/ocl.hpp>

using namespace cv;
using namespace std;

signed long toc(std::chrono::time_point<std::chrono::high_resolution_clock> tic) {
    auto toc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<signed long, std::nano> out = toc - tic;
    return out.count();
}

int main() {
    // Create OpenCL context
    ocl::Context context;
    // Create platform
    context.create(ocl::Device::TYPE_ALL);
    // Init Device
    ocl::Device device;

    // Print number of devices
    cout << endl;
    cout << "-------------------------------------------------------------------" << endl;
    cout << "Number of devices:  " << context.ndevices() << endl;
    // Print info on all devices
    for (int i = 0; i < context.ndevices(); i++) {
        device = context.device(i);
        cout << "-------------------------------------------------------------------" << endl;
        cout << "Device no.:         " << i << endl;
        cout << "Device type:        " << device.type() << endl;
        cout << "Name:               " << device.name() << endl;
        cout << "Available (0/1):    " << device.available() << endl;
        cout << "Max clock (MHz):    " << device.maxClockFrequency() << endl;
        cout << "Max compute units:  " << device.maxComputeUnits() << endl;
        cout << "OpenCL Version:     " << device.OpenCLVersion() << endl;
        cout << "OpenCL C Version:   " << device.OpenCL_C_Version() << endl;
        cout << "ImageSupport (0/1): " << device.imageSupport() << endl;
    }
    cout << "-------------------------------------------------------------------" << endl;
    cout << endl;

    // Init Stuff
    Mat image;
    String image_path = "/home/jenslindahl/CLionProjects/HelloCLCV/clion_logo.png";

    // Init timer
    auto tic = chrono::high_resolution_clock::now();
    // Load Image
    image = imread(image_path, CV_LOAD_IMAGE_COLOR);
    if (image.data) {
        // Show Image
        namedWindow("window name", WINDOW_AUTOSIZE);
        imshow("window name", image);
    } else {
        cout << "error reading file - correct name or wha'?" << endl;
    }

    // Stop timer
    auto t = toc(tic);
    cout << "Ticks elapsed: " << t << endl;
    // Wait for user to quit
    waitKey(0);

    return 0;
}