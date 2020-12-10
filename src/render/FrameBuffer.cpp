#include "FrameBuffer.h"
#include <cstring>

FrameBuffer::FrameBuffer(int width, int height) :_width(width), _height(height) {
    _buffer = new Color[width * height];
    _depthBuffer = new float[width * height];
}

FrameBuffer::~FrameBuffer() {
    delete[] _buffer;
    delete[] _depthBuffer;
}

void FrameBuffer::Clear() {
    memset(_buffer, 0, sizeof(Color) * _width * _height);
    memset(_depthBuffer, 0, sizeof(float) * _width * _height);
}

void FrameBuffer::Write(int x, int y, glm::vec3 color) {
    int r = y;
    int c = x;

    int idx = r * _width + c;

    _buffer[idx].R = (unsigned char)(glm::clamp(color[2], 0.0f, 0.999f) * 256);
    _buffer[idx].G = (unsigned char)(glm::clamp(color[1], 0.0f, 0.999f) * 256);
    _buffer[idx].B = (unsigned char)(glm::clamp(color[0], 0.0f, 0.999f) * 256);
}
