// MapPlayer.h
#pragma once
#include <vector>
#include <deque>
#include <string>
#include <chrono>
#include <set>
#include "Note.h"
#include "Flash.h"
#include "raylib.h"
#include "Beatmap.h"

class MapPlayer {
public:
    MapPlayer();
    ~MapPlayer();

    // »нтерфейс
    void updateAndDraw();

private:
    // загрузка нот
    BeatmapConstructor beatmap;

    bool loadFromPath(const std::string& path);

    // обновление логики
    void tick();
    void updateControls();
    void updateInputFeedback();
    void drawNotes();
    void drawInput(); //temp idk

    void onInvalidPress();
    void onNoteMiss(Note &note);
    void onNoteHit(Note& note);
    void onLongNoteEndMiss(Note& note);
    void onLongNoteEndHit(Note& note);

    // внутренн€€ структура
    std::deque<Note> loadedNotes;
    std::deque<Note> renderedNotes;
    size_t nextNoteIndex = 0;

    std::set<size_t> keysDown;

    int               timePassedMs = 0;
    std::chrono::steady_clock::time_point startTime;
    int               lastNoteId = -1;

    float approachRate = 0.5f;

    Color inputColor = GRAY;

    void press();
    void release();

    Sound hitSound;
};
