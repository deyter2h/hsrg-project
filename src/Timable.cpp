#include "./Timable.h"
#include <chrono>
#include <iostream>

Timeable::Timeable() : startTime(std::chrono::steady_clock::now()),
pauseTime(), _isPaused(false) {

}

void Timeable::tick()
{
    this->timePassedMs = generateTimePassed();

    /*if (timePassedMs >= ) {

    }*/
}

int Timeable::generateTimePassed() {
    using namespace std::chrono;
    if (_isPaused) return (int)duration_cast<milliseconds>(pauseTime - startTime).count() + time_added;
    else {
        auto now = steady_clock::now();
        return (int)duration_cast<milliseconds>(now - startTime).count() + time_added;
    }
}

int Timeable::getTimePassed() const
{
    return this->timePassedMs;
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

//void Timeable::setBpm(unsigned int bpm)
//{
//    this->bpm = bpm;
//}

//void Timeable::setSignature(SignatureInfo sign)
//{
//    this->signature = sign;
//}

void Timeable::incrementTime(int ms)
{
    this->time_added += ms;
}

//int Timeable::getSignatureDivision() const {
//    //float div = 4;
//    //switch (signature) {
//    //case Signature::ONE_FOUR:  div = 4; break;
//    //case Signature::ONE_THREE: div = 3; break;
//    //default:  div = 4; break;
//    //}
//    return signature.denominator;
//}

//double  Timeable::getBeatTime() const
//{
//    return  60000.0 / (double)bpm / double(getSignatureDivision());
//}

//std::vector<Beat> Timeable::calculateBeats(unsigned int segments) const
//{
//    std::vector<Beat> beats;
//    if (segments == 0) return beats;
//
//    int division = getSignatureDivision();  
//    double stepMs = 60000.0 / double(bpm) / division;
//
//    double nowMs = double(getTimePassed());
//
//    double beatsSinceStart = nowMs / stepMs;
//    if (beatsSinceStart < 0) beatsSinceStart = 0;
//    int measureIndex = int(std::floor(beatsSinceStart / division));
//
//    double measureStartMs = double(measureIndex) * division * stepMs;
//
//    for (unsigned int i = 0; i < segments * division; ++i) {
//        double beatTimeMs = measureStartMs + double(i) * stepMs;
//        int roundedTime = int(std::round(beatTimeMs));
//
//        unsigned int beatIndex = i % division;  
//        beats.push_back({ signature, roundedTime, beatIndex });
//    }
//
//    return beats;
//}

