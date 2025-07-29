#pragma once
#include <string>
#include <optional>
#include <raylib.h>
#include <vector>

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

struct MapData {
	std::vector<Section> sections;
	std::vector<Note> notes;
	int offset;
};

struct MapMetadata {
    std::string name;
    std::string author;
};

struct MapLoad {
    MapData mapData;
    Music music;
    Image bg;
    MapMetadata meta;
};

enum class GuiEventType {
    Play,
    Restart,
    MP3,
    Load,
    Save,
    Skip,
    SkipStep,
    PlaybackSpeed,
    Offset,
    InsertSection,
    RemoveSection,
    JumpSection,
    Bpm,
    Signature,
    Spacing,
    InsertNoteStart,
    InsertNoteEnd,
    RemoveNote,
    // …add more as you grow…
};

struct GuiEvent {
    GuiEventType type;
    std::string payload;
};