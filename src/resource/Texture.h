#pragma once
#include <vector>
#include <glm/glm.hpp>
enum class TextureColorFormat {
    GRAYSCALE8,
    DEPTH32,
    RGB24,
    RGBA32,
};

enum class TextureWarpingMode {
    CLAMP,
    REPEAT,
    MIRRORED_REPEAT,
};

enum class TextureSampleModeMag {
    NEAREST,
    BI_LINEAR,
};
class Texture {
public:
    Texture() = default;
    Texture(int width, int height, TextureColorFormat colorFormat, const std::vector<unsigned char>& data);
    ~Texture();


    glm::vec4 SampleColor(float u, float v, bool invY, TextureWarpingMode mode, TextureSampleModeMag sampleMag) const;

private:
    unsigned char* _data;
    int width, height;
    TextureColorFormat _colorFormat;

    glm::vec4 getPixel(int x, int y, bool invY) const;
};
