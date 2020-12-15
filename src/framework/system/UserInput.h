#pragma once
#include <Windows.h>
#include <glm/glm.hpp>
#include "SystemUtils.h"

class UserInput {

    friend LRESULT CALLBACK  WndProc(HWND, UINT, WPARAM, LPARAM);

public:
    UserInput() = default;
    ~UserInput() = default;
    MouseInfo getMouseInfo()const { return _mouseInfo; }

private:
    MouseInfo _mouseInfo;

    void MouseDown();
    void MouseUp();
    void MouseMove(glm::ivec2 pos);
};
