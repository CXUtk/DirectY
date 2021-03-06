﻿#include "FragmentShader.h"

#include <algorithm>
FragmentShader::FragmentShader() {
    _sampleMode = TextureSampleModeMag::NEAREST;
}
glm::vec3 FragmentShader::fragment_shader(const FragmentShaderPayload& payload) {
    //auto uv = payload.vertex.texCoord;
    //uv *= 5;
    //uv.x = std::fmod(uv.x, 1.0);
    //uv.y = std::fmod(uv.y, 1.0);
    //if (uv.x < 0) uv.x = 1.0 - std::abs(uv.x);
    //if (uv.y < 0) uv.y = 1.0 - std::abs(uv.y);

    //bool a = uv.x < 0.5;
    //bool b = uv.y < 0.5;
    static glm::vec3 dirLight = glm::vec3(0, 0, 5);

    // 光照模型中法向量需要先进行normalize，否则插值出来的法向量并不一定正确
    auto N = glm::normalize(payload.vertex.normal);

    //N = glm::vec3(_texture2->SampleColor(payload.vertex.texCoord.x, payload.vertex.texCoord.y, true, TextureWarpingMode::CLAMP,
    //    TextureSampleModeMag::NEAREST));
    //N = glm::normalize(N * 2.f - glm::vec3(1.0));
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

    glm::vec3 textureColor = glm::vec3(_texture->SampleColor(payload.vertex.texCoord.x, payload.vertex.texCoord.y, true, TextureWarpingMode::CLAMP,
        _sampleMode));
    return   textureColor * (glm::vec3(0.1f) + glm::vec3(0.8f) * diffuse * alpha * alpha) + glm::vec3(specular) * 0.5f;
}
