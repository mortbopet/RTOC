#include <chrono>
#include <iostream>

#include "timer.h"

inline void Timer::tic(Timer& t0) {
    // Set tic
    t0.time_point = std::chrono::steady_clock::now();
};

inline long Timer::toc(const Timer& timer) {
    // Get toc
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point t0 = timer.time_point;
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

inline double Timer::toc(const Timer& timer, int time_mode) {
    // Get toc
    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point t0 = timer.time_point;
    long duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    switch (time_mode) {
        case MICROSECONDS:
            break;
        case MILLISECONDS:
            duration /= (double)1000;
            break;
        case SECONDS:
            duration /= (double)10000000;
            break;
        case MINUTES:
            duration /= (double)60000000;
            break;
        default:
            break;
    }
    return (double)duration;
}
