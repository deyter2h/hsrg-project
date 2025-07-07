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

    // 1) Проверка индексов…
    auto timings = getTimingsFor(signature);
    if (timingStartId < 0 || timingStartId >= int(timings.size()) ||
        timingEndId   < 0 || timingEndId >= int(timings.size()) ||
        timingStartId > timingEndId)
        return;

    if (timings.empty()) return;

    int startMs = timings[timingStartId];
    int length = timings[timingEndId] - startMs;

    // 2) Добавляем запись
    entries.push_back({ {startMs, length}, signature });
    size_t idx = entries.size() - 1;

    // 3) Инкрементальная группировка по разнице timingStartId
    static int  lastTimingId = -1;
    static int  lastDelta = 0;
    static bool first = true;
    static bool second = false;

    int delta = timingStartId - lastTimingId;

    if (first) {
        groups.clear();
        groups.push_back({ idx, idx, signature });
        first = false;
        second = true;
    }
    else if (second) {
        // вторая нота расширяет первую группу из [0..0] в [0..1]
        lastDelta = delta;
        groups.back().startIdx = idx - 1;
        groups.back().endIdx = idx;
        groups.back().signature = signature;
        second = false;
    }
    else {
        if (delta == lastDelta) {
            // тот же шаг — просто расширяем актуальную группу вправо
            groups.back().endIdx = idx;
        }
        else {
            // шаг изменился — создаём новую группу из двух последних нот
            lastDelta = delta;
            groups.push_back({ idx - 1, idx, signature });
        }
    }

    // 4) Обязательно в конце обновляем lastTimingId
    lastTimingId = timingStartId;
}

