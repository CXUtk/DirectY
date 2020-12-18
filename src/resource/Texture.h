#pragma once
#include <vector>
#include <glm/glm.hpp>
enum class ColorFormat {
    GRAYSCALE8,
    DEPTH32,
    RGB24,
    RGBA32,
};

enum class SampleMode {
    CLAMP,
    REPEAT,
    MIRRORED_REPEAT,
};
class Texture {
public:
    Texture() = default;
    Texture(int width, int height, ColorFormat colorFormat, const std::vector<unsigned char>& data);
    ~Texture();


    glm::vec4 SampleColor(float u, float v, bool invY) const;

private:
    unsigned char* _data;
    int width, height;
    ColorFormat _colorFormat;

    glm::vec4 getPixel(int x, int y, bool invY) const;
};
