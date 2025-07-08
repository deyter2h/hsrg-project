#include "MapPlayer.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Settings.h"

//CURRENTLY IT IS ALLOWED TO RELEASE A LONG NOTE (SUCCESS), IF IT WAS HIT INCORRECTLY

MapPlayer::MapPlayer() {
    loadFromPath("");
    startTime = std::chrono::steady_clock::now();
    Sound hitSound = LoadSound("resources/hit.wav");
}
MapPlayer::~MapPlayer() {}

bool MapPlayer::loadFromPath(const std::string& path) {
    this->beatmap.load(360, "data/audio.mp3");
    beatmap.placeNote(Signature::ONE_FOUR, 30, 30);
    beatmap.placeNote(Signature::ONE_FOUR, 35, 50);

    loadedNotes = this->beatmap.getEntries();

    return true;
}

void MapPlayer::press() {
    if (renderedNotes.empty()) return;
    NoteEntry& currentNote = renderedNotes.front();
    if (currentNote.note.hit_offset_ms.has_value()) return; //Эта нота уже нажата
    if (currentNote.note.timing_ms - timePassedMs >= Settings::NOTE_HIT_WINDOW_MS) { return; } //Нажали когда нота далеко - нет штрафа
    if (timePassedMs - Settings::NOTE_HIT_WINDOW_MS >= currentNote.note.timing_ms) {   //Нажали сильно позже начала/во время длинной ноты - штраф
        hitFlash.trigger(RED, timePassedMs - currentNote.note.timing_ms);
        currentNote.note.hit_offset_ms = timePassedMs - currentNote.note.timing_ms; 
        return;
    }
    if (std::abs(timePassedMs - currentNote.note.timing_ms) >= Settings::NOTE_HIT_WINDOW_GOOD_MS) { //Нажали не в тайминг - штраф
        hitFlash.trigger(RED, timePassedMs - currentNote.note.timing_ms);
        currentNote.note.hit_offset_ms = timePassedMs - currentNote.note.timing_ms;
        return;
    };

    currentNote.note.hit_offset_ms = timePassedMs - currentNote.note.timing_ms;
}

void MapPlayer::release() {
    if (renderedNotes.empty()) return;
    NoteEntry& currentNote = renderedNotes.front();
    if (currentNote.note.release_offset_ms.has_value()) return; //Эта нота уже зажата
    if (!currentNote.note.hit_offset_ms.has_value()) return; //Эта нота не зажималась
    if (!currentNote.note.length_ms) return; //Обычная нота - не нужно отпускать в тайминг
    if (std::abs(timePassedMs - currentNote.note.timing_ms - currentNote.note.length_ms) >= Settings::NOTE_HIT_WINDOW_GOOD_MS) { //Отпустили не в тайминг
        hitFlash.trigger(RED, timePassedMs - currentNote.note.timing_ms);
        currentNote.note.release_offset_ms = timePassedMs - (currentNote.note.timing_ms + currentNote.note.length_ms);
        return;
    }

    currentNote.note.release_offset_ms = timePassedMs - (currentNote.note.timing_ms + currentNote.note.length_ms);
}

void MapPlayer::tick() {
    auto now = std::chrono::steady_clock::now();
    float realDelta = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    startTime = now;
    timePassedMs += int(realDelta);

    //on add
    while (nextNoteIndex < loadedNotes.size() &&
        loadedNotes[nextNoteIndex].note.timing_ms < timePassedMs + 15000) //hardcoded
    {
        renderedNotes.push_back(loadedNotes[nextNoteIndex]);
        nextNoteIndex++;
    }

    //on delete
    while (!renderedNotes.empty() &&
        renderedNotes.front().note.timing_ms + Settings::NOTE_HIT_WINDOW_GOOD_MS + renderedNotes.front().note.length_ms < timePassedMs)
    {
        renderedNotes.pop_front();
    }
    
}

void MapPlayer::updateControls() {
    if (IsKeyPressed(KEY_X)) this->press();
    if (IsKeyReleased(KEY_X)) this->release();

    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
        approachRate += 0.1;
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
        approachRate -= 0.1;
}

void MapPlayer::updateInputFeedback() {
    if (renderedNotes.empty()) return;

    const NoteEntry& currentNote = renderedNotes.front();

    if (currentNote.note.hit_offset_ms.has_value()) {
        if (currentNote.note.length_ms == 0) {
            //Note
            if (std::abs(currentNote.note.hit_offset_ms.value()) <= Settings::NOTE_HIT_WINDOW_GOOD_MS) {
                hitFlash.trigger(GREEN, currentNote.note.hit_offset_ms.value());
                // renderedNotes.pop_front();
            }
        }
        if (currentNote.note.length_ms != 0) {
            //LN
            if (std::abs(currentNote.note.hit_offset_ms.value()) <= Settings::NOTE_HIT_WINDOW_GOOD_MS) {
                hitFlash.trigger(GREEN, currentNote.note.hit_offset_ms.value());
            }
            else {
                //hitFlash.trigger(BLUE, currentNote.hit_offset_ms.value());
            }
        }

    }

    if (currentNote.note.release_offset_ms.has_value()) {
        if (std::abs(currentNote.note.release_offset_ms.value()) <= Settings::NOTE_HIT_WINDOW_GOOD_MS) {
            hitFlash.trigger(YELLOW, currentNote.note.hit_offset_ms.value());
        }
        else {
            hitFlash.trigger(RED, currentNote.note.hit_offset_ms.value());
        }
    }
}

void MapPlayer::drawNotes() {

    const float travelDistancePx = float(GetScreenWidth() - Settings::HIT_WIDTH);

    for (auto& entry : renderedNotes) {
        const auto& n = entry.note;
        float headDelta = (n.timing_ms - timePassedMs) * approachRate;
        float tailDelta = (n.timing_ms + n.length_ms - timePassedMs) * approachRate;

        if (headDelta < 0) headDelta = 0;
        
        int startX = Settings::HIT_WIDTH + int((headDelta / Settings::NOTE_PREVIEW_TIME_MS) * travelDistancePx);
        int endX = Settings::HIT_WIDTH + int((tailDelta / Settings::NOTE_PREVIEW_TIME_MS) * travelDistancePx);

        DrawRectangle(startX, 0, std::max(endX - startX, Settings::BASE_NOTE_WIDTH), Settings::NOTE_HEIGHT, BLACK);
    }

}

void MapPlayer::updateAndDraw() {
    tick();
    updateControls();
    updateInputFeedback();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    hitFlash.drawZone(Settings::HIT_WIDTH, GetScreenHeight());
    hitFlash.drawOffset(0, Settings::HIT_WIDTH / 2, 20);

    DrawLine(Settings::HIT_WIDTH, 0, Settings::HIT_WIDTH, GetScreenHeight(), BLUE);

    drawNotes();

    const std::string title = "Time: " + std::to_string(timePassedMs) + " AR: " + std::to_string(approachRate);
    SetWindowTitle(title.c_str());

    EndDrawing();
}