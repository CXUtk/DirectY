#include "Texture.h"

static glm::vec3 lerp(glm::vec3 A, glm::vec3 B, float x) {
    return A + (B - A) * x;
}

glm::vec3 Texture::GetBinlinearClamp(float x, float y) {
    x = x * width, y = y * height;
    int X = (int)(x - 1e-6), Y = (int)(y - 1e-6);
    int X1 = glm::clamp<int>(X + 1, 0, width - 1);
    int Y1 = glm::clamp<int>(Y + 1, 0, height - 1);
    glm::vec3 c[4];
    c[0] = GetPixel(X, Y);
    c[1] = GetPixel(X1, Y);
    c[2] = GetPixel(X, Y1);
    c[3] = GetPixel(X1, Y1);
    float Xext = x - X, Yext = y - Y;
    return lerp(lerp(c[0], c[2], Yext), lerp(c[1], c[3], Yext), Xext);
}
