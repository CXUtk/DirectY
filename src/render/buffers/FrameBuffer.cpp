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
    memset(_depthBuffer, 0x7f, sizeof(float) * _width * _height);
}

float FrameBuffer::GetZBuffer(int x, int y) const {
    int r = y;
    int c = x;

    int idx = r * _width + c;
    return _depthBuffer[idx];
}

void FrameBuffer::Write(int x, int y, glm::vec3 color) {
    int r = y;
    int c = x;

    if (r < 0 || r >= _height || c < 0 || c >= _width) return;

    int idx = r * _width + c;
    //if ((int)_buffer[idx].R + _buffer[idx].G + _buffer[idx].B > 0) {
    //    memset(&_buffer[idx], 0xff, sizeof(Color));
    //    return;
    //}

    _buffer[idx].R = (unsigned char)(glm::clamp(color[2], 0.0f, 0.999f) * 256);
    _buffer[idx].G = (unsigned char)(glm::clamp(color[1], 0.0f, 0.999f) * 256);
    _buffer[idx].B = (unsigned char)(glm::clamp(color[0], 0.0f, 0.999f) * 256);
}

void FrameBuffer::WriteZBuffer(int x, int y, float z) {
    int r = y;
    int c = x;
    if (r < 0 || r >= _height || c < 0 || c >= _width) return;

    int idx = r * _width + c;
    _depthBuffer[idx] = z;
}
