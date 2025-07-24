#include "./Timable.h"
#include <chrono>
#include <iostream>
#include <algorithm>

Timeable::Timeable() : startTime(std::chrono::steady_clock::now()),
pauseTime(), _isPaused(false) {
    this->restart();
}

void Timeable::tick()
{
    this->timePassedMs = generateTimePassed();

    /*if (timePassedMs >= ) {

    }*/
}

int Timeable::generateTimePassed() {
    using namespace std::chrono;
    double rawMs;
    if (_isPaused) {
        rawMs = duration_cast<milliseconds>(pauseTime - startTime).count();
        
    }
    else {
        rawMs = duration_cast<milliseconds>(steady_clock::now() - startTime).count();
    }
    double scaled = rawMs * double(_speedMul) + double(time_added);
    return int(std::round(scaled));
}

int Timeable::getTimePassed() const
{
    return this->timePassedMs;
}

void Timeable::setTime(int ms) {
    using namespace std::chrono;
    auto now = steady_clock::now();

    time_added = ms;
    startTime = now;

    if (_isPaused) {
        pauseTime = now;
    }

    timePassedMs = ms;
}

void Timeable::setMul(float m) {
    using namespace std::chrono;
   
    int curr = getTimePassed();

    auto now = steady_clock::now();
    startTime = now;
    time_added = curr;

    if (_isPaused) {
        pauseTime = now;
    }

    _speedMul = std::clamp(m, 0.1f, 4.0f);

    timePassedMs = curr;
}

void Timeable::resume()
{
    if (_isPaused) {
        auto now = std::chrono::steady_clock::now();
        auto pausedFor = now - pauseTime;
        startTime += pausedFor;
        _isPaused = false;
    }
}

void Timeable::pause()
{
    if (!_isPaused) {
        pauseTime = std::chrono::steady_clock::now();
        _isPaused = true;
    }
}

void Timeable::restart()
{
    this->startTime = std::chrono::steady_clock::now();
    _isPaused = false;
    time_added = 0;
}

bool Timeable::isPaused() const
{
    return this->_isPaused;
}

void Timeable::incrementTime(int ms)
{
    this->time_added += ms;
}
