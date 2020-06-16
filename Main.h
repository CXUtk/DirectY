#pragma once
#include <Windows.h>
#include "Rasterizer.h"
#include "Camera.h"
class Main {
public:
    Main(HWND hwnd, int width, int height);
    void Run();
    void MoveCamera(const glm::vec3& moveVec) {
        _camera.SetPosition(_camera.GetPosition() + moveVec);
    }
    ~Main() {
        delete _rasterizer;
    }
private:
    HWND hWnd;
    int _width, _height;
    Rasterizer* _rasterizer;
    Camera _camera;

    std::vector<Triangle> _triangles;
};
