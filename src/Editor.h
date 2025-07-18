#pragma once
#include <Timable.h>
#include "raylib.h"

const Rectangle TIMELINE_AREA = { 0, 50, 800, 100 };
const float PIXEL_PER_MS = 0.5f;

class Editor : public Timeable {
public:
	Editor();

	void update();

private:

	float _spacingMul = 1.0f;
	int offset = 0;

	std::vector<Section> sections;
	std::vector<Beat> pregen_beats;

	void render();
	void listen();

	void _draw_timeline();
	void _draw_playback_button();
};

