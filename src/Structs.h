#pragma once

#include <vector>

struct BeatRow {
	std::vector<bool> data;

	BeatRow(int divisions) : data(divisions, false) {}

	auto operator[](int i) { return data[i]; }
	auto operator[](int i) const { return data[i]; }
};

struct Section {
	int bpm;
	int divisions;
	std::vector<BeatRow> beats;
};

struct Start {
	int sectionId;
	int beatRowid;
};

struct MapInfo {
	int songOffsetMs;
	float playbackSpeed;
	std::vector<Section> sections;
	Start start;
};