#include "UserInput.h"

void UserInput::MouseDown() {
    printf("Clicked! %d %d\n", _mouseInfo.mousePos.x, _mouseInfo.mousePos.y);
    _mouseInfo.isMouseLeftDown = true;
}

void UserInput::MouseUp() {
    printf("Released!\n");
    _mouseInfo.isMouseLeftDown = false;
}

void UserInput::MouseMove(glm::ivec2 pos) {
    _mouseInfo.mousePos = pos;
}
