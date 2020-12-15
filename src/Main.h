#pragma once
#include <glm/gtx/transform.hpp>
#include "render/Renderer.h"
#include "framework/system/"
class Main {
public:
    Main(std::shared_ptr<Window>);
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
