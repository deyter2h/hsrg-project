#pragma once
#include "Structs.h"
#include <vector>
#include <cmath>

static class BeatFactory {
public:
	static std::vector<Beat> GENERATE_FROM_SECTION(Section section) {
		std::vector<Beat> beats;

		double stepMs = 60000.0 / double(section.bpm) / section.signature.denominator;
		for (double t = section.start_ms; t < section.end_ms; t += stepMs) {
			beats.push_back({ int(std::round(t)) });
		}
		
		return beats;
	}
};