#include "raylib.h"
#include "raygui.h"
#include <iostream>
#include "Timable.h"
#include "Editor.h"
#include "BeatFactory.h"

Editor::Editor() {
	//test data
	this->sections.push_back( 
		Section(
			{ 1, 4 },
			60,
			0,
			1000
			)
	);
	this->sections.push_back(
		Section(
			{ 1, 4 },
			60,
			1000,
			2000
		)
	);
	//pre gen all beats
	for (auto& i : sections) {
		auto s_beats = BeatFactory::GENERATE_FROM_SECTION(i);
		for (auto& s : s_beats) {
			this->pregen_beats.push_back(s);
		}
	}
}

void Editor::listen() {

}

void Editor::render() {
	BeginDrawing();
	ClearBackground(RAYWHITE);
	GuiEnable();

	_draw_timeline();
	_draw_playback_button();


	GuiDisable();
	EndDrawing();
}

void Editor::update() {
	this->tick();
	this->listen();
	this->render();
}

void Editor::_draw_timeline() {
	DrawRectangleRec(TIMELINE_AREA, GRAY);
	BeginScissorMode(
		int(TIMELINE_AREA.x), int(TIMELINE_AREA.y),
		int(TIMELINE_AREA.width), int(TIMELINE_AREA.height) + 100
	);

	int centerY = TIMELINE_AREA.y + TIMELINE_AREA.height / 2;
	
	int  elapsed = getTimePassed();

	std::vector<float> xs;
	xs.reserve(pregen_beats.size());
	for (auto& b : pregen_beats) {
		float x = TIMELINE_AREA.x
			+ (b.timing_ms - elapsed) * PIXEL_PER_MS * _spacingMul;
		xs.push_back(x);
	}

	Vector2 mp = GetMousePosition();

	for (size_t i = 0; i + 1 < xs.size(); ++i) {
		const int width = xs[i + 1] - xs[i];
		Rectangle slice{
			xs[i] - width / 2,
			TIMELINE_AREA.y,
			width,
			TIMELINE_AREA.height
		};
		/*if (CheckCollisionPointRec(mp, slice)) {
			DrawRectangleRec(slice, Fade(UI_LINE_COLOR, 0.1f));
			DrawText(std::to_string(beats[i].timing_ms).c_str(), xs[i] + (xs[i + 1] - xs[i]) / 2,
				TIMELINE_AREA.y + TIMELINE_AREA.height - 12, 12, BLACK);

			if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) {
				placedNotes.insert({ beats[i].timing_ms, { beats[i].timing_ms, 0 } });
			}

			if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_RIGHT)) {
				placedNotes.erase(beats[i].timing_ms);
			}

			break;
		}*/
	}

	for (size_t i = 0; i < xs.size(); ++i) {
		bool isDown = (i % sections[0].signature.denominator) == 0;
		int  tickH = isDown ? 50 : 30;
		Color col = isDown ? BLACK : BLACK;

		float x = xs[i];
		DrawLine(int(x), centerY - tickH, int(x), centerY + tickH, col);

		//const char* txt = TextFormat("%d", beats[i].index + 1);
		/*int txtW = MeasureText(txt, 14);
		DrawText(
			txt,
			int(x - txtW / 2),
			centerY + 50 + 2,
			14,
			BLACK
		);*/
	}

	EndScissorMode();

}

void Editor::_draw_playback_button() {
	int ret = GuiButton({ 5, 5, 75, 25 }, isPaused() ? "Resume" : "Pause");
	if (ret) isPaused() ? resume() : pause();
}
