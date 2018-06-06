#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

enum class TIME_MODE { MICROSECONDS, MILLISECONDS, SECONDS, MINUTES };

class Timer {
private:
    std::chrono::steady_clock::time_point time_point;

public:
    void tic();
    long toc();
    double toc(TIME_MODE time_mode);
};

#endif  // TIMER_H
