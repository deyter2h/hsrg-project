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

    // ���������
    void updateAndDraw();

private:
    // �������� ���
    Beatmap beatmap;

    bool loadFromPath(const std::string& path);

    // ���������� ������
    void tick();
    void updateControls();
    void updateInputFeedback();
    void drawNotes();

    // ���������� ���������
    std::deque<NoteEntry> loadedNotes;
    std::deque<NoteEntry> renderedNotes;
    size_t nextNoteIndex = 0;


    int               timePassedMs = 0;
    std::chrono::steady_clock::time_point startTime;
    int               lastNoteId = -1;

    float approachRate = 0.5f;

    Flash hitFlash;

    void press();
    void release();

    Sound hitSound;
};
