#include "Beatmap.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>

static Signature stringToSig(const std::string& s) {
    if (s == "4/4") return Signature::ONE_FOUR;
    if (s == "3/4") return Signature::ONE_THREE;
    throw std::runtime_error("Unknown signature: " + s);
}

static std::string sigToString(Signature s) {
    switch (s) {
    case Signature::ONE_FOUR:  return "4/4";
    case Signature::ONE_THREE: return "3/4";
    }
    return "4/4";
}

void BeatmapConstructor::init(size_t bpm) {
    this->bpmSections.push_back({ Signature::ONE_FOUR, bpm, 0 });

    /*if (music.frameCount == 0) {
        std::cout << "cant load song" << std::endl;
        return;
    }*/

    //float seconds = GetMusicTimeLength(music);
    //this->songLengthMs = int(std::round(seconds * 1000.0f));
}

BeatmapConstructor::~BeatmapConstructor() {
}

std::vector<int> BeatmapConstructor::getTimings() {
    std::vector<int> result;
    if (songLengthMs <= 0) return result;

    std::sort(bpmSections.begin(), bpmSections.end(),
        [](auto& a, auto& b) { return a.timingMs < b.timingMs; });

    for (size_t idx = 0; idx < bpmSections.size(); ++idx) {
        auto& sec = bpmSections[idx];
        int div = 4;
        switch (sec.signature) {
        case Signature::ONE_FOUR:  div = 4; break;
        case Signature::ONE_THREE: div = 3; break;
        default:                    div = 4; break;
        }

        int startMs = sec.timingMs;
        int endMs = (idx + 1 < bpmSections.size())
            ? bpmSections[idx + 1].timingMs
            : songLengthMs;

        if (endMs <= startMs)
            continue;

        double stepMs = 60000.0 / sec.bpm / double(div);
        int    count = int(std::ceil((endMs - startMs) / stepMs));

        for (int i = 0; i <= count; ++i) {
            int t = int(std::round(startMs + i * stepMs));
            if (t < endMs)
                result.push_back(t + sec.offsetMs);
        }
    }

    return result;
}

size_t BeatmapConstructor::getSongLengthMs() {
    //
    return 999999;
}

void BeatmapConstructor::placeNote(std::vector<int> &timings,
    size_t startId,
    size_t endId)
{
    entries.push_back({ timings[startId], timings[endId] - timings[startId] });
    timedEntries.push_back({ startId, endId });
    
}

std::vector<BpmSection> BeatmapConstructor::getSections() {
    return this->bpmSections;
}

void BeatmapConstructor::addSection(BpmSection section) {
    this->bpmSections.push_back(section);
}

void BeatmapConstructor::removeSection(size_t id) {
    if (id > 0 && id < bpmSections.size()) this->bpmSections.erase(bpmSections.begin() + id);
}

void BeatmapConstructor::readFrom(const std::string& path) {
    std::ifstream in{ path };
    if (!in) throw std::runtime_error("Cannot open " + path);

    std::string line;
    enum class Mode { Sections, Notes } mode = Mode::Sections;

    while (std::getline(in, line)) {
        // strip leading/trailing whitespace
        if (line.empty() || line[0] == '#') continue;

        if (line.find(',') == std::string::npos) continue;
        std::istringstream ss{ line };
        std::string a, o, b, c;

        // parse sections until we see a blank line, then switch to Notes
        if (mode == Mode::Sections) {
            std::getline(ss, a, ',');
            std::getline(ss, o, ',');
            std::getline(ss, b, ',');
            std::getline(ss, c);
            BpmSection sec;
            sec.timingMs = std::stoi(a);
            sec.offsetMs = std::stoi(o);
            sec.bpm = std::stoi(b);
            sec.signature = stringToSig(c);
            bpmSections.push_back(sec);

            // detect blank line as section→notes boundary
            if (in.peek() == '\n') mode = Mode::Notes;
        }
        else {
            // Notes mode: two fields only
            std::getline(ss, a, ',');
            std::getline(ss, b);
            TimedNote n;
            n.startId = std::stoi(a);
            n.endId = std::stoi(b);
            timedEntries.push_back(n);
        }
    }

    //с учетом таймингов
    auto timings = this->getTimings();
    for (auto& i : timedEntries) {
        if (i.startId >= timings.size() || i.endId >= timings.size()) continue;
        entries.push_back({ timings[i.startId],timings[i.endId] - timings[i.startId] });
    }
}

void BeatmapConstructor::writeTo(const std::string& path) {
    std::ofstream out{ path };
    out << "# Sections: timeMs,offsetMs,bpm,signature\n";
    for (auto& sec : bpmSections) {
        out
            << sec.timingMs << ','
            << sec.offsetMs << ','
            << sec.bpm << ','
            << sigToString(sec.signature)
            << '\n';
    }

    out << "\n# Notes: startId,endId\n";
    for (auto& n : timedEntries) {
        out
            << n.startId << ','
            << n.endId << '\n';
    }
}