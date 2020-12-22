#pragma once
#include <glm/glm.hpp>

struct Color {
    unsigned char R, G, B;
    Color() = default;
    Color(int r, int g, int b) {
        R = r, G = g, B = b;
    }
};

class FrameBuffer {
public:
    FrameBuffer(int width, int height, int multiple);
    ~FrameBuffer();

    void Clear();
    Color* GetBuffer() const { return _buffer; }
    Color* GetBufferWithOutMultipler() const;
    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }
    float GetZBuffer(int x, int y, int m) const;

    void Write(int x, int y, int m, glm::vec3 color);
    void WriteFull(int x, int y, glm::vec3 color);
    void WriteZBuffer(int x, int y, int m, float z);

private:
    Color* _buffer;
    Color* _tmpBuffer;
    float* _depthBuffer;
    int _width, _height;
    int _multiple;
};
