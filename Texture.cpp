#include "Texture.h"

static glm::vec3 lerp(glm::vec3 A, glm::vec3 B, float x) {
    return A + (B - A) * x;
}

Texture::~Texture() {
    for (int i = 0; i < 10; i++) {
        if (mipmap[i]) {
            delete[] mipmap[i];
        }
    }

}

glm::vec3 Texture::GetBinlinearClamp(float x, float y, int L) const {
    x = x * width / (1 << L), y = y * height / (1 << L);
    int X = (int)(x - 1e-6), Y = (int)(y - 1e-6);
    int X1 = glm::clamp<int>(X + 1, 0, width / (1 << L) - 1);
    int Y1 = glm::clamp<int>(Y + 1, 0, height / (1 << L) - 1);
    glm::vec3 c[4];
    c[0] = GetPixel(X, Y, L);
    c[1] = GetPixel(X1, Y, L);
    c[2] = GetPixel(X, Y1, L);
    c[3] = GetPixel(X1, Y1, L);
    float Xext = x - X, Yext = y - Y;
    return lerp(lerp(c[0], c[2], Yext), lerp(c[1], c[3], Yext), Xext);
}

glm::vec3 Texture::GetSquare(glm::vec2 tl, float L) const {
    float Log = std::max(0.f, log2(L * width));
    return lerp(GetBinlinearClamp(tl.x, tl.y, (int)Log), GetBinlinearClamp(tl.x, tl.y, std::min((int)Log + 1, 8)), Log - (int)Log);
}


void Texture::GenMipMap() {
    mipmap[0] = new glm::vec3[width * height];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int offset = (i * width + j);
            mipmap[0][offset] = glm::vec3(_image[offset * 4] / 255.f, _image[offset * 4 + 1] / 255.f, _image[offset * 4 + 2] / 255.f);
        }
    }
    _genMipmap(1, width / 2, height / 2);
}


void Texture::_genMipmap(int L, int w, int h) {
    if (!w || !h)return;
    mipmap[L] = new glm::vec3[w * h];
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            glm::vec3 color(0, 0, 0);
            int offset = (i * 2 * w + j * 2);
            color += mipmap[L - 1][offset];
            offset = ((i * 2 + 1) * w + j * 2);
            color += mipmap[L - 1][offset];
            offset = (i * 2 * w + j * 2 + 1);
            color += mipmap[L - 1][offset];
            offset = ((i * 2 + 1) * w + j * 2 + 1);
            color += mipmap[L - 1][offset];

            mipmap[L][i * w + j] = color / 4.f;
        }
    }
    _genMipmap(L + 1, w / 2, h / 2);
}
