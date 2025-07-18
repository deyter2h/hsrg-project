#include "Editor.h"



//struct EditorNote : Note {
//    bool played = false;
//};
//
//class Editor : public Timeable {
//public:
//    Editor() {
//        
//        hitSound = LoadSound("C:/music/sound.wav");
//        
//    };
//
//    void update() {
//        tick();
//        _handleBeatSound(); //why even
//        handleInput();
//        render();
//    }
//
//private:
//    float timelineZoom = 1.0f;
//    int songOffset = 0;
//
//    int nextHitTiming = songOffset;
//
//    std::map<int, EditorNote> placedNotes;
//    Sound hitSound;
//
//    void _handleBeatSound() {
//        if (this->getTimePassed() >= nextHitTiming) {
//                  
//        }
//        for (auto& i : placedNotes) {
//            if (getTimePassed() >= i.first && !i.second.played) {
//                i.second.played = true;
//                PlaySound(hitSound);
//            }
//
//            if (getTimePassed() < i.first) {
//                i.second.played = false;
//            }
//        }
//    }
//
//    void handlePlacedNotes() {
//        float pxPerMs = PIXELS_PER_MS;
//        int   elapsed = getTimePassed();
//
//        for (auto& i : placedNotes) {
//            float x = TIMELINE_AREA.x
//                + (i.first - elapsed) * pxPerMs * timelineZoom;
//
//            DrawCircle(x, TIMELINE_AREA.y + TIMELINE_AREA.height / 2, 12, RED);
//        }
//    }
//
//    void handleInput() {
//        if (IsKeyPressed(KEY_SPACE)) {
//            isPaused() ? resume() : pause();
//        }
//
//        float wheel = GetMouseWheelMove();
//        if (wheel != 0.0f) {
//            incrementTime(int(wheel) * getBeatTime()); //skip on next beat, not this
//        }
//    }
//
//    void render() {
//        GuiEnable();
//        DrawText(
//            TextFormat("Time: %d ms", getTimePassed()),
//            SCREEN_WIDTH / 2 - 50, TIMELINE_AREA.y - 30,
//            UI_FONT_SIZE_LABEL, UI_LINE_COLOR
//        );
//        DrawRectangleRec(TIMELINE_AREA, UI_TIMELINE_BG);
//        
//        if (GuiButton({ 20,20,100,30 }, "Load Map")) { /*…*/ }
//        drawBpmSelection();
//        drawOffsetSelection();
//        drawSignatureSelection();
//        GuiSlider({ 580,20,100,30 }, "-", "+", &timelineZoom, 0.5f, 5.0f);
//        
//        GuiDisable();
//        drawTimeline();
//        handlePlacedNotes();
//    }
//
//    void drawBpmSelection() {
//        static bool  isActive = false;
//
//        int ret = GuiSpinner({ 160,20,100,30 }, "BPM", &this->bpm, 1, 1000, isActive); //fix ref borders
//
//        if (ret) {
//            isActive = !isActive;
//        }
//    }
//
//    void drawOffsetSelection() {
//        static bool  isActive = false;
//        //allow up to -song len + song len
//        int ret = GuiSpinner({ 300,20,100,30 }, "Offset", &this->songOffset, -999999, 999999, isActive); //fix ref borders
//
//        if (ret) {
//            isActive = !isActive;
//        }
//    }
//
//    void drawSignatureSelection() {
//        static bool  isActive = false;
//        static int   selIndex = 0; 
//
//        std::string sign_str = std::accumulate(
//            common_signatures.begin(), common_signatures.end(),
//            std::string{},
//            [](const std::string& acc, const SignatureInfo& el) {
//                return acc.empty()
//                    ? el.label
//                    : acc + ";" + el.label;
//            }
//        );
//     
//       int ret = GuiDropdownBox(
//            (Rectangle{
//            440, 20, 100, 30
//                }) ,
//           sign_str.c_str(),
//            &selIndex,
//           isActive
//        );
//
//       if (ret) {
//           isActive = !isActive;
//           setSignature(common_signatures[selIndex]);
//       } 
//       
//    }
//
//    void drawTimeline() {
//        GuiEnable();
//        BeginScissorMode(
//            int(TIMELINE_AREA.x), int(TIMELINE_AREA.y),
//            int(TIMELINE_AREA.width), int(TIMELINE_AREA.height) + 100
//        );
//
//        int centerY = TIMELINE_AREA.y + TIMELINE_AREA.height / 2;
//        auto beats = calculateBeats(8);
//
//        int  division = getSignatureDivision();
//        float pxPerMs = PIXELS_PER_MS;
//        int   elapsed = getTimePassed();
//
//        // Your new zoom factor
//        float zoom = timelineZoom;  // could be changed at runtime
//
//        // Precompute all beat X positions with zoom
//        std::vector<float> xs;
//        xs.reserve(beats.size());
//        for (auto& b : beats) {
//            float x = TIMELINE_AREA.x
//                + (b.timing_ms - elapsed) * pxPerMs * zoom;
//            xs.push_back(x); //offset goes not here prob
//        }
//
//        Vector2 mp = GetMousePosition();
//
//        // Highlight hovered segment
//        for (size_t i = 0; i + 1 < xs.size(); ++i) {
//            const int width = xs[i + 1] - xs[i];
//            Rectangle slice{
//                xs[i] - width/2,
//                TIMELINE_AREA.y,
//                width,
//                TIMELINE_AREA.height
//            };
//            if (CheckCollisionPointRec(mp, slice)) {
//                DrawRectangleRec(slice, Fade(UI_LINE_COLOR, 0.1f));
//                DrawText(std::to_string(beats[i].timing_ms).c_str(), xs[i] + (xs[i + 1] - xs[i])/2,
//                    TIMELINE_AREA.y + TIMELINE_AREA.height - 12, 12, BLACK);
//
//                if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
//                    placedNotes.insert({ beats[i].timing_ms, { beats[i].timing_ms, 0 } });
//                }
//
//                if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_RIGHT)) {
//                    placedNotes.erase(beats[i].timing_ms);
//                }
//
//                break;
//            }
//        }
//
//        handlePlacedNotes();
//
//               // Draw ticks and labels
//        for (size_t i = 0; i < xs.size(); ++i) {
//            bool isDown = (i % division) == 0;
//            int  tickH = isDown ? UI_TICK_HEIGHT_MAIN : UI_TICK_HEIGHT_SUB;
//            Color col = isDown ? UI_DOWNBEAT_COLOR : UI_LINE_COLOR;
//
//            float x = xs[i];
//            DrawLine(int(x), centerY - tickH, int(x), centerY + tickH, col);
//
//            const char* txt = TextFormat("%d", beats[i].index + 1);
//            int txtW = MeasureText(txt, UI_FONT_SIZE_BEAT);
//            DrawText(
//                txt,
//                int(x - txtW / 2),
//                centerY + UI_TICK_HEIGHT_MAIN + 2,
//                UI_FONT_SIZE_BEAT,
//                UI_BEAT_COLOR
//            );
//        }
//
//        EndScissorMode();
//        GuiDisable();
//    }
//};

int main() {
    InitAudioDevice();
    InitWindow(800, 600, "R");
    SetTargetFPS(60);

    Editor ed = Editor();
    ed.pause();
    
    while (!WindowShouldClose()) {
        ed.update();
    }

    CloseWindow();
    return 0;
}