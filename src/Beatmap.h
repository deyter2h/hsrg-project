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
	const std::deque<Grouped>& getGroups() const { return groups; }

private:
	std::vector<int> getTimingsFor(Signature sig) const;

	int                           bpm = 120;
	int                           songLengthMs = 100000;
	Music                         music;

	std::deque<NoteEntry>         entries;   
	std::deque<Grouped>           groups; 
};
