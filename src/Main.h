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


    int _vbuff, _vbuff2, _vbuff3;
    int _modelBuff, _modelBuff1;
    int _numVertices, _numFaces;

    Renderer* _renderer;
};
