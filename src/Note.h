#pragma once
#include <optional>

struct Note {
	int timing_ms;
	int length_ms;
	std::optional<int> hit_offset_ms;
	std::optional<int> release_offset_ms;
};

