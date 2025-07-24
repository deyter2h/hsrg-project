#pragma once
#include "Structs.h"
#include <vector>
#include <cmath>

static class BeatFactory {
public:
	static std::vector<Beat> GENERATE_FROM_SECTION(Section section) {
		std::vector<Beat> beats;

		double stepMs = 60000.0 / double(section.bpm) / section.signature.numerator;
		
		unsigned short int beat_index = 1;
		for (double t = section.start_ms; t < section.end_ms; t += stepMs) {
			beats.push_back({ int(std::round(t)), beat_index });

			beat_index++;

			if (beat_index == section.signature.numerator +1) beat_index = 1;
		}
		
		return beats;
	}

	static std::vector<Beat> GENERATE_FROM_SECTIONS(std::vector<Section> sections) {
		std::vector<Beat> ret;

		for (auto& i : sections) {
			auto vec = GENERATE_FROM_SECTION(i);
			ret.insert(ret.end(), vec.begin(), vec.end());
		}
		return ret;
	}

    static size_t findClosestBeatIndex(
        const std::vector<PlayableBeat>& beats,
        int currentTime
    ) {
        // beats must be sorted ascending on timing_ms
        auto it = std::lower_bound(
            beats.begin(),
            beats.end(),
            currentTime,
            [](const PlayableBeat& b, int t) {
                return b.timing_ms < t;
            }
        );

        // 1) if we're past the end, snap to the last element
        if (it == beats.end()) {
            return beats.size() - 1;
        }
        // 2) if we're before the first element, snap to zero
        if (it == beats.begin()) {
            return 0;
        }

        // 3) otherwise compare the found element and its predecessor
        size_t idx = std::distance(beats.begin(), it);
        int    rightDiff = it->timing_ms - currentTime;
        int    leftDiff = currentTime - (it - 1)->timing_ms;

        return (leftDiff <= rightDiff) ? (idx - 1) : idx;
    }
};