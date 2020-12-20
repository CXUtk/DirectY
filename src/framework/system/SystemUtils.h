#pragma once

constexpr int MAX_KEY_TYPE = 255;
struct MouseInfo {
    bool isMouseLeftDown;
    glm::ivec2 mousePos;

    MouseInfo() :isMouseLeftDown(false), mousePos(glm::ivec2(0)) {
    }
};


struct KeyState {
    bool isPressed[MAX_KEY_TYPE];

    KeyState() {
        memset(isPressed, 0, sizeof(isPressed));
    }
};
