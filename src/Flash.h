#pragma once
#include "raylib.h"
#include <cmath>

struct Flash {
    Color  flashColor = LIGHTGRAY;
    float  duration = 0.35f;   // в секундах для одиночной вспышки
    float  endTime = 0.0f;    // GetTime() после которого гаснет вспышка
    int    offsetMs = 0;       // запомнённый оффсет
    bool   holdActive = false;   // режим удержания подсветки

    // Запустить вспышку: цвет и оффсет
    void trigger(Color c, int delta) {
        flashColor = c;
        offsetMs = delta;
        endTime = GetTime() + duration;
        holdActive = false;  // отменяем режим удержания при однократной вспышке
    }

    // Включить подсветку до вызова unhold()
    void hold(Color c) {
        flashColor = c;
        holdActive = true;
    }

    // Отключить подсветку, возвращаясь к обычному фону
    void unhold() {
        holdActive = false;
    }

    // Проверка активной подсветки
    bool active() const {
        return holdActive || (GetTime() <= endTime);
    }

    // Возвращает текущий фон (flash или базовый)
    Color currentBg(Color baseBg) const {
        return active() ? Fade(flashColor, 0.3f) : baseBg;
    }

    // Рисует зону попадания [0..hitX] высотой height
    void drawZone(int hitX, int height) const {
        Color baseBg = Fade(LIGHTGRAY, 0.3f);
        DrawRectangle(0, 0, hitX, height, currentBg(baseBg));
    }

    // Рисует оффсет рядом с линией попадания
    void drawOffset(int posX, int posY, int fontSize = 20) const {
        if (!active()) return;
        const char* sign = offsetMs >= 0 ? "+" : "-";
        int val = std::abs(offsetMs);
        DrawText(
            TextFormat("%s%d ms", sign, val),
            posX, posY,
            fontSize,
            BLACK
        );
    }
};
