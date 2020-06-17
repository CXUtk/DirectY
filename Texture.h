#pragma once
#include <string>
#include <vector>
#include "lodepng.h"
#include <algorithm>
#include <glm/glm.hpp>

class Texture {
public:
    Texture(const std::string& filename) {
        std::vector<unsigned char> png;
        //load and decode
        unsigned error = lodepng::load_file(png, filename);
        if (!error) error = lodepng::decode(_image, width, height, png);
        GenMipMap();
    }
    ~Texture();

    glm::vec3 GetPixel(int x, int y, int L = 0)const {
        int offset = (y * width / (1 << L) + x);
        return mipmap[L][offset];
    }
    glm::vec3 GetNearest(float x, float y) const {
        int offset = ((int)(y * height - 1e-6) * width + (int)(x * width - 1e-6)) * 4;
        return glm::vec3(_image[offset] / 255.f, _image[offset + 1] / 255.f, _image[offset + 2] / 255.f);
    }
    glm::vec3 GetBinlinearClamp(float x, float y, int L) const;
    glm::vec3 GetSquare(glm::vec2 tl, float L) const;
    void GenMipMap();
private:
    void _genMipmap(int L, int w, int h);

    std::vector<unsigned char> _image;
    glm::vec3* mipmap[10];
    unsigned width, height;
};
