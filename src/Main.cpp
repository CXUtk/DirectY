#include "Main.h"
#include <glm/gtx/transform.hpp>
#include <vector>


Main::Main(HWND hwnd, int width, int height) :hWnd(hwnd), _width(width), _height(height) {
    _renderer = new Renderer(width, height);
}

void Main::Run() {
    _renderer->ClearFrameBuffer();
    _renderer->Present(hWnd);
}
