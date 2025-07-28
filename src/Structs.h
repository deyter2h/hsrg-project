#pragma once
#include <string>
#include <optional>

struct SignatureInfo {
	int numerator;
	int denominator;
	std::string str;
};

struct Beat {
	int timing_ms;
	short unsigned int index;
};

struct PlayableBeat : public Beat {
	bool isPlayed = false;
};

struct Section {
	SignatureInfo signature;
	float bpm;
	float start_ms;
	float end_ms;
};

struct CurrentState {
	int sectionId;
	int beatId;
};

struct Note {
	int timing_ms;
	int length_ms;
	std::optional<int> hit_offset_ms;
	std::optional<int> release_offset_ms;
};

struct Map {
	std::vector<Section> sections;
	std::vector<Note> notes;
	int offset;
};