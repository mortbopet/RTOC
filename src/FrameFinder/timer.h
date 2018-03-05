//
// Created by Jens Lindahl on 04/03/2018.
//

#ifndef FRAMEFINDER_TICTOC_H
#define FRAMEFINDER_TICTOC_H

#include <iostream>
#include <ctime>
#include <chrono>

namespace timer {

    class Timer {
    public:
        std::chrono::steady_clock::time_point tick;
    };

    enum TIME_MODE {
        TM_TICKS,
        TM_NANOSECONDS,
        TM_MICROSECONDS,
        TM_MILLISECONDS,
        TM_SECONDS
    };

    inline void tic(Timer& t0) {
        // Set tic
        t0.tick = std::chrono::steady_clock::now();
    };

    inline void toc(const Timer& timer) {
        // Get toc
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point t0 = timer.tick;
        std::string string_to_print = "Elapsed time: ";
        long duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        if (duration > 60000000) {
            duration /= 60000;
            string_to_print += std::to_string(duration) + " minutes.";
        } else if (duration > 1000000) {
            duration /= 1000000;
            string_to_print += std::to_string(duration) + " seconds.";
        } else if (duration > 1000){
            duration /= 1000;
            string_to_print += std::to_string(duration) + " milliseconds.";
        } else {
            string_to_print += std::to_string(duration) + " microseconds.";
        }
        std::cout << string_to_print << std::endl;
    }
    inline double toc(const Timer& timer, int overload) {
        // Get toc
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point t0 = timer.tick;
        std::string string_to_print = "Elapsed time: ";
        long duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        return duration /= 1000000;

    }
}


#endif //FRAMEFINDER_TICTOC_H
