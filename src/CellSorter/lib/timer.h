#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

class Timer {
private:
    std::chrono::steady_clock::time_point time_point;
    

    enum TIME_MODE { MICROSECONDS, MILLISECONDS, SECONDS, MINUTES };

public:
    Timer();
    
    void tic();
    long toc();
    double toc(int time_mode);
};

#endif  // TIMER_H
