#include "FragmentShader.h"

#include <algorithm>
glm::vec3 FragmentShader::fragment_shader(const FragmentShaderPayload& payload) {
    auto uv = payload.vertex.texCoord * 5.f;
    uv.x = std::fmod(uv.x, 1.0);
    uv.y = std::fmod(uv.y, 1.0);
    bool a = uv.x < 0.5;
    bool b = uv.y < 0.5;
    static glm::vec3 dirLight = glm::vec3(0, 0, -5);

    // 光照模型中法向量需要先进行normalize，否则插值出来的法向量并不一定正确
    auto N = glm::normalize(payload.vertex.normal);
    auto I = dirLight - glm::vec3(payload.vertex.pos);
    auto dis = glm::length(I);
    I /= dis;
    auto R = glm::reflect(-I, N);

    auto viewPos = _camera->GetEyePos();
    auto V = glm::normalize(viewPos - glm::vec3(payload.vertex.pos));


    auto diffuse = std::max(glm::dot(N, I), 0.f);
    auto specular = std::pow(std::max(glm::dot(R, V), 0.f), 128);
    float v = dis / 9;
    float alpha = std::max(1 - v * v, 0.f);
    //glm::vec3(0.5) + N * 0.5f;//
    return  glm::vec3(((a + b) & 1) ? 0.5f : 1.0f) * (glm::vec3(0.2f) + glm::vec3(0.8f) * diffuse * alpha * alpha) + glm::vec3(specular) * 0.5f;
}
