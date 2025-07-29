#pragma once
#include <Timable.h>
#include "raylib.h"
#include "Editor-Gui.h"
#include "MapContainer.h"

class Editor : public Timeable {
public:
	Editor();

	void update();

private:

	float _spacingMul = 1.0f;
	int offset = 0;

	std::vector<Section> sections;
	std::vector<PlayableBeat> calculated_beats;
	std::vector<Note> placed_notes;
	CurrentState currentState;

	void flushPlayedBeats();

	void render();
	void listen();
	void recalculateBeats();
	void updateCurrentInfo();

	void spawnSection(Section s);
	void spawnNote(Note n);

	void adjustToNearestBeat();
	void flushSections();

	void updateExternalGuiValues();
	int getTotalLengthMs();

	EditorGui gui;
	void handleGuiEvent(const GuiEvent& e);

	void actionPlay();
	void actionResume();
	void actionPause();
	void actionRestart();
	void actionLoadAudio();
	void actionLoadData();
	void actionSaveData();
	void actionSkipTime(float percent); // 0.0f -> 100.0f
	void actionSkipStep(float delta); // -1.0f || +1.0f
	void actionChangePlayback(float multiplier); // 0.01f -> 2.0f
	void actionChangeOffset(int offset); 
	void actionInsertSection();
	void actionRemoveSection();
	void actionJumpSection(); // impl
	void actionChangeSectionBpm(int bpm);
	void actionChangeSectionSignature(size_t signatureId);
	void actionChangeSpacing(float multiplier);  // 0.01f -> 2.0f
	void actionPlaceNote();
	void actionEditNote();
	void actionRemoveNote();

	//tempppp
	Music _music;
	Sound _sound;
	bool isMusicValidLocal = false;

	int editingNoteIndex = -1;

	MapContainer mapContainer;

};
