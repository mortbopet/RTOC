#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

class Timer {
public:
    std::chrono::steady_clock::time_point time_point;

    enum TIME_MODE { MICROSECONDS, MILLISECONDS, SECONDS, MINUTES };

    inline void tic(Timer& t0);
    inline long toc(const Timer& timer);
    inline double toc(const Timer& timer, int time_mode);
};

#endif  // TIMER_H