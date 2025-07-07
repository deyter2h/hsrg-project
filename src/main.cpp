#include "raylib.h"
#include <algorithm>
#include "MapPlayer.h"

int main() {
    InitAudioDevice();
    InitWindow(800, 100, "HSRG");
    SetTargetFPS(60);

    MapPlayer player;
    while (!WindowShouldClose()) {
        player.updateAndDraw();
    }

    CloseWindow();
    return 0;
}

