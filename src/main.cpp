//#include "raylib.h"
//#include <algorithm>
//#include "MapPlayer.h"
//#include "Settings.h"
//
//int main() {
//    InitAudioDevice();
//    InitWindow(800, Settings::WINDOW_HEIGHT, "HSRG");
//    SetTargetFPS(60);
//
//    MapPlayer player;
//    while (!WindowShouldClose()) {
//        player.updateAndDraw();
//    }
//
//    CloseWindow();
//    return 0;
//}
//

// map_editor_ui.cpp
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <raymath.h>
#include <string>
//#include "Beatmap.h"
#include <cmath>
#include <algorithm>
#include "./Timable.h"
#include <iostream>

static const int   SCREEN_WIDTH = 1024;
static const int   SCREEN_HEIGHT = 600;
static const Rectangle TIMELINE_AREA = { 20, 80, SCREEN_WIDTH - 40, 100 };
static const float PIXELS_PER_MS = 0.5f;
static const Color UI_BG_COLOR = RAYWHITE;
static const Color UI_TIMELINE_BG = LIGHTGRAY;
static const Color UI_BAND_COLOR_A = Fade(GRAY, 0.3f);
static const Color UI_BAND_COLOR_B = Fade(GRAY, 0.5f);
static const Color UI_LINE_COLOR = DARKGRAY;
static const Color UI_DOWNBEAT_COLOR = RED;
static const Color UI_BEAT_COLOR = DARKGRAY;
static const int   UI_FONT_SIZE_LABEL = 14;
static const int   UI_FONT_SIZE_BEAT = 12;
static const int   UI_TICK_HEIGHT_MAIN = 20;
static const int   UI_TICK_HEIGHT_SUB = 10;
static const float UI_SCROLL_SPEED = 20.0f;

static const char* SigToString(Signature s) {
    switch (s) {
    case Signature::ONE_FOUR:  return "4/4";
    case Signature::ONE_THREE: return "3/4";
    default:                   return "?";
    }
}

class Editor : public Timeable {
public:
    Editor() = default;
    void onBeat() override {};

private:

};

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rhythm Map Editor");
    SetTargetFPS(60);

    float timelineOffset = 0.0f;
    bool  showLoadDialog = false;
    bool  showAddSection = false;
    int   speedSelIndex = 0;
    const char* speedOptions[] = { "0.5x", "1x", "1.5x", "2x" };
    const int   speedCount = 4;

    Editor ed = Editor();
   // ed.pause();

    //BeatmapConstructor bm;
    //bm.readFrom("map.txt");
    //auto sections = bm.getSections();
    //int  songLengthMs = bm.getSongLengthMs();

    /*std::vector<float> allBeats;
    for (size_t si = 0; si < sections.size(); ++si) {
        auto& sec = sections[si];
        int  start = sec.timingMs + sec.offsetMs;
        int  end = (si + 1 < sections.size()) ? sections[si + 1].timingMs : songLengthMs;
        int  division = (sec.signature == Signature::ONE_THREE) ? 3 : 4;
        double beatMs = 60000.0 / sec.bpm / division;
        int    count = int(std::ceil((end - start) / beatMs));
        for (int bi = 0; bi <= count; ++bi) {
            allBeats.push_back(start + bi * float(beatMs));
        }
    }
    std::sort(allBeats.begin(), allBeats.end());*/
    std::vector<Beat> beats = ed.getCurrentBeats(ed.getTimePassed(), 3000);
    while (!WindowShouldClose()) {
        
        float wheel = GetMouseWheelMove();

        if (IsKeyPressed(KEY_SPACE)) {
            if (ed.isPaused()) ed.resume();
            else ed.pause();
        }
        
        if (wheel != 0) {
            ed.incrementTime(wheel * (int)ed.getBeatTime());
            beats = ed.getCurrentBeats(ed.getTimePassed(), 3000);
        }

        if (beats.back().timing_ms <= ed.getTimePassed()) {
            beats = ed.getCurrentBeats(ed.getTimePassed(), 3000);
        }

        BeginDrawing();
        ClearBackground(UI_BG_COLOR);

        DrawText(
            std::to_string(ed.getTimePassed()).c_str(),
            SCREEN_WIDTH / 2, TIMELINE_AREA.y - 30, UI_FONT_SIZE_LABEL, UI_LINE_COLOR
        );

        if (GuiButton({ 20, 20, 120, 30 }, "Load Map")) {
            showLoadDialog = true;
        }
        if (GuiButton({ 160, 20, 180, 30 }, "Add Section")) {
            showAddSection = true;
        }
        /*speedSelIndex = GuiComboBox(
            { 360, 20, 120, 30 }, 0, 0
        );*/

        DrawRectangleRec(TIMELINE_AREA, UI_TIMELINE_BG);

        // draw blue vertical playhead line
        /*float playX = TIMELINE_AREA.x + TIMELINE_AREA.width * 0.5f;
        DrawLineV({ playX, (float)TIMELINE_AREA.y },
            { playX, (float)(TIMELINE_AREA.y + TIMELINE_AREA.height) },
            BLUE);*/

        GuiEnable();
        BeginScissorMode(
            (int)TIMELINE_AREA.x, (int)TIMELINE_AREA.y,
            (int)TIMELINE_AREA.width, (int)TIMELINE_AREA.height + 100
        );

        int yLine = TIMELINE_AREA.y + TIMELINE_AREA.height / 2;

        /*for (size_t i = 0; i < sections.size(); ++i) {
            auto& sec = sections[i];
            int  start = sec.timingMs;
            int  end = (i + 1 < sections.size())
                ? sections[i + 1].timingMs
                : songLengthMs;
            float x1 = TIMELINE_AREA.x + start * PIXELS_PER_MS - timelineOffset;
            float x2 = TIMELINE_AREA.x + end * PIXELS_PER_MS - timelineOffset;
            Rectangle band = { x1, TIMELINE_AREA.y + 100, x2 - x1, TIMELINE_AREA.height };
            DrawRectangleRec(band, (i % 2 == 0) ? UI_BAND_COLOR_A : UI_BAND_COLOR_B);

            std::string label = TextFormat(
                "Time: %d\nOffset: %d\nBPM: %zu\nSig: %s",
                sec.timingMs, sec.offsetMs, sec.bpm, SigToString(sec.signature)
            );
            DrawText(
                label.c_str(),
                int(x1), int(band.y + 5),
                UI_FONT_SIZE_LABEL, UI_LINE_COLOR
            );
        }*/

        /*DrawLine(
            (int)TIMELINE_AREA.x, yLine,
            (int)(TIMELINE_AREA.x + TIMELINE_AREA.width), yLine,
            UI_LINE_COLOR
        );*/

        for (int bi = 0; bi < beats.size(); bi ++) {
            int  division = (beats[bi].signature == Signature::ONE_THREE) ? 3 : 4;
            int tickH = (bi % division == 0) ? UI_TICK_HEIGHT_MAIN : UI_TICK_HEIGHT_SUB;
            float x = (beats[bi].timing_ms * PIXELS_PER_MS) - ed.getTimePassed() * PIXELS_PER_MS;
            DrawLine(int(x), yLine - tickH, int(x), yLine + tickH, UI_LINE_COLOR);
        }


       /* for (size_t si = 0; si < sections.size(); ++si) {
            auto& sec = sections[si];
            int  start = sec.timingMs + sec.offsetMs;
            int  end = (si + 1 < sections.size())
                ? sections[si + 1].timingMs
                : songLengthMs;
            int  division = (sec.signature == Signature::ONE_THREE) ? 3 : 4;
            double beatMs = 60000.0 / sec.bpm / division;
            float  beatPx = float(beatMs * pxPerMs);
            int    firstIdx = std::max(0, int(std::floor((timelineOffset / pxPerMs - start) / beatMs)));
            int    lastIdx = std::max(0, int(std::ceil(((timelineOffset + TIMELINE_AREA.width) / pxPerMs - start) / beatMs)));

            for (int bi = firstIdx; bi <= lastIdx; ++bi) {
                float worldMs = start + bi * float(beatMs);
                if (worldMs < start || worldMs > end) continue;
                float x = TIMELINE_AREA.x + worldMs * pxPerMs - timelineOffset;
                int tickH = (bi % division == 0) ? UI_TICK_HEIGHT_MAIN : UI_TICK_HEIGHT_SUB;
                

                int   beatNum = (bi % division) + 1;
                Color col = (beatNum == 1) ? UI_DOWNBEAT_COLOR : UI_BEAT_COLOR;
                const char* txt = TextFormat("%d", beatNum);
                int txtW = MeasureText(txt, UI_FONT_SIZE_BEAT);
                DrawText(
                    txt,
                    int(x - txtW / 2),
                    yLine - tickH - UI_FONT_SIZE_BEAT - 2,
                    UI_FONT_SIZE_BEAT,
                    col
                );
            }
        }*/

        EndScissorMode();
        GuiDisable();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}