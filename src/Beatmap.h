#pragma once
#include "Note.h"
#include "raylib.h"
#include <vector>
#include <string>
#include <optional>
#include <deque>

enum Signature {
	ONE_FOUR,
	ONE_THREE,
};

class Beatmap {
public:
	Beatmap();

	~Beatmap();

	void load(int bpm, const std::string& filename);
						  
	std::vector<int> getTimingsFor(Signature signature);

	std::deque<Note> getNotes();

	void placeNote(Signature signature, int timingStartId, int timingEndId);

private:
	int bpm;
	int      songLengthMs = 0;
	std::vector<Note> notes;

	Music music;
};
