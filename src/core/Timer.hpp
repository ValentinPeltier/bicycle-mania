#pragma once

#include <chrono>

class Timer {
        typedef std::chrono::time_point<std::chrono::high_resolution_clock> Time_T;

    public:
        Timer();
        void reset();
        long getNanoseconds();
        long getMilliseconds();
        long getSeconds();
        long getMinutes();
        long getHours();

    private:
        Time_T startTime;

        Time_T getTime() const;
};