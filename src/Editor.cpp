#pragma once

#include "raylib.h"
#include "raygui.h"
#include "raymath.h"
#include <algorithm>
#include <iostream>
#include <functional>
#include "Timable.h"
#include "Editor.h"
#include "Structs.h"
#include "BeatFactory.h"
#include "MapContainer.h"

using namespace std::chrono;

Editor::Editor() {
	_sound = LoadSound("C:/music/sound.wav");

	this->flushSections();

	std::string titleStr = "Editing - " + mapContainer.getWorkingPath();
	SetWindowTitle(titleStr.c_str());
	
	//for first frame init
	this->update();
	updateCurrentInfo();
	this->recalculateBeats();

	setTime(0);

}

void Editor::flushSections() {
	this->sections.clear();
	this->placed_notes.clear();
	sections.push_back({ { 4, 4 },
		DEFAULT_BPM,
		0,
		DEFAULT_SECTION_LENGTH });
}

void Editor::flushPlayedBeats() {
	for (auto& i : calculated_beats) {
		if (i.isPlayed && i.timing_ms > getTimePassed()) i.isPlayed = false;
	}
	for (auto& i : placed_notes) {
		if (i.hit_offset_ms.has_value() && i.timing_ms >= getTimePassed()) i.hit_offset_ms.reset();
	}
}

void Editor::recalculateBeats() {
	auto beats = BeatFactory::GENERATE_FROM_SECTIONS(this->sections);
	this->calculated_beats.clear();
	this->calculated_beats.resize(beats.size() + 1);
	for (int i = 0; i < beats.size(); i++) {
		this->calculated_beats[i] = { beats[i].timing_ms, beats[i].index, false };
	} 
} 

void Editor::listen() {

}

void Editor::render() {
	BeginDrawing();
	ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

	auto events = gui.draw_debug();

	for (auto& e : events) {
		handleGuiEvent(e);
	}
	gui.draw_timeline(getTimePassed(), calculated_beats, sections, placed_notes, _spacingMul, offset);
	EndDrawing();
}

void Editor::updateCurrentInfo() {
	int now = getTimePassed();

	int currentBeatId = BeatFactory::findClosestBeatIndex(calculated_beats, now);

	auto it = std::find_if(sections.begin(), sections.end(),
		[&](auto& sec) { return now >= sec.start_ms && now <= sec.end_ms; });
	int currentSectionId = (it == sections.end()
		 ? 0
		 : (int)std::distance(sections.begin(), it));

	for (auto& i : placed_notes) {
		if (i.timing_ms <= now && !i.hit_offset_ms.has_value()) {
			PlaySound(_sound);
			i.hit_offset_ms = now;
		}
	}

	this->currentState = { currentSectionId, currentBeatId };
	
	updateExternalGuiValues();

	if (!isPaused() && isMusicValidLocal) {
		UpdateMusicStream(_music);
	}
}

void Editor::update() {
	
	this->tick();
	this->listen();
	if (!isPaused()) this->updateCurrentInfo(); //
	
	this->render();
}

void Editor::adjustToNearestBeat() {
	size_t id = BeatFactory::findClosestBeatIndex(calculated_beats, getTimePassed());
	int closestTiming = calculated_beats[id].timing_ms;
	
	setTime(closestTiming);
	updateCurrentInfo();

	if (isMusicValidLocal) {
		SeekMusicStream(_music, (closestTiming + offset) / 1000.0f);
	}
}

void Editor::updateExternalGuiValues() {
	this->gui.offsetVal = offset;
	this->gui.bpmVal = sections[currentState.sectionId].bpm;
	auto &sig = sections[currentState.sectionId].signature;
	this->gui.selectedSignature = findSignatureIndex(sig.numerator, sig.denominator);

	float v = ((float)getTotalLengthMs() / 100.0f);
	float fr = (float)getTimePassed() / v;
	gui.timelineVal = fr;
}

void Editor::spawnNote(Note n) {
	int now = getTimePassed();
	if (std::none_of(placed_notes.begin(), placed_notes.end(),
		[&](auto& x) {//? display error otherwise idk
			return (x.timing_ms == now || (now >= x.timing_ms && now <= x.timing_ms + x.length_ms)); 
		}))
		 {
		placed_notes.push_back({ n });
		editingNoteIndex = placed_notes.size() - 1;
		}


}

void Editor::spawnSection(Section s) {
	if (sections.size()) sections[currentState.sectionId].end_ms = s.start_ms;
	
	sections.push_back(s);

	std::sort(sections.begin(), sections.end(), [](const auto& lhs, const auto& rhs)
		{
			return lhs.start_ms < rhs.start_ms;
		});
	
}

int Editor::getTotalLengthMs() {
	return sections[currentState.sectionId].end_ms; //Assume they are sorted and one always presents
}

void Editor::handleGuiEvent(const GuiEvent& e) {
	if (!sections.size()) return; 

	switch (e.type) {
	case GuiEventType::Play: {
		actionPlay();
		break;
	}
	case GuiEventType::Restart: {
		actionRestart();
		break;
	}

	case GuiEventType::Load: {
		actionLoadData();
		break;
	}
		
	case GuiEventType::Save: {
		actionSaveData();
		break;
	}
		
	case GuiEventType::Skip: {
		actionSkipTime(std::stof(e.payload));
		break;
	}

	case GuiEventType::SkipStep: {
		actionSkipStep(std::stof(e.payload));
		break;
	}

	case GuiEventType::Offset: {
		actionChangeOffset(std::stof(e.payload));
		break;
	}

	case GuiEventType::Bpm: {
		actionChangeSectionBpm(std::stof(e.payload));
		break;
	}

	case GuiEventType::Signature: {
		actionChangeSectionSignature(std::stoi(e.payload));
		break;
	}

	case GuiEventType::Spacing: {
		actionChangeSpacing(std::stof(e.payload));
		break;
	}

	case GuiEventType::InsertSection: {
		actionInsertSection();
		break;
	}

	case GuiEventType::RemoveSection: {
		actionRemoveSection();
		break;
	}

	case GuiEventType::PlaybackSpeed: {
		actionChangePlayback(std::stof(e.payload));
		break;
	}

	case GuiEventType::InsertNoteStart: {
		actionPlaceNote();
		break;
	}

	case GuiEventType::InsertNoteEnd: {
		actionEditNote();
		break;
	}

	case GuiEventType::RemoveNote: {
		actionRemoveNote();
		break;
	}

	case GuiEventType::MP3: {
		actionLoadAudio();
		break;
	}
	}

	recalculateBeats();
	adjustToNearestBeat();

}

void Editor::actionPlay() {
	isPaused() ? actionResume() : actionPause();
}

void Editor::actionResume() {
	resume();
	if (isMusicValidLocal) ResumeMusicStream(_music);
}

void Editor::actionPause() {
	pause();
	if (isMusicValidLocal) PauseMusicStream(_music);
}

void Editor::actionRestart() {
	setTime(0);
	if (isMusicValidLocal) {
		ResumeMusicStream(_music);
		SeekMusicStream(_music, (float)offset / 1000.0f);
	}
	actionResume();
}

void Editor::actionLoadAudio() {
	const char* patterns[] = { "*.mp3" };
	char const* result = tinyfd_openFileDialog(
		"title",
		"",
		1,
		patterns,
		nullptr,
		0
	);
	if (!result) return; // Error
	_music = LoadMusicStream(result);
	if (!IsMusicValid(_music)) return; //Error
	isMusicValidLocal = true;

	mapContainer.modifyMap(std::string(result), { sections, placed_notes, offset });

	PlayMusicStream(_music);
	PauseMusicStream(_music);
	SetMusicVolume(_music, 2.0f);
	actionPause();
}

void Editor::actionLoadData() {
	char const* result = tinyfd_selectFolderDialog("Select folder", "");
	if (!result) return;

	auto map = mapContainer.loadExisting(std::string(result));

	std::string titleStr = "Editing - " + mapContainer.getWorkingPath();
	SetWindowTitle(titleStr.c_str());

	this->offset = map.data.offset;

	_music = map.music;

	PlayMusicStream(_music);
	PauseMusicStream(_music);
	SetMusicVolume(_music, 2.0f);
	actionPause();

	flushSections();
	sections.clear();

	for (auto& s : map.data.sections) this->spawnSection(s);
	for (auto& n : map.data.notes) this->spawnNote(n);

	////temp
	isMusicValidLocal = true;
}

void Editor::actionSaveData() {
	std::sort(placed_notes.begin(), placed_notes.end(), [](const auto& lhs, const auto& rhs) { return lhs.timing_ms < rhs.timing_ms; });

	mapContainer.modifyMap(mapContainer.getAudioPath(), {sections, placed_notes, offset});
}

void Editor::actionSkipTime(float percent) {
	int totalSkipLengthMs = sections[sections.size() - 1].end_ms;
	int ms = int(std::ceil(((float)totalSkipLengthMs / 100.0f) * percent));

	setTime(ms);

	flushPlayedBeats();
}

void Editor::actionSkipStep(float delta) {
	int id = std::clamp<int>(currentState.beatId + (int)delta, 0, (int)calculated_beats.size() - 1);
	int timing = calculated_beats[id].timing_ms;

	this->setTime(timing);

	flushPlayedBeats();
}

void Editor::actionChangePlayback(float multiplier) {
	this->setMul(multiplier);
	if (isMusicValidLocal) SetMusicPitch(_music, multiplier);
}

void Editor::actionChangeOffset(int offset) {
	this->offset = offset;
	if (isMusicValidLocal) SeekMusicStream(_music, (float)offset / 1000.0f);
}

void Editor::actionInsertSection() {
	Section newSec;
	newSec.signature = ALLOWED_SIGNATURES[gui.selectedSignature];
	newSec.bpm = gui.bpmVal;;
	newSec.start_ms = getTimePassed();
	newSec.end_ms = (currentState.sectionId + 1) >= sections.size() ? sections[currentState.sectionId].end_ms : sections[currentState.sectionId + 1].end_ms;
	spawnSection(newSec);
}

void Editor::actionRemoveSection() {
	if (currentState.sectionId == 0) return; // Error

	if (sections.empty() || currentState.sectionId < 0 ||
		currentState.sectionId >= (int)sections.size()) return; // Error

	sections.erase(sections.begin() + currentState.sectionId);

	for (int i = 0; i + 1 < (int)sections.size(); ++i) {
		sections[i].end_ms = sections[i + 1].start_ms;
	}

	if (sections.size() == 1) {
		sections[0].end_ms = getTotalLengthMs() > 0 ? getTotalLengthMs() : DEFAULT_SECTION_LENGTH;
	}

	sections[sections.size() - 1].end_ms = getTotalLengthMs() > 0 ? getTotalLengthMs() : DEFAULT_SECTION_LENGTH;
}

void Editor::actionJumpSection() {
}

void Editor::actionChangeSectionBpm(int bpm) {
	sections[currentState.sectionId].bpm = bpm;
}

void Editor::actionChangeSectionSignature(size_t signatureId) {
	sections[currentState.sectionId].signature = ALLOWED_SIGNATURES[signatureId];
}

void Editor::actionChangeSpacing(float multiplier) {
	this->_spacingMul = multiplier;
}

void Editor::actionPlaceNote() {
	spawnNote({ getTimePassed(), 0 });
	editingNoteIndex = placed_notes.size() - 1;
}

void Editor::actionEditNote() {
	if (editingNoteIndex >= placed_notes.size() || editingNoteIndex == -1) return; // Error
	placed_notes[editingNoteIndex].length_ms = getTimePassed() - placed_notes[editingNoteIndex].timing_ms;
	editingNoteIndex = -1;
}

void Editor::actionRemoveNote() {
	int now = getTimePassed();
	std::erase_if(placed_notes,
		[now](auto const& note) { return note.timing_ms == now; });

	if (editingNoteIndex >= placed_notes.size() || editingNoteIndex == -1) return;
	placed_notes.erase(placed_notes.begin() + editingNoteIndex);

	editingNoteIndex = -1;
}

