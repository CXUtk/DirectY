#include "Texture.h"
#include <algorithm>
Texture::Texture(int width, int height, ColorFormat colorFormat, const std::vector<unsigned char>& data) : width(width), height(height), _colorFormat(colorFormat) {
    _data = new unsigned char[data.size()];
    memcpy(_data, data.data(), data.size());
}

Texture::~Texture() {
}

glm::vec4 Texture::SampleColor(float u, float v, bool invY) const {
    int x = u * width;
    int y = v * height;
    return glm::vec4(getPixel(x, y, invY));
}

glm::vec4 Texture::getPixel(int x, int y, bool invY) const {
    x = std::min(std::max(x, 0), width - 1);
    y = std::min(std::max(y, 0), width - 1);
    int r = invY ? (height - y - 1) : y;
    int c = x;
    int idx = r * width + c;
    switch (_colorFormat) {
    case ColorFormat::GRAYSCALE8: {
        float v = _data[idx] / (float)255;
        return glm::vec4(glm::vec3(v), 1.0f);
        break;
    }
    case ColorFormat::DEPTH32: {
        float d = *((float*)_data + idx);
        return glm::vec4(glm::vec3(d), 1.0f);
        break;
    }
    case ColorFormat::RGB24: {
        idx *= 3;
        unsigned char c[3];
        memcpy(c, _data + idx, 3);
        return glm::vec4(glm::vec3(c[0] / (float)255, c[1] / (float)255, c[2] / (float)255), 1.0f);
        break;
    }
    case ColorFormat::RGBA32: {
        idx *= 4;
        unsigned char c[4];
        memcpy(c, _data + idx, 4);
        return glm::vec4(glm::vec3(c[0] / (float)255, c[1] / (float)255, c[2] / (float)255), c[3] / (float)255);
        break;
    }
    default:
        break;
    }
}
