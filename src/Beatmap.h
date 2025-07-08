#pragma once
#include "Note.h"
#include "raylib.h"
#include <vector>
#include <string>
#include <optional>
#include <deque>

//Single bpm impl

enum Signature {
	ONE_FOUR,
	ONE_THREE,
};

struct NoteEntry {
	Note      note;
	Signature signature;
	int group;
};

struct Grouped {
	size_t    startIdx;  
	size_t    endIdx;   
	Signature signature; 
};

class Beatmap {
public:
	Beatmap() = default;

	~Beatmap();

	void load(int bpm, const std::string& filename);
						  
	std::vector<int> getTimingsFor(Signature signature);

	void placeNote(Signature signature, int timingStartId, int timingEndId);

	const std::deque<NoteEntry>& getEntries() const { return entries; }

private:

	int                           bpm = 120;
	int                           songLengthMs = 100000;
	Music                         music;

	std::deque<NoteEntry>         entries;   
};
