#pragma once
#include <glm/glm.hpp>
#include "SystemUtils.h"

class UserInput {
public:
    UserInput();
    ~UserInput() = default;
    MouseInfo getMouseInfo()const { return _mouseInfo; }
    int getMouseWheel()const { return _wheelValue; }
    void MouseDown();
    void MouseUp();
    void MouseMove(glm::ivec2 pos);
    void MouseWheel(int val);

private:
    MouseInfo _mouseInfo;
    int _wheelValue;


};
