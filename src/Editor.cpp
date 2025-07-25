#include "raylib.h"
#include "raygui.h"
#include "raymath.h"
#include <algorithm>
#include <iostream>
#include "Timable.h"
#include "Editor.h"
#include "BeatFactory.h"
#include "tinyfiledialog.h"

Editor::Editor() {
	

	_sound = LoadSound("C:/music/sound.wav");
	setTime(0);

	if (!this->sections.size()) {
		sections.push_back({ { 4, 4 },
DEFAULT_BPM,
0,
DEFAULT_SECTION_LENGTH });
	}

	this->onUpdateTimeline();

}

void Editor::flushPlayedBeats() {
	for (auto& i : calculated_beats) {
		if (i.isPlayed && i.timing_ms > getTimePassed()) i.isPlayed = false;
	}
	for (auto& i : placed_notes) {
		if (i.hit_offset_ms.has_value() && i.timing_ms > getTimePassed()) i.hit_offset_ms.reset();
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

		int id = currentState.beatId + wheel;

		if (id < 0) id = 0;
		if (id > calculated_beats.size() - 1) id = calculated_beats.size() - 1;
		int closestTiming = calculated_beats[id].timing_ms;

		this->setTime(closestTiming);
		if (isMusicValidLocal) {
			SeekMusicStream(_music, (closestTiming + offset) / 1000.0f);
		}
		flushPlayedBeats();
		onUpdateTimeline();
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

void Editor::update() {
	this->tick();
	this->listen();
	this->render();

	if (!isPaused()) {
		if (isMusicValidLocal) {
			UpdateMusicStream(_music);
		}
	}
	//prob too much for cycling
	int beatId = BeatFactory::findClosestBeatIndex(calculated_beats, getTimePassed());

	int sectionId = -1;

	for (int i = 0; i < sections.size(); i++) {
		if (getTimePassed() >= sections[i].start_ms && getTimePassed() <= sections[i].end_ms) {
			sectionId = i;
			break;
		}
	}

	for (auto& i : placed_notes) {
		int now = getTimePassed();
		if (i.timing_ms <= now && !i.hit_offset_ms.has_value()) {
			PlaySound(_sound);
			i.hit_offset_ms = now;
		}
	}

	this->currentState = { sectionId, beatId };
}

void Editor::handleGuiEvent(const GuiEvent& e) {
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
		onUpdateTimeline();
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
		onUpdateTimeline();
		break;
	}

	case GuiEventType::Load:
		break;

	case GuiEventType::Save:
		break;

	case GuiEventType::Skip: {
		float frac = std::stof(e.payload);

		int totalSkipLengthMs = sections[sections.size() - 1].end_ms;

		int ms = int(std::ceil(((float)totalSkipLengthMs / 100.0f) * frac));
		
		size_t id = BeatFactory::findClosestBeatIndex(calculated_beats, ms);

		int closestTiming = calculated_beats[id].timing_ms;

		setTime(closestTiming);
		if (isMusicValidLocal) {
			SeekMusicStream(_music, (closestTiming + offset) / 1000.0f);
		}

		flushPlayedBeats();
		onUpdateTimeline();
		break;
	}

	case GuiEventType::Offset: {
		int v = std::stoi(e.payload);
		this->offset = v;

		onUpdateTimeline();

		break;
	}

	case GuiEventType::Bpm: {
		if (currentState.sectionId == -1 || !sections.size()) break;

		sections[currentState.sectionId].bpm = std::stof(e.payload);

		onUpdateTimeline();

		break;
	}

	case GuiEventType::Signature: {
		if (currentState.sectionId == -1 || !sections.size()) break;
		//?
		sections[currentState.sectionId].signature.denominator = 4;
		sections[currentState.sectionId].signature.numerator = std::stoi(e.payload);

		onUpdateTimeline();

		break;
	}

	case GuiEventType::Spacing: {
		float fr = std::stof(e.payload);
		this->_spacingMul = fr;
		break;
	}

	case GuiEventType::InsertSection: {
		int now = getTimePassed();
		float end = songLengthMs > 0 ? songLengthMs : DEFAULT_SECTION_LENGTH;

		if (currentState.sectionId != -1) {
			sections[currentState.sectionId].end_ms = now;
		}
		
		//set default to current
		Section newSec;
		newSec.signature = { 4,4,"4/4" };
		newSec.bpm = DEFAULT_BPM;
		newSec.start_ms = now;
		newSec.end_ms = end;

		sections.push_back(newSec);
		
		std::sort(sections.begin(), sections.end(), [](const auto& lhs, const auto& rhs)
			{
				return lhs.start_ms < rhs.start_ms;
			});

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

	case GuiEventType::InsertNote: {
		int now = getTimePassed();
		bool exists = false;
		for (auto& i : placed_notes) {
			if (i.timing_ms == now) {
				exists = true;
				break;
			}
		}
		if (!exists) {
			placed_notes.push_back({ now, 0 });
		}
		

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

				songLengthMs = GetMusicTimeLength(_music) * 1000;

				sections[sections.size() - 1].end_ms = songLengthMs; //?

				onUpdateTimeline();
			}

			break;
		}

	}


	}

}

