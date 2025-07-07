#pragma once
#include "raylib.h"
#include <cmath>

struct Flash {
    Color  flashColor = LIGHTGRAY;
    float  duration = 0.35f;   // � �������� ��� ��������� �������
    float  endTime = 0.0f;    // GetTime() ����� �������� ������ �������
    int    offsetMs = 0;       // ���������� ������
    bool   holdActive = false;   // ����� ��������� ���������

    // ��������� �������: ���� � ������
    void trigger(Color c, int delta) {
        flashColor = c;
        offsetMs = delta;
        endTime = GetTime() + duration;
        holdActive = false;  // �������� ����� ��������� ��� ����������� �������
    }

    // �������� ��������� �� ������ unhold()
    void hold(Color c) {
        flashColor = c;
        holdActive = true;
    }

    // ��������� ���������, ����������� � �������� ����
    void unhold() {
        holdActive = false;
    }

    // �������� �������� ���������
    bool active() const {
        return holdActive || (GetTime() <= endTime);
    }

    // ���������� ������� ��� (flash ��� �������)
    Color currentBg(Color baseBg) const {
        return active() ? Fade(flashColor, 0.3f) : baseBg;
    }

    // ������ ���� ��������� [0..hitX] ������� height
    void drawZone(int hitX, int height) const {
        Color baseBg = Fade(LIGHTGRAY, 0.3f);
        DrawRectangle(0, 0, hitX, height, currentBg(baseBg));
    }

    // ������ ������ ����� � ������ ���������
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
