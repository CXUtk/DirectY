#include "UserInput.h"

UserInput::UserInput() {
    _wheelValue = 0;
}

void UserInput::MouseDown() {
    _mouseInfo.isMouseLeftDown = true;
}

void UserInput::MouseUp() {
    _mouseInfo.isMouseLeftDown = false;
}

void UserInput::MouseMove(glm::ivec2 pos) {
    _mouseInfo.mousePos = pos;
}

void UserInput::MouseWheel(int val) {
    _wheelValue += val / 120;
}
