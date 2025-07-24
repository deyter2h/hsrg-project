#include "Editor.h"

int main() {
    InitAudioDevice();
    InitWindow(1024, 600, "R");
    SetTargetFPS(60);

    Editor ed = Editor();
    ed.pause();

    while (!WindowShouldClose()) {
        ed.update();
    }

    CloseWindow();
    return 0;
    
}