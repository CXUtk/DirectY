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
    memset(_buffer, 0xaa, sizeof(Color) * _width * _height);
    memset(_depthBuffer, 0, sizeof(float) * _width * _height);
}
