#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

enum class TIME_MODE { MICROSECONDS, MILLISECONDS, SECONDS, MINUTES };

class Timer {
private:
    std::chrono::steady_clock::time_point time_point;
    TIME_MODE m_timeMode;

public:
    Timer(TIME_MODE t = TIME_MODE::MILLISECONDS) : m_timeMode(t) {}

    void tic() {  // Set tic
        time_point = std::chrono::steady_clock::now();
    }

    double toc() {  // Get toc
        std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point t0 = time_point;
        long duration = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        switch (m_timeMode) {
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
};

#endif  // TIMER_H
