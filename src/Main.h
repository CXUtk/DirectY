#pragma once
#include <Windows.h>
#include <glm/gtx/transform.hpp>
#include "render/Renderer.h"
class Main {
public:
    Main(HWND hwnd, int width, int height);
    void Run();
    ~Main() {
        delete _renderer;
    }
private:
    HWND hWnd;
    int _width, _height;

    Renderer* _renderer;
};
