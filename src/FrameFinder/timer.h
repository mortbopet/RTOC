//
// Created by Jens Lindahl on 04/03/2018.
//

#ifndef FRAMEFINDER_TICTOC_H
#define FRAMEFINDER_TICTOC_H

#include <iostream>
#include <ctime>
#include <chrono>

namespace timer {

    enum TIME_MODE {
        TM_TICKS,
        TM_NANOSECONDS,
        TM_MICROSECONDS,
        TM_MILLISECONDS,
        TM_SECONDS
    };

    inline void tic(std::chrono::steady_clock::time_point& t0) {
        // Set tic
        t0 = std::chrono::steady_clock::now();
    };

    inline double toc(const std::chrono::steady_clock::time_point& t0, const int timemode) {
        // Get toc
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        // Return count
        switch (timemode) {
            case TM_TICKS:
            case TM_NANOSECONDS:
                return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            case TM_MICROSECONDS:
                return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
            case TM_MILLISECONDS:
                return std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
            case TM_SECONDS:
                return std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();
            default :
                return -1;
        }

    }
    inline void tocp(const std::chrono::steady_clock::time_point& t0) {
        // Get toc
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        std::string string_to_print = "Elapsed time: ";
        long duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

        if (duration > 60000) {
            duration /= 60000;
            string_to_print += std::to_string(duration) + " minutes.";
        } else if (duration > 1000) {
            duration /= 1000;
            string_to_print += std::to_string(duration) + " seconds.";
        } else {
            string_to_print += std::to_string(duration) + " milliseconds.";
        }
        std::cout << string_to_print << std::endl;
    }

}


#endif //FRAMEFINDER_TICTOC_H
