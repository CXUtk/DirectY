#include "FrameBuffer.h"
#include <cstring>

FrameBuffer::FrameBuffer(int width, int height, int multiple) :_width(width), _height(height), _multiple(multiple) {
    _buffer = new Color[width * height * multiple];
    _tmpBuffer = new Color[width * height];
    _depthBuffer = new float[width * height * multiple];
}

FrameBuffer::~FrameBuffer() {
    delete[] _buffer;
    delete[] _depthBuffer;
    delete[] _tmpBuffer;
}

void FrameBuffer::Clear() {
    memset(_buffer, 0, sizeof(Color) * _width * _height * _multiple);
    memset(_depthBuffer, 0x7f, sizeof(float) * _width * _height * _multiple);
}

Color* FrameBuffer::GetBufferWithOutMultipler() const {
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            int idx = i * _width + j;
            glm::vec3 color = glm::vec3(0);
            for (int k = 0; k < _multiple; k++) {
                auto& c = _buffer[idx * _multiple + k];
                color += glm::vec3(c.R, c.G, c.B);
            }
            color /= _multiple;
            _tmpBuffer[idx] = Color((int)(color.r + 0.5), (int)(color.g + 0.5), (int)(color.b + 0.5));
        }
    }
    return _tmpBuffer;
}

float FrameBuffer::GetZBuffer(int x, int y, int m) const {
    int r = y;
    int c = x;
    if (r < 0 || r >= _height || c < 0 || c >= _width) return 0;

    int idx = r * _width + c;
    return _depthBuffer[idx * _multiple + m];
}

void FrameBuffer::Write(int x, int y, int m, glm::vec3 color) {
    int r = y;
    int c = x;

    if (r < 0 || r >= _height || c < 0 || c >= _width) return;

    int idx = r * _width + c;
    //if ((int)_buffer[idx].R + _buffer[idx].G + _buffer[idx].B > 0) {
    //    memset(&_buffer[idx], 0xff, sizeof(Color));
    //    return;
    //}

    Color& pixel = _buffer[idx * _multiple + m];

    pixel.R = (unsigned char)(glm::clamp(color[2], 0.0f, 0.999f) * 256);
    pixel.G = (unsigned char)(glm::clamp(color[1], 0.0f, 0.999f) * 256);
    pixel.B = (unsigned char)(glm::clamp(color[0], 0.0f, 0.999f) * 256);
}

void FrameBuffer::WriteFull(int x, int y, glm::vec3 color) {
    int r = y;
    int c = x;

    if (r < 0 || r >= _height || c < 0 || c >= _width) return;

    int idx = r * _width + c;

    for (int k = 0; k < _multiple; k++) {
        Color& pixel = _buffer[idx * _multiple + k];
        pixel.R = (unsigned char)(glm::clamp(color[2], 0.0f, 0.999f) * 256);
        pixel.G = (unsigned char)(glm::clamp(color[1], 0.0f, 0.999f) * 256);
        pixel.B = (unsigned char)(glm::clamp(color[0], 0.0f, 0.999f) * 256);
    }
}

void FrameBuffer::WriteZBuffer(int x, int y, int m, float z) {
    int r = y;
    int c = x;
    if (r < 0 || r >= _height || c < 0 || c >= _width) return;

    int idx = r * _width + c;
    _depthBuffer[idx * _multiple + m] = z;
}
