#include "Main.h"
#include <glm/gtx/transform.hpp>
#include <vector>


Main::Main(HWND hwnd, int width, int height) :hWnd(hwnd), _width(width), _height(height) {
    _renderer = new Renderer(width, height);

    Vertex triangle[3] = {
        Vertex(glm::vec4(0, 0.5, 0, 1), glm::vec3(1, 0, 0)),
        Vertex(glm::vec4(-0.5, -0.5, 0, 1), glm::vec3(0, 1, 0)),
        Vertex(glm::vec4(0.5, -0.5, 0, 1), glm::vec3(0, 0,1))
    };

    _vbuff = _renderer->CreateVertexBuffer(sizeof(triangle), sizeof(Vertex), &triangle);
}

void Main::Run() {
    _renderer->ClearFrameBuffer();
    _renderer->Draw(_vbuff, 0, 3);
    _renderer->Present(hWnd);
}
