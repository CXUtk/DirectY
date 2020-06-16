#pragma once
#include <string>
#include <vector>
#include "lodepng.h"
#include <glm/glm.hpp>

class Texture {
public:
    Texture(const std::string& filename) {
        std::vector<unsigned char> png;
        //load and decode
        unsigned error = lodepng::load_file(png, filename);
        if (!error) error = lodepng::decode(_image, width, height, png);
    }

    glm::vec3 GetPixel(int x, int y) {
        int offset = (y * width + x) * 4;
        return glm::vec3(_image[offset] / 255.f, _image[offset + 1] / 255.f, _image[offset + 2] / 255.f);
    }
    glm::vec3 GetNearest(float x, float y) {
        int offset = ((int)(y * height - 1e-6) * width + (int)(x * width - 1e-6)) * 4;
        return glm::vec3(_image[offset] / 255.f, _image[offset + 1] / 255.f, _image[offset + 2] / 255.f);
    }
    glm::vec3 GetBinlinearClamp(float x, float y);
private:
    std::vector<unsigned char> _image;
    unsigned width, height;
};
