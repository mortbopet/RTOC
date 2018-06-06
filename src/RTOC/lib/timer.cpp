#include "timer.h"
#include <string>

void Timer::tic() {
    // Set tic
    time_point = std::chrono::steady_clock::now();
}

long Timer::toc() {
    // Get toc
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point t0 = time_point;
    std::string string_to_print = "Elapsed time: ";
    long duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    if (duration > 60000000) {
        string_to_print += std::to_string(duration / (double)60000000) + " minutes.";
    } else if (duration > 1000000) {
        string_to_print += std::to_string(duration / (double)1000000) + " seconds.";
    } else if (duration > 1000) {
        string_to_print += std::to_string(duration / (double)1000) + " milliseconds.";
    } else {
        string_to_print += std::to_string(duration) + " microseconds.";
    }
    std::cout << string_to_print << std::endl;
    return duration;
}

double Timer::toc(TIME_MODE time_mode) {
    // Get toc
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point t0 = time_point;
    long duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    switch (time_mode) {
        case TIME_MODE::MICROSECONDS:
            break;
        case TIME_MODE::MILLISECONDS:
            duration /= (double)1000;
            break;
        case TIME_MODE::SECONDS:
            duration /= (double)10000000;
            break;
        case TIME_MODE::MINUTES:
            duration /= (double)60000000;
            break;
        default:
            break;
    }
    return (double)duration;
}
