#pragma once

#include "raylib.h"
#include "raygui.h"
#include "raymath.h"
#include <algorithm>
#include <iostream>
#include <functional>
#include "Timable.h"
#include "Editor.h"
#include "BeatFactory.h"
#include "MapStore.h"
#include "tinyfiledialog.h"
using namespace std::chrono;

Editor::Editor() {
	_sound = LoadSound("C:/music/sound.wav");

	this->flushSections();
	
	//for first frame init
	this->update();
	updateCurrentInfo();
	this->onUpdateTimeline();

	setTime(0);

}

void Editor::flushSections() {
	this->sections.clear();
	this->placed_notes.clear();
	sections.push_back({ { 4, 4 },
		DEFAULT_BPM,
		0,
		DEFAULT_SECTION_LENGTH });
	this->songLengthMs = DEFAULT_SECTION_LENGTH;
}

void Editor::flushPlayedBeats() {
	for (auto& i : calculated_beats) {
		if (i.isPlayed && i.timing_ms > getTimePassed()) i.isPlayed = false;
	}
	for (auto& i : placed_notes) {
		if (i.hit_offset_ms.has_value() && i.timing_ms >= getTimePassed()) i.hit_offset_ms.reset();
	}
}

void Editor::onUpdateTimeline() {
	auto beats = BeatFactory::GENERATE_FROM_SECTIONS(this->sections);
	this->calculated_beats.clear();
	this->calculated_beats.resize(beats.size() + 1);
	for (int i = 0; i < beats.size(); i++) {
		this->calculated_beats[i] = { beats[i].timing_ms, beats[i].index, false };
	}
} 

void Editor::listen() {
	float wheel = GetMouseWheelMove();
	if (wheel != 0.0f && sections.size()) {

		int id = std::clamp<int>(currentState.beatId + (int)wheel,0,(int)calculated_beats.size() - 1);
		int timing = calculated_beats[id].timing_ms;

		this->setTime(timing);
		adjustToNearestBeat();
		flushPlayedBeats();
	}
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

	float v = ((float)songLengthMs / 100.0f);
	float fr = (float)getTimePassed() / v;
	gui.timelineVal = fr;
}

void Editor::onNewNote(Note n) {
	int now = getTimePassed();
	if (std::none_of(placed_notes.begin(), placed_notes.end(),
		[&](auto& x) {//? display error otherwise idk
			return (x.timing_ms == now || (now >= x.timing_ms && now <= x.timing_ms + x.length_ms)); 
		}))
		 {
		placed_notes.push_back({ n });
		}
}

void Editor::onNewSection(Section s) {
	sections[currentState.sectionId].end_ms = s.start_ms;
	sections.push_back(s);

	std::sort(sections.begin(), sections.end(), [](const auto& lhs, const auto& rhs)
		{
			return lhs.start_ms < rhs.start_ms;
		});
	
}
void Editor::handleGuiEvent(const GuiEvent& e) {

	static int noteStartMs = 0;

	switch (e.type) {
	case GuiEventType::Play: {
		if (!sections.size()) break;

		this->setTime(calculated_beats[currentState.beatId].timing_ms);

		if (isPaused()) {
			if (isMusicValidLocal) {
				ResumeMusicStream(_music);
			}

			resume();
		}
		else {
			if (isMusicValidLocal) {
				PauseMusicStream(_music);
			}

			pause();
		}
		updateCurrentInfo();
		break;
	}
	case GuiEventType::Restart: {
		this->setTime(0);
		if (!sections.size()) break;
		if (isMusicValidLocal) {
			ResumeMusicStream(_music);
			SeekMusicStream(_music, (float)offset / 1000.0f);
		}
		resume();
		flushPlayedBeats();

		updateCurrentInfo();
		break;
	}

	case GuiEventType::Load: {

		const char* patterns[] = { "*.txt" };
		char const* result = tinyfd_openFileDialog(
			"Select map",
			"",
			1,
			patterns,
			nullptr,
			0
		);
		if (result) {
			TextMapStore s(result);
			auto map = s.load();
			if (!map.has_value()) break;
			auto _sections = map.value().sections;
			if (map.value().sections.size() < 1) break;
			auto _notes = map.value().notes;
			auto _offset = map.value().offset;

			this->offset = _offset;

			flushSections();

			for (auto& s : _sections) {
				this->onNewSection(s);
			}

			this->placed_notes.clear();
			for (auto& n : _notes) {
				this->onNewNote(n);
			}

			this->updateExternalGuiValues();

			break;
		}
	
		break;
	}
		

	case GuiEventType::Save: {
		std::sort(placed_notes.begin(), placed_notes.end(), [](const auto& lhs, const auto& rhs)
			{
				return lhs.timing_ms < rhs.timing_ms;
			});

		std::string path = std::string(GetApplicationDirectory()) + "/map.txt";
		TextMapStore s(path);
		s.save({ sections, placed_notes, offset });
		break;
	}
		

	case GuiEventType::Skip: {
		float frac = std::stof(e.payload);
		int totalSkipLengthMs = sections[sections.size() - 1].end_ms;
		int ms = int(std::ceil(((float)totalSkipLengthMs / 100.0f) * frac));

		setTime(ms);
		adjustToNearestBeat();
		flushPlayedBeats();

		break;
	}

	case GuiEventType::Offset: {
		int v = std::stoi(e.payload);
		this->offset = v;

		break;
	}

	case GuiEventType::Bpm: {
		if (currentState.sectionId == -1 || !sections.size()) break;

		sections[currentState.sectionId].bpm = std::stof(e.payload);

		adjustToNearestBeat();
		onUpdateTimeline();
		break;
	}

	case GuiEventType::Signature: {
		if (currentState.sectionId == -1 || !sections.size()) break;
		//?
		SignatureInfo selectedSignature = ALLOWED_SIGNATURES[std::stoi(e.payload)];
		sections[currentState.sectionId].signature = selectedSignature;

		adjustToNearestBeat();
		onUpdateTimeline();

		break;
	}

	case GuiEventType::Spacing: {
		float fr = std::stof(e.payload);
		this->_spacingMul = fr;
		break;
	}

	case GuiEventType::InsertSection: {
		Section newSec;
		newSec.signature = ALLOWED_SIGNATURES[gui.selectedSignature]; //might break
		newSec.bpm = gui.bpmVal;;
		newSec.start_ms = getTimePassed();
		newSec.end_ms = (currentState.sectionId + 1) >= sections.size() ? sections[currentState.sectionId].end_ms : sections[currentState.sectionId + 1].end_ms;
		onNewSection(newSec);
		updateCurrentInfo();
		onUpdateTimeline();
		break;
	}

	case GuiEventType::RemoveSection: {
		if (currentState.sectionId == 0) return;

		if (sections.empty() || currentState.sectionId < 0 ||
			currentState.sectionId >= (int)sections.size()) break;

		sections.erase(sections.begin() + currentState.sectionId);

		for (int i = 0; i + 1 < (int)sections.size(); ++i) {
			sections[i].end_ms = sections[i + 1].start_ms;
		}

		if (sections.size() == 1) {
			sections[0].end_ms = songLengthMs > 0 ? songLengthMs : DEFAULT_SECTION_LENGTH;
		}
		//?
		sections[sections.size() - 1].end_ms = songLengthMs > 0 ? songLengthMs : DEFAULT_SECTION_LENGTH;

		updateCurrentInfo();

		onUpdateTimeline();
		break;
		
	}

	case GuiEventType::PlaybackSpeed: {
		float sp = std::stof(e.payload);
		this->setMul(sp);
		if (isMusicValidLocal) {
			SetMusicPitch(_music, sp);
		}

		break;
	}

	case GuiEventType::InsertNoteStart: {
		noteStartMs = getTimePassed();

		break;
	}

	case GuiEventType::InsertNoteEnd: {
		int now = getTimePassed();
		e.payload;
		this->onNewNote({ noteStartMs, now - noteStartMs });

		noteStartMs = 0; //?reduntant

		break;
	}

	case GuiEventType::RemoveNote: {
		int now = getTimePassed();
		for (int i = placed_notes.size() - 1; i >= 0; --i) {
			if (placed_notes[i].timing_ms == now) {
				placed_notes.erase(placed_notes.begin() + i);
				break;
			}
		}
		break;
	}

	case GuiEventType::MP3: {
		const char* patterns[] = { "*.mp3" };
		char const* result = tinyfd_openFileDialog(
			"title",
			"",
			1,
			patterns,
			nullptr,
			0
		);
		if (result) {
			_music = LoadMusicStream(result);
			if (IsMusicValid(_music)) {
				isMusicValidLocal = true;

				setTime(0);
				PlayMusicStream(_music);
				PauseMusicStream(_music);
				SetMusicVolume(_music, 2.0f);

				this->flushSections();

				updateCurrentInfo();
				onUpdateTimeline();
			}

			break;
		}

	}


	}

}

