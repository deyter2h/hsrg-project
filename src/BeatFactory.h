#pragma once
#include "Structs.h"
#include <vector>
#include <cmath>

static class BeatFactory {
public:
	static std::vector<Beat> GENERATE_FROM_SECTION(Section section) {
		std::vector<Beat> beats;

		double stepMs = 60000.0 / double(section.bpm) / section.signature.denominator;
		
		unsigned short int beat_index = 1;
		for (double t = section.start_ms; t < section.end_ms; t += stepMs) {
			beats.push_back({ int(std::round(t)), beat_index });

			beat_index++;

			if (beat_index == section.signature.denominator+1) beat_index = 1;
		}
		
		return beats;
	}
};