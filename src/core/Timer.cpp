#include "Timer.hpp"
#include <chrono>

Timer::Timer()
    : startTime(this->getTime()) {
}

void Timer::reset() {
    this->startTime = this->getTime();
}

long Timer::getNanoseconds() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(this->getTime() - this->startTime).count();
}

long Timer::getMilliseconds() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(this->getTime() - this->startTime).count();
}

long Timer::getSeconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(this->getTime() - this->startTime).count();
}

long Timer::getMinutes() {
    return std::chrono::duration_cast<std::chrono::minutes>(this->getTime() - this->startTime).count();
}

long Timer::getHours() {
    return std::chrono::duration_cast<std::chrono::hours>(this->getTime() - this->startTime).count();
}

Timer::Time_T Timer::getTime() const {
    return std::chrono::high_resolution_clock::now();
}