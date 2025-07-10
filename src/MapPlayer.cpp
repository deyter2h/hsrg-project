#include "MapPlayer.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Settings.h"
#include <numeric>

//CURRENTLY IT IS ALLOWED TO RELEASE A LONG NOTE (SUCCESS), IF IT WAS HIT INCORRECTLY

MapPlayer::MapPlayer() {
    loadFromPath("");
    startTime = std::chrono::steady_clock::now();
}
MapPlayer::~MapPlayer() {}

bool MapPlayer::loadFromPath(const std::string& path) {
   // Music m = LoadMusicStream("resources/audio.mp3");
    /*this->beatmap.addSection({ Signature::ONE_FOUR, 220, 0, 5000 });

    auto timings = this->beatmap.getTimings();

    for (int i = 0; i < timings.size(); i++) {
        beatmap.placeNote(timings, i, i);
    }*/

    beatmap.readFrom("map.txt");

    loadedNotes = this->beatmap.getEntries();

    

    return true;
}

void MapPlayer::press() {
    if (renderedNotes.empty()) return;
    Note& currentNote = renderedNotes.front();
    if (currentNote.hit_offset_ms.has_value()) return; //??? Эта нота уже нажата
    if (currentNote.timing_ms - timePassedMs >= Settings::NOTE_HIT_WINDOW_MS) { onInvalidPress();  return; } //Нажали когда нота далеко - нет штрафа
    if (timePassedMs - Settings::NOTE_HIT_WINDOW_MS >= currentNote.timing_ms) {   //Нажали сильно позже начала/во время длинной ноты - штраф
        currentNote.hit_offset_ms = timePassedMs - currentNote.timing_ms; 
        onNoteMiss(currentNote);
        return;
    }
    if (std::abs(timePassedMs - currentNote.timing_ms) >= Settings::NOTE_HIT_WINDOW_GOOD_MS) { //Нажали не в тайминг - штраф
        currentNote.hit_offset_ms = timePassedMs - currentNote.timing_ms;
        onNoteMiss(currentNote);
        return;
    };
    onNoteHit(currentNote);
    currentNote.hit_offset_ms = timePassedMs - currentNote.timing_ms;
}

void MapPlayer::release() {
    if (renderedNotes.empty()) return;
    Note& currentNote = renderedNotes.front();
    if (currentNote.release_offset_ms.has_value()) return; //Эта нота уже зажата
    if (!currentNote.hit_offset_ms.has_value()) return; //Эта нота не зажималась
    if (!currentNote.length_ms) return; //Обычная нота - не нужно отпускать в тайминг
    if (std::abs(timePassedMs - currentNote.timing_ms - currentNote.length_ms) >= Settings::NOTE_HIT_WINDOW_GOOD_MS) { //Отпустили не в тайминг
        currentNote.release_offset_ms = timePassedMs - (currentNote.timing_ms + currentNote.length_ms);
        onNoteMiss(currentNote);
        return;
    }
    onNoteHit(currentNote);
    currentNote.release_offset_ms = timePassedMs - (currentNote.timing_ms + currentNote.length_ms);
}

void MapPlayer::tick() {
    auto now = std::chrono::steady_clock::now();
    float realDelta = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    startTime = now;
    timePassedMs += int(realDelta);

    //on add
    while (nextNoteIndex < loadedNotes.size() &&
        loadedNotes[nextNoteIndex].timing_ms < timePassedMs + 15000) //hardcoded
    {
        renderedNotes.push_back(loadedNotes[nextNoteIndex]);
        nextNoteIndex++;
    }

    //on delete
    while (!renderedNotes.empty() &&
        renderedNotes.front().timing_ms + Settings::NOTE_HIT_WINDOW_GOOD_MS + renderedNotes.front().length_ms < timePassedMs)
    {   
        onNoteMiss(renderedNotes.front());
        renderedNotes.pop_front();
    }
}

void MapPlayer::updateControls() {
   
    for (size_t i = 39; i <= 96; i++) {
        if (IsKeyPressed(i)) {
            keysDown.insert(i);
            this->press();
            
        }
    }

    for (auto it = keysDown.begin(); it != keysDown.end(); ) {
        if (IsKeyReleased(*it)) {
            this->release();
            it = keysDown.erase(it);
        }
        else {
            ++it;
        }
    }
    
    if (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))
        approachRate += 0.1;
    if (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))
        approachRate -= 0.1;
}

void MapPlayer::updateInputFeedback() {
    if (renderedNotes.empty()) return;

    const Note& currentNote = renderedNotes.front();

    if (currentNote.hit_offset_ms.has_value()) {
        if (currentNote.length_ms == 0) {
            //Note
            if (std::abs(currentNote.hit_offset_ms.value()) <= Settings::NOTE_HIT_WINDOW_GOOD_MS) {
                // renderedNotes.pop_front();
            }
        }
        if (currentNote.length_ms != 0) {
            //LN
            if (std::abs(currentNote.hit_offset_ms.value()) <= Settings::NOTE_HIT_WINDOW_GOOD_MS) {
            }
            else {
                //hitFlash.trigger(BLUE, currentNote.hit_offset_ms.value());
            }
        }

    }

    if (currentNote.release_offset_ms.has_value()) {
        if (std::abs(currentNote.release_offset_ms.value()) <= Settings::NOTE_HIT_WINDOW_GOOD_MS) {
        }
        else {
        }
    }
}

void MapPlayer::drawNotes() {

    const float travelDistancePx = float(Settings::WINDOW_WIDTH - Settings::HIT_WIDTH);

    for (auto& entry : renderedNotes) {
        const auto& n = entry;
        float headDelta = (n.timing_ms - timePassedMs) * approachRate;
        float tailDelta = (n.timing_ms + n.length_ms - timePassedMs) * approachRate;

        if (headDelta < 0) headDelta = 0;
        
        int startX = Settings::HIT_WIDTH + int((headDelta / Settings::NOTE_PREVIEW_TIME_MS) * travelDistancePx);
        int endX = Settings::HIT_WIDTH + int((tailDelta / Settings::NOTE_PREVIEW_TIME_MS) * travelDistancePx);

        DrawRectangle(startX, 0, std::max(endX - startX, Settings::BASE_NOTE_WIDTH), Settings::NOTE_HEIGHT, BLACK);
    }

}

void MapPlayer::drawInput() {
    size_t N = keysDown.size();
    if (N == 0) return;

    // full width of the input zone (just left of the hit line)
    int totalW = Settings::HIT_WIDTH - 1;
    // gap between slots
    int gap = 2;
    // padding inside each slot
    int pad = 4;
    // font size for key names
    int fontSize = 20;

    // compute each slot’s outer width
    int sliceW = std::max(1, (totalW - int((N - 1) * gap)) / int(N));
    // compute inner height (window minus vertical padding)
    int sliceH = Settings::WINDOW_HEIGHT - 2 * pad;

    int idx = 0;
    for (size_t keycode : keysDown) {
        // outer slot origin
        int outerX = idx * (sliceW + gap);
        int outerY = 0;

        // inset by pad on all sides
        Rectangle r;
        r.x = outerX + pad;
        r.y = outerY + pad;
        r.width = sliceW - 2 * pad;
        r.height = sliceH;

        // draw rounded rectangle
        DrawRectangleRounded(r, 0.2f, 4, inputColor);

        // lookup key name, measure, and draw centered
        const char* name = GetKeyName(int(keycode));
        int textW = MeasureText(name, fontSize);
        int textH = fontSize; // raylib uses fontSize as line height
        int textX = r.x + (r.width - textW) / 2;
        int textY = r.y + (r.height - textH) / 2;
        DrawText(name, textX, textY, fontSize, WHITE);

        idx++;
    }
}

void MapPlayer::updateAndDraw() {
    tick();
    updateControls();
    updateInputFeedback();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawLine(Settings::HIT_WIDTH, 0, Settings::HIT_WIDTH, Settings::WINDOW_WIDTH, BLUE);

    drawNotes();
    drawInput();

    const std::string title = "Time: " + std::to_string(timePassedMs) + " AR: " + std::to_string(approachRate);
    SetWindowTitle(title.c_str());

    EndDrawing();
}

//Handlers
void MapPlayer::onInvalidPress() {
    inputColor = GRAY;
}

void MapPlayer::onNoteMiss(Note& note) {
    inputColor = RED;
}

void MapPlayer::onNoteHit(Note& note) {
    inputColor = GREEN;
}

void MapPlayer::onLongNoteEndMiss(Note& note) {
    inputColor = RED;
}

void MapPlayer::onLongNoteEndHit(Note& note) {
    inputColor = YELLOW;
}