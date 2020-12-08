#pragma once

struct Color {
    unsigned char R, G, B;
};

class FrameBuffer {
public:
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    void Clear();
    Color* GetBuffer() const { return _buffer; }
    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }

private:
    Color* _buffer;
    float* _depthBuffer;
    int _width, _height;
};
