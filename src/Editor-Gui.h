#pragma once

#include <vector>
#include <string>

#include "raylib.h"
#include "raygui.h"

#include "SectionSelection.h"
#include "Constants.h"

//const Rectangle TIMELINE_AREA = { 50, 50, 600, 100 };

static constexpr Rectangle TIMELINE_AREA = { 72, 64, 800, 128 };
static constexpr Rectangle GLOBAL_PANEL = { 32, 184, 200, 264 };
static constexpr Rectangle SECTION_PANEL = { 248, 184, 200, 264 };
static constexpr Rectangle VIEW_PANEL = { 464, 184, 200, 264 };

const float PIXEL_PER_MS = 0.5f;

enum class GuiEventType {
    Play,
    Restart,
    MP3,
    Load,
    Save,
    Skip,
    PlaybackSpeed,
    Offset,
    InsertSection,
    RemoveSection,
    JumpSection,
    Bpm,
    Signature,
    Spacing,
    InsertNoteStart,
    InsertNoteEnd,
    RemoveNote,
    // …add more as you grow…
};

struct GuiEvent {
    GuiEventType type;
    std::string payload;
};


class EditorGui {
private:

public:
    float timelineVal = 0.0f; //(((
    int bpmVal = DEFAULT_BPM;
    int offsetVal = 0;
    int selectedSignature = 0;
	EditorGui() = default;

    std::vector<GuiEvent> draw_debug()
    {
        std::vector<GuiEvent> events;

        // ──────────────────────────────────────────────────────────────────────────────────────
        // Static UI state
        // ──────────────────────────────────────────────────────────────────────────────────────

        // Buttons
        static bool btnLoadSong = false;
        static bool btnLoad = false;
        static bool btnSave = false;
        static bool btnRestart = false;
        static bool btnPlay = false;
        static bool btnInsertSec = false;
        static bool btnRemoveSec = false;
        static bool btnInsertNote = false;
        static bool btnRemoveNote = false;

        // Sliders & their last‐frame values
        static float spacingVal = 0.2f, lastSpacingVal = 0.f;

        // Spinners & their last‐frame values
        static int lastOffsetVal = 0;
        static int lastBpmVal = 0;

        // Dropdowns & last‐frame values
        static bool  speedOpen = false;
        static int   speedSel = 2, lastSpeedSel = 0;
        static bool  jumpOpen = false;
        static int   jumpSel = 0, lastJumpSel = 0;
        static bool  sigOpen = false;
        static int lastSigSel = 0;

        // ──────────────────────────────────────────────────────────────────────────────────────
        // Draw controls
        // ──────────────────────────────────────────────────────────────────────────────────────

        GuiEnable();

        // Top‑left toolbar
        btnLoadSong = GuiButton({ 32,  72, 32, 32 }, "MP3");
        if (btnLoadSong) events.push_back({ GuiEventType::MP3 });
        btnLoad = GuiButton({ 32, 112, 32, 32 }, "Ld.");
        if (btnLoad)   events.push_back({ GuiEventType::Load });
        btnSave = GuiButton({ 32, 152, 32, 32 }, "Sv.");
        if (btnSave)   events.push_back({ GuiEventType::Save });

        GuiPanel({ 72,  64, 800,128 }, nullptr);

        btnRestart = GuiButton({ 880, 72, 32, 32 }, "Rs.");
        if (btnRestart) events.push_back({ GuiEventType::Restart });
        btnPlay = GuiButton({ 880,112, 32, 32 }, "Pl.");
        if (btnPlay || IsKeyPressed(KEY_SPACE))    events.push_back({ GuiEventType::Play });

        // Timeline scrubber
        if (GuiSlider({ 80,200,784,16 }, nullptr, nullptr, &timelineVal, 0.0f, 100.0f)) {
            events.push_back({ GuiEventType::Skip, std::to_string(timelineVal) });
        }

        // Left panels
        GuiPanel({ 32,224,200,264 }, nullptr);
        GuiPanel({ 248,224,200,264 }, nullptr);
        

        GuiLabel({ 40,232,112,16 }, "Global");
        GuiLabel({ 256,232,112,16 }, "Edit section");

        // Offset spinner
        lastOffsetVal = offsetVal;
        static bool OffsetEdit = false;
        if (GuiSpinner({ 128,256,96,16 }, nullptr, &offsetVal, 0, 99999, OffsetEdit))
            OffsetEdit = !OffsetEdit;
        if (offsetVal != lastOffsetVal)
            events.push_back({ GuiEventType::Offset, std::to_string(offsetVal) });
        GuiLabel({ 40,256,72,16 }, "Song offset");

        // BPM spinner
        lastBpmVal = bpmVal;
        static bool BpmEdit = false;
        if (GuiSpinner({ 328,256,112,16 }, nullptr, &bpmVal, 1, MAX_BPM, BpmEdit))
            BpmEdit = !BpmEdit;
        if (bpmVal != lastBpmVal)
            events.push_back({ GuiEventType::Bpm, std::to_string(bpmVal) });
        GuiLabel({ 256,256,40,16 }, "BPM");
        GuiLabel({ 256,280,72,16 }, "Signature");

        // Section insert/remove
        btnInsertSec = GuiButton({ 256,456, 88,24 }, "Insert new");
        if (btnInsertSec) events.push_back({ GuiEventType::InsertSection });
        btnRemoveSec = GuiButton({ 352,456, 88,24 }, "Remove current");
        if (btnRemoveSec) events.push_back({ GuiEventType::RemoveSection });

        // Right panels
        GuiPanel({ 464,224,200,264 }, nullptr);
        GuiLabel({ 472,232,112,16 }, "View");
        GuiLabel({ 472,256, 56,16 }, "Spacing");

        // Spacing slider
        lastSpacingVal = spacingVal;
        GuiSlider({ 544,256,112,16 }, nullptr, nullptr, &spacingVal, 0.05f, 1.0f);
        if (spacingVal != lastSpacingVal)
            events.push_back({ GuiEventType::Spacing, std::to_string(spacingVal) });

        // Jump‐to label & dropdown
        GuiLabel({ 256,304,56,16 }, "Jump to");
        lastJumpSel = jumpSel;
        static bool JumpEdit = false;
        if (GuiDropdownBox({ 328,304,112,16 }, "ONE;TWO;THREE", &jumpSel, JumpEdit))
            JumpEdit = !JumpEdit;
        if (jumpSel != lastJumpSel)
            events.push_back({ GuiEventType::JumpSection, std::to_string(jumpSel) });

        // Speed dropdown
        static int SpeedActive = 0;
        static bool SpeedEdit = false;
        if (GuiDropdownBox({ 880,152,64,32 }, PLAYBACK_MULTIPLIERS_STR.c_str(), &SpeedActive, SpeedEdit)) {
            events.push_back({ GuiEventType::PlaybackSpeed, std::to_string(PLAYBACK_MULTIPLIERS_RAW[SpeedActive]) });
            SpeedEdit = !SpeedEdit;
        }  

        static bool SigEdit = false;
        
        if (GuiDropdownBox({ 328,280,112,16 }, ALLOWED_SIGNATURES_STR.c_str(), &selectedSignature, SigEdit)) {
            events.push_back({ GuiEventType::Signature, std::to_string(selectedSignature)});
            SigEdit = !SigEdit;
        }

        GuiPanel({ 680,224,200,264 }, nullptr);
        GuiLabel({ 688,232,56,16 }, "Notes");
        GuiLabel({ 688,256, 56,16 }, "Place");

        static bool isNotePlaced = false;
        btnInsertNote =  GuiButton({ 760, 256 , 112, 16 }, isNotePlaced ? "Note end" : "Note start");
        if (btnInsertNote ) {      
            if (isNotePlaced) {
                events.push_back({ GuiEventType::InsertNoteEnd });
                
            }
            if (!isNotePlaced) {
                events.push_back({ GuiEventType::InsertNoteStart });
            }
            isNotePlaced = !isNotePlaced;
            
            
        }

        GuiLabel({ 688, 280 ,112, 16 }, "Remove");
        btnRemoveNote = GuiButton({ 760, 280 , 112, 16 }, "Right Mouse");
        if (btnRemoveNote ) {  //|| IsMouseButtonPressed(MOUSE_BUTTON_LEFT)
            events.push_back({ GuiEventType::RemoveNote });
        }


            
        GuiDisable();

        return events;
    }

  
    void draw_timeline(int elapsed,
        const std::vector<PlayableBeat>& beats,
        const std::vector<Section>& sections,
        const std::vector<Note>& notes,
        float spacing,
        int offset)
    {

        float leftX = TIMELINE_AREA.x;
        float rightX = TIMELINE_AREA.x + TIMELINE_AREA.width;
        float centerX = leftX + TIMELINE_AREA.width * 0.5f;
        int   topY = TIMELINE_AREA.y;
        int   bottomY = TIMELINE_AREA.y + TIMELINE_AREA.height;

        // 1) Draw section bands (as before, but shifted so now=middle)
        for (size_t i = 0; i < sections.size(); ++i)
        {
            const auto& sec = sections[i];
            float x1 = centerX + (sec.start_ms - elapsed)
                * PIXEL_PER_MS * spacing;
            float x2 = centerX + (sec.end_ms - elapsed)
                * PIXEL_PER_MS * spacing;

            if (x2 < leftX)  continue;
            if (x1 > rightX) break;

            x1 = std::max(x1, leftX);
            x2 = std::min(x2, rightX);

            Color fill = (i % 2 == 0 ? Fade({ 255,20,20,255 }, 0.2f)
                : Fade({ 20,20,255,255 }, 0.2f));
            Rectangle band{ x1, TIMELINE_AREA.y, x2 - x1, TIMELINE_AREA.height };
            DrawRectangleRec(band, fill);

            std::string info = "BPM:" + std::to_string(sec.bpm)
                + " Sig:" + std::to_string(sec.signature.numerator)
                + "/" + std::to_string(sec.signature.denominator)
                + "\n[" + std::to_string(sec.start_ms)
                + "-" + std::to_string(sec.end_ms) + "]";
            DrawText(info.c_str(), int(x1 + 4), int(band.y + 4), 12, BLACK);
        }

        // 2) Cull & draw only visible beats (shifted around centerX)
        struct VBeat { float x; const Beat* b; };
        std::vector<VBeat> visible;
        visible.reserve(beats.size());
        for (auto& b : beats)
        {
            float bx = centerX + (b.timing_ms - elapsed)
                * PIXEL_PER_MS * spacing;

            if (bx < leftX)       continue;
            if (bx > rightX)      break;
            visible.push_back({ bx, &b });
        }

        // 3) Hover highlight (same logic)
        Vector2 mp = GetMousePosition();
        for (size_t i = 0; i + 1 < visible.size(); ++i)
        {
            float mid = (visible[i].x + visible[i + 1].x) * 0.5f;
            Rectangle slice{
                mid - (visible[i + 1].x - visible[i].x) * 0.5f,
                TIMELINE_AREA.y,
                visible[i + 1].x - visible[i].x,
                TIMELINE_AREA.height
            };
            if (CheckCollisionPointRec(mp, slice))
            {
                DrawRectangleRec(slice, Fade(LIGHTGRAY, 0.1f));
                DrawText(
                    TextFormat("%d", visible[i].b->timing_ms),
                    int(visible[i].x),
                    TIMELINE_AREA.y + TIMELINE_AREA.height - 12,
                    12,
                    BLACK
                );
                break;
            }
        }

        // 4) Draw ticks & numbers
        int centerY = TIMELINE_AREA.y + TIMELINE_AREA.height / 2;
        for (auto& vb : visible)
        {
            const Beat& b = *vb.b;
            int tickH = (b.index == 1 ? 20 : 10);
            DrawLine(int(vb.x), centerY - tickH, int(vb.x), centerY + tickH, BLACK);

            const char* txt = TextFormat("%u", b.index);
            int txtW = MeasureText(txt, 14);
            DrawText(txt,
                int(vb.x - txtW / 2),
                centerY + TIMELINE_AREA.height / 4 + 2,
                14,
                BLACK);
        }

        for (auto& n : notes)
        {
            float bx = centerX + (n.timing_ms - elapsed)
                * PIXEL_PER_MS * spacing;
            float bend = centerX + ((n.timing_ms + n.length_ms) - elapsed)
                * PIXEL_PER_MS * spacing;

            if (bx < leftX)       continue;
            if (bx > rightX)      break;
            if (bend < leftX)       continue;
            if (bend > rightX)      bend = rightX;

            DrawLineEx({ float(bx) - 15*spacing, (float)centerY }, { float(bend + 15*spacing), (float)centerY }, 2, BLACK);
            
        }

        // 0) Draw blue playhead
        DrawLineEx({ float(centerX), (float)topY }, { float(centerX), (float)bottomY }, 3, BLUE);
        // And the current time label
        const char* timeTxt = TextFormat("%d ms", elapsed);

        DrawText(timeTxt,
            int(centerX) + 3,
            TIMELINE_AREA.y + TIMELINE_AREA.height - 14,
            14,
            BLUE);
    }


};