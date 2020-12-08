#pragma once
#include "FrameBuffer.h"
#include <glm/glm.hpp>
#include <memory>
#include <Windows.h>
class Renderer {
public:
    Renderer(int width, int height);

    void SetFrameBuffer(FrameBuffer* frameBuffer) { _frameBuffer = frameBuffer; }

    void ClearFrameBuffer();

    void Present(HWND hwnd);

private:
    FrameBuffer* _frameBuffer;
};
