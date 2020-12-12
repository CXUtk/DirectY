#include "Main.h"
#include "render/device/GDIDevice.h"
#include <glm/gtx/transform.hpp>
#include <vector>


Main::Main(HWND hwnd, int width, int height) :hWnd(hwnd), _width(width), _height(height) {
    _renderer = new Renderer(width, height, std::make_shared<GDIDevice>(hwnd));

    Vertex triangle[6] = {
        Vertex(glm::vec4(-0.5, 0.5, 0, 1), glm::vec3(1, 0, 0), glm::vec2(0, 0)),
        Vertex(glm::vec4(-0.5, -0.5, 0, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1)),
        Vertex(glm::vec4(0.5, -0.5, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 1)),

        Vertex(glm::vec4(-0.5, 0.5, 0, 1), glm::vec3(1, 0, 0), glm::vec2(0, 0)),
        Vertex(glm::vec4(0.5, -0.5, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 1)),
        Vertex(glm::vec4(0.5, 0.5, 0, 1), glm::vec3(1, 1, 0), glm::vec2(1, 0))
    };

    Vertex triangle2[3] = {
        Vertex(glm::vec4(-0.5, 0.5, -1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
        Vertex(glm::vec4(-0.5, -0.5, -1, 1), glm::vec3(1, 1, 0), glm::vec2(0, 1)),
        Vertex(glm::vec4(0.5, -0.5, 1, 1), glm::vec3(0, 1, 1), glm::vec2(1, 1)),
    };

    Vertex lines[2] = {
        Vertex(glm::vec4(1, 3, 0, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1)),
        Vertex(glm::vec4(-1, -1, 0, 1), glm::vec3(1, 1, 0), glm::vec2(0, 0)),
    };

    _vbuff = _renderer->CreateVertexBuffer(sizeof(triangle), sizeof(Vertex), &triangle);
    _vbuff2 = _renderer->CreateVertexBuffer(sizeof(triangle2), sizeof(Vertex), &triangle2);
    _vbuff3 = _renderer->CreateVertexBuffer(sizeof(lines), sizeof(Vertex), &lines);
}

void Main::Run() {
    _renderer->ClearFrameBuffer();
    //_renderer->SetDrawMode(DrawMode::WireFrame);
    _renderer->DrawElements(_vbuff, 0, 6, Primitives::Triangles);
    _renderer->DrawElements(_vbuff2, 0, 3, Primitives::Triangles);
    _renderer->DrawElements(_vbuff3, 0, 2, Primitives::Lines);
    _renderer->Present();
}
