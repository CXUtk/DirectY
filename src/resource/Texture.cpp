#include "Texture.h"
#include <algorithm>
Texture::Texture(int width, int height, TextureColorFormat colorFormat, const std::vector<unsigned char>& data) : width(width), height(height), _colorFormat(colorFormat) {
    _data = new unsigned char[data.size()];
    memcpy(_data, data.data(), data.size());
}

Texture::~Texture() {
}

glm::vec4 Texture::SampleColor(float u, float v, bool invY, TextureWarpingMode mode, TextureSampleModeMag sampleMag) const {
    switch (mode) {
    case TextureWarpingMode::CLAMP:
        u = std::max(0.f, std::min(u, 1.0f));
        v = std::max(0.f, std::min(v, 1.0f));
        break;
    case TextureWarpingMode::REPEAT:
        u = std::fmod(u, 1.0f);
        v = std::fmod(v, 1.0f);
        if (u < 0) u += 1.0f;
        if (v < 0) v += 1.0f;
        break;
    case TextureWarpingMode::MIRRORED_REPEAT:
        u = std::abs(u);
        v = std::abs(v);
        u = 1.0f - std::fmod(u, 1.0f);
        v = 1.0f - std::fmod(v, 1.0f);
        break;
    default:
        break;
    }
    int x = (int)(u * width);
    int y = (int)(v * height);

    switch (sampleMag) {
    case TextureSampleModeMag::NEAREST: {
        return getPixel(x, y, invY);
    }
    case TextureSampleModeMag::BI_LINEAR: {
        float dx = u * width - x, dy = v * height - y;
        glm::vec4 c[4];
        c[0] = getPixel(x, y, invY);
        c[1] = getPixel(x + 1, y, invY);
        c[2] = getPixel(x + 1, y + 1, invY);
        c[3] = getPixel(x, y + 1, invY);

        glm::vec4 X1 = glm::mix(c[0], c[1], dx);
        glm::vec4 X2 = glm::mix(c[3], c[2], dx);
        return glm::mix(X1, X2, dy);
    }
    default:
        return glm::vec4(0);
    }
}

glm::vec4 Texture::getPixel(int x, int y, bool invY) const {
    int r = std::min(y, height - 1);
    int c = std::min(x, width - 1);
    int idx = r * width + c;
    switch (_colorFormat) {
    case TextureColorFormat::GRAYSCALE8: {
        float v = _data[idx] / (float)255;
        return glm::vec4(glm::vec3(v), 1.0f);
        break;
    }
    case TextureColorFormat::DEPTH32: {
        float d = *((float*)_data + idx);
        return glm::vec4(glm::vec3(d), 1.0f);
        break;
    }
    case TextureColorFormat::RGB24: {
        idx *= 3;
        unsigned char c[3];
        memcpy(c, _data + idx, 3);
        return glm::vec4(glm::vec3(c[0] / (float)255, c[1] / (float)255, c[2] / (float)255), 1.0f);
        break;
    }
    case TextureColorFormat::RGBA32: {
        idx *= 4;
        unsigned char c[4];
        memcpy(c, _data + idx, 4);
        return glm::vec4(glm::vec3(c[0] / (float)255, c[1] / (float)255, c[2] / (float)255), c[3] / (float)255);
        break;
    }
    default:
        break;
    }
    return glm::vec4(0);
}
