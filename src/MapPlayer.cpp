// MapPlayer.cpp
#include "MapPlayer.h"
#include <algorithm>
#include <cmath>
#include <iostream>

//CURRENTLY IT IS ALLOWED TO RELEASE A LONG NOTE (SUCCESS), IF IT WAS HIT INCORRECTLY

int previewWindowMs = 2000;
constexpr int NOTE_HIT_WINDOW_GOOD_MS = 50;
constexpr int NOTE_HIT_WINDOW_MS = 150;

MapPlayer::MapPlayer() {
    loadFromPath("");
    startTime = std::chrono::steady_clock::now();
    Sound hitSound = LoadSound("resources/hit.wav");
}
MapPlayer::~MapPlayer() {}

bool MapPlayer::loadFromPath(const std::string& path) {
    this->beatmap.load(360, "data/audio.mp3");

    for (int i = 0; i < 1000 / 4 ; i++) {
        this->beatmap.placeNote(ONE_FOUR, i * 4, i * 4);
    }

    this->loadedNotes = this->beatmap.getNotes();

    return true;
}

void MapPlayer::press() {
    if (renderedNotes.empty()) return;

    Note& currentNote = renderedNotes.front();

    if (currentNote.hit_offset_ms.has_value()) return; //Эта нота уже нажата

    if (currentNote.timing_ms - timePassedMs >= NOTE_HIT_WINDOW_MS) { return; } //Нажали когда нота далеко - нет штрафа

    if (timePassedMs - NOTE_HIT_WINDOW_MS >= currentNote.timing_ms) {   //Нажали сильно позже начала/во время длинной ноты - штраф
        hitFlash.trigger(RED, timePassedMs - currentNote.timing_ms);
        currentNote.hit_offset_ms = timePassedMs - currentNote.timing_ms; 
        return;
    }

    if (std::abs(timePassedMs - currentNote.timing_ms) >= NOTE_HIT_WINDOW_GOOD_MS) { //Нажали не в тайминг - штраф
        hitFlash.trigger(RED, timePassedMs - currentNote.timing_ms);
        currentNote.hit_offset_ms = timePassedMs - currentNote.timing_ms;
        return;
    };

    currentNote.hit_offset_ms = timePassedMs - currentNote.timing_ms;

    //hitFlash.trigger(GREEN, timePassedMs - currentNote.timing_ms);

    if (!currentNote.length_ms) { //Попали в обычную ноту - удалим TMP
       // renderedNotes.pop_front();
    }
}

void MapPlayer::release() {
    if (renderedNotes.empty()) return;

    Note& currentNote = renderedNotes.front();

    if (currentNote.release_offset_ms.has_value()) return; //Эта нота уже зажата

    if (!currentNote.hit_offset_ms.has_value()) return; //Эта нота не зажималась

    if (!currentNote.length_ms) return; //Обычная нота - не нужно отпускать в тайминг

    if (std::abs(timePassedMs - currentNote.timing_ms - currentNote.length_ms) >= NOTE_HIT_WINDOW_GOOD_MS) { //Отпустили не в тайминг
        hitFlash.trigger(RED, timePassedMs - currentNote.timing_ms);
        currentNote.release_offset_ms = timePassedMs - (currentNote.timing_ms + currentNote.length_ms); 
        return;
    }

    currentNote.release_offset_ms = timePassedMs - (currentNote.timing_ms + currentNote.length_ms);

    //hitFlash.trigger(GREEN, timePassedMs - (currentNote.timing_ms + currentNote.length_ms));

    //renderedNotes.pop_front(); //Отжали в тайминг длинную ноту - удалим TMP
}

void MapPlayer::tick() {
    auto now = std::chrono::steady_clock::now();
    float realDelta = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    startTime = now;
    timePassedMs += int(realDelta);

    // 1) ДОБАВЛЯЕМ НОВЫЕ ноты, начиная с nextNoteIndex
    while (nextNoteIndex < loadedNotes.size() &&
        loadedNotes[nextNoteIndex].timing_ms < timePassedMs + 15000) //hardcoded
    {
        renderedNotes.push_back(loadedNotes[nextNoteIndex]);
        nextNoteIndex++;
    }

    while (!renderedNotes.empty() &&
        renderedNotes.front().timing_ms + NOTE_HIT_WINDOW_GOOD_MS + renderedNotes.front().length_ms < timePassedMs)
    {
        //hitFlash.trigger(RED, timePassedMs - renderedNotes.front().timing_ms);
        renderedNotes.pop_front();
    }

    if (IsKeyPressed(KEY_X)) {
        this->press();
    }
    if (IsKeyReleased(KEY_X)) {
        this->release();
    }
}

void MapPlayer::drawNotes() {

    for (auto& n : renderedNotes) {
        const float previewMs = float(previewWindowMs);
        const float scrollW = float(GetScreenWidth() - hitX);
        int screenH = GetScreenHeight();
        const int noteWidth = 8;  // только для tap-нот

        // 1) позиция головы
        float headDelta = (n.timing_ms - timePassedMs) * speedMultiplier;
        if (headDelta < 0) headDelta = 0;
        int xH = hitX + int((headDelta / previewMs) * scrollW + 0.5f);

        if (n.length_ms == 0) {
            // TAP-нота (как раньше)
            if (timePassedMs < n.timing_ms) {
                DrawRectangle(xH - noteWidth / 2, 0, noteWidth, screenH, BLACK);
            }
            else {
                int tailDist = (n.timing_ms + NOTE_HIT_WINDOW_GOOD_MS) - timePassedMs;
                if (tailDist > 0) {
                    float ratio = float(tailDist) / float(NOTE_HIT_WINDOW_GOOD_MS);
                    int   h = int(screenH * ratio + 0.5f);
                    int   y = (screenH - h) / 2;
                    DrawRectangle(xH - noteWidth / 2, y, noteWidth, h, BLACK);
                }
            }
        }
        else {
            // HOLD-нота: рассчитываем позицию хвоста
            float tailDelta = (n.timing_ms + n.length_ms - timePassedMs) * speedMultiplier;
            if (tailDelta < 0) tailDelta = 0;
            int xT = hitX + int((tailDelta / previewMs) * scrollW + 0.5f);

            // ширина = abs(xT - xH)
            int xs = std::min(xH, xT);
            int xe = std::max(xH, xT);
            int width = xe - xs;

            // рисуем прямоугольник нужной ширины и полной высоты
            DrawRectangle(xs, 0, width, screenH, BLACK);
        }
    }

    const Note& currentNote = renderedNotes.front();

    if (currentNote.hit_offset_ms.has_value()) {
        if (currentNote.length_ms == 0) {
            //Note
            if (std::abs(currentNote.hit_offset_ms.value()) <= NOTE_HIT_WINDOW_GOOD_MS) {
                hitFlash.trigger(GREEN, currentNote.hit_offset_ms.value());
               // renderedNotes.pop_front();
            }
        }
        if (currentNote.length_ms != 0) {
            //LN
            if (std::abs(currentNote.hit_offset_ms.value()) <= NOTE_HIT_WINDOW_GOOD_MS) {
                hitFlash.trigger(GREEN, currentNote.hit_offset_ms.value());
            }
            else {
                //hitFlash.trigger(BLUE, currentNote.hit_offset_ms.value());
            }
        }

    }

    if (currentNote.release_offset_ms.has_value()) {
        if (std::abs(currentNote.release_offset_ms.value()) <= NOTE_HIT_WINDOW_GOOD_MS) {
            hitFlash.trigger(YELLOW, currentNote.hit_offset_ms.value());
        }
        else {
            hitFlash.trigger(RED, currentNote.hit_offset_ms.value());
        }
    }

}

void MapPlayer::updateAndDraw() {
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
        speedMultiplier = std::min(speedMultiplier + step, maxSpeed);
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
        speedMultiplier = std::max(speedMultiplier - step, minSpeed);

    tick();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    hitFlash.drawZone(hitX, GetScreenHeight());
    hitFlash.drawOffset(0, hitX / 2, 20);

    DrawLine(hitX, 0, hitX, GetScreenHeight(), BLUE);

    const float scrollW = float(GetScreenWidth() - hitX);

    drawNotes();

    const std::string title = "Time: " + std::to_string(timePassedMs) + " Speed: " + std::to_string(speedMultiplier);
    SetWindowTitle(title.c_str());

    EndDrawing();
}