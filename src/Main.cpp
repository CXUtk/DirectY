#include "Main.h"
#include <glm/gtx/transform.hpp>
#include <vector>


Main::Main(HWND hwnd, int width, int height) :hWnd(hwnd), _width(width), _height(height) {
    _renderer = new Renderer(width, height);

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

    _vbuff = _renderer->CreateVertexBuffer(sizeof(triangle), sizeof(Vertex), &triangle);
    _vbuff2 = _renderer->CreateVertexBuffer(sizeof(triangle2), sizeof(Vertex), &triangle2);
}

void Main::Run() {
    _renderer->ClearFrameBuffer();
    _renderer->Draw(_vbuff, 0, 6);
    _renderer->Draw(_vbuff2, 0, 3);
    _renderer->Present(hWnd);
}
