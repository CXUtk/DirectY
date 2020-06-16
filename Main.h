#pragma once
#include <Windows.h>
#include <glm/gtx/transform.hpp>
#include "Rasterizer.h"
#include "Camera.h"
class Main {
public:
    Main(HWND hwnd, int width, int height);
    void Run();
    void MoveCamera(const glm::vec3& moveVec) {
        _camera.SetPosition(_camera.GetPosition() + moveVec);
    }
    void RotateCamera(float r, const glm::vec3& axis) {
        glm::mat4 rot = glm::rotate(r, axis);
        glm::vec3 newPos = rot * glm::vec4(_camera.GetPosition(), 1);
        _camera.SetPosition(newPos);

    }
    ~Main() {
        delete _rasterizer;
        if (_texture)
            delete _texture;
    }
private:
    HWND hWnd;
    int _width, _height;
    Rasterizer* _rasterizer;
    Camera _camera;
    Texture* _texture;

    std::vector<Triangle> _triangles;
};
