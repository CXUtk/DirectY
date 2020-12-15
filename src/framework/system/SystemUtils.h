#pragma once
struct MouseInfo {
    bool isMouseLeftDown;
    glm::ivec2 mousePos;

    MouseInfo() :isMouseLeftDown(false), mousePos(glm::ivec2(0)) {
    }
};
