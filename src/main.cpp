#include "raylib.h"
#include <algorithm>
#include "src/MapPlayer.h"

int main() {
    InitAudioDevice();
    InitWindow(800, 100, "HSRG");
    SetTargetFPS(165*4);

    MapPlayer player;
    while (!WindowShouldClose()) {
        player.updateAndDraw();
    }

    CloseWindow();
    return 0;
}

