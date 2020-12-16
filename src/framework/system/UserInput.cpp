#include "UserInput.h"

void UserInput::MouseDown() {
    _mouseInfo.isMouseLeftDown = true;
}

void UserInput::MouseUp() {
    _mouseInfo.isMouseLeftDown = false;
}

void UserInput::MouseMove(glm::ivec2 pos) {
    _mouseInfo.mousePos = pos;
}
