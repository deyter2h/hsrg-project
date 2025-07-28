#pragma once
#include <Timable.h>
#include "raylib.h"
#include "Editor-Gui.h"

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
	void onUpdateTimeline();
	void updateCurrentInfo();

	void onNewSection(Section s);
	void onNewNote(Note n);

	void adjustToNearestBeat();
	void flushSections();

	void updateExternalGuiValues();

	EditorGui gui;
	void handleGuiEvent(const GuiEvent& e);

	//tempppp
	Music _music;
	Sound _sound;
	bool isMusicValidLocal = false;
	int songLengthMs = 0;

};

