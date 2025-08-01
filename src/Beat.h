#pragma once

#include "Structs.h"
#include <vector>

static std::vector<int> get_beat_timings(MapInfo &info) {
	std::vector<int> timings;

	double timing = (double)info.songOffsetMs;

	for (int s = info.start.sectionId; s < info.sections.size(); s++) {
		auto beat_time = 60000.0f / info.sections[s].bpm / info.sections[s].divisions / info.playbackSpeed;

		for (int b = 0; b < info.sections[s].beats.size(); b++) {
			for (int d = 0; d < info.sections[s].divisions; d++) {
				if (info.sections[s].beats[b][d]) timings.push_back((int)timing);

				timing += (double)beat_time;
			}
		}
	}

	return timings;
}