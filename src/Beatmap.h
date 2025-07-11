#pragma once
#include "Note.h"
#include "raylib.h"
#include <vector>
#include <string>
#include <optional>
#include <deque>

#include "./Timable.h"

struct BpmSection {
	Signature signature;
	size_t bpm;
	int timingMs;
	int offsetMs;
};

struct TimedNote {
	size_t startId;
	size_t endId;
};

class BeatmapConstructor : Timeable {
public:
	BeatmapConstructor() = default;

	~BeatmapConstructor();

	void readFrom(const std::string& path);
	void writeTo(const std::string& path);

	size_t getSongLengthMs();
						  
	std::vector<int> getTimings();

	void placeNote(std::vector<int>& timings,
		size_t startId,
		size_t endId);

	std::vector<BpmSection> getSections();
	void addSection(BpmSection section);
	void removeSection(size_t id);

	const std::deque<Note>& getEntries() const { return entries; }

	void onBeat() override;

private:
	std::vector<BpmSection> bpmSections;
	size_t songLengthMs = 100000;
	size_t offsetMs = 0;

	std::deque<Note> entries;   
	std::deque<TimedNote> timedEntries;
};
