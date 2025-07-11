#include "./Timable.h"
#include <chrono>
#include <iostream>

Timeable::Timeable() : startTime(std::chrono::steady_clock::now()),
pauseTime(), _isPaused(false) {
    this->bpm = 60;
    this->signature = Signature::ONE_FOUR;
}

void Timeable::tick() const
{
    if (_isPaused) return;

    /*if (this->getTimePassed() >= ) {

    }*/
}

int Timeable::getTimePassed() const
{
    using namespace std::chrono;
    if (_isPaused) return (int)duration_cast<milliseconds>(pauseTime - startTime).count() + time_added;
    else {
        auto now = steady_clock::now();
        return (int)duration_cast<milliseconds>(now - startTime).count() + time_added;
    }
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

void Timeable::setBpm(unsigned int bpm)
{
    this->bpm = bpm;
}

void Timeable::setSignature(Signature sign)
{
    this->signature = sign;
}

void Timeable::incrementTime(int ms)
{
    this->time_added += ms;
}

unsigned int Timeable::getBeatTime() const
{
    //rewrite it
    int div = 4;
    switch (signature) {
    case Signature::ONE_FOUR:  div = 4; break;
    case Signature::ONE_THREE: div = 3; break;
    default:  div = 4; break;
    }

    return  60000.0 / bpm / double(div);;
}

std::vector<Beat> Timeable::getCurrentBeats(int timing_ms, unsigned int distance_ms) const
{
    std::vector<Beat> beats;
    // 1) division from signature
    int division = 4;
    switch (signature) {
    case Signature::ONE_FOUR:  division = 4; break;
    case Signature::ONE_THREE: division = 3; break;
    default: /* leave as 4 */           break;
    }

    // 2) ms per beat
    double stepMs = 60000.0 / double(bpm) / double(division);

    // 3) compute index range
    double windowStart = double(timing_ms);
    double windowEnd = windowStart + double(distance_ms);

    int firstIdx = int(std::ceil(windowStart / stepMs));
    int lastIdx = int(std::floor(windowEnd / stepMs));
    if (lastIdx < firstIdx) return beats;

    // 4) collect
    for (int idx = firstIdx; idx <= lastIdx; ++idx) {
        int t = int(std::round(idx * stepMs));
        beats.push_back({ signature, t });
    }
    return beats;
}

