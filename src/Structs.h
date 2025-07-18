#pragma once
#include <string>
#include <optional>

struct SignatureInfo {
	int numerator;
	int denominator;
};

struct Beat {
	int timing_ms;
};

struct Section {
	SignatureInfo signature;
	int bpm;
	int start_ms;
	int end_ms;
};

struct Note {
	int timing_ms;
	int length_ms;
	std::optional<int> hit_offset_ms;
	std::optional<int> release_offset_ms;
};
