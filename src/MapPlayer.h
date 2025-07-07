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

    // Интерфейс
    void updateAndDraw();

private:
    // загрузка нот
    Beatmap beatmap;

    bool loadFromPath(const std::string& path);

    // обновление логики
    void tick();
    void drawNotes();

    // внутренняя структура
    std::deque<Note> loadedNotes;
    std::deque<Note> renderedNotes;
    size_t nextNoteIndex = 0;


    int               timePassedMs = 0;
    std::chrono::steady_clock::time_point startTime;
    int               lastNoteId = -1;

    // для скорости прокрутки
    float speedMultiplier = 0.5f;
    const float step = 0.1f;
    const float minSpeed = 0.1f;
    const float maxSpeed = 5.0f;

    // вспомогательные
    static constexpr int hitX = 125;

    Flash hitFlash;

    void press();
    void release();

    Sound hitSound;
};
