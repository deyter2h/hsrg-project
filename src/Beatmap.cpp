#include "Beatmap.h"
#include <cmath>
#include <iostream>

void Beatmap::load(int bpm, const std::string& filename) {
    this->bpm = bpm;

    this->music = LoadMusicStream(filename.c_str());
    if (music.frameCount == 0) {
        std::cout << "cant load song" << std::endl;
        return;
    }

    float seconds = GetMusicTimeLength(music);
    this->songLengthMs = int(std::round(seconds * 1000.0f));
}

Beatmap::~Beatmap() {
    UnloadMusicStream(music);
}

std::vector<int> Beatmap::getTimingsFor(Signature signature) {
    std::vector<int> result;
    if (songLengthMs <= 0) return result;

    int div = 4;
    switch (signature) {
    case Signature::ONE_FOUR:  div = 4; break;
    case Signature::ONE_THREE: div = 3; break;
    }

    double stepMs = (60000.0 / bpm) / double(div);
    int count = int(std::ceil(songLengthMs / stepMs));
    result.reserve(count);

    for (int i = 0; i <= count; ++i) {
        int t = int(std::round(i * stepMs));
        if (t <= songLengthMs) result.push_back(t);
    }
    return result;
}

void Beatmap::placeNote(Signature signature,
    int timingStartId,
    int timingEndId)
{
    auto timings = getTimingsFor(signature);
    if (timingStartId < 0 || timingStartId >= (int)timings.size() ||
        timingEndId   < 0 || timingEndId >= (int)timings.size() ||
        timingStartId > timingEndId)
        return;

    if (timings.empty()) return;

    int startMs = timings[timingStartId];
    int length = timings[timingEndId] - startMs;

    // 2) Добавляем запись с временным group = -1
    entries.push_back({ {startMs, length}, signature, -1 });
    size_t idx = entries.size() - 1;

    
}

