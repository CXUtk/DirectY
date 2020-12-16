#include "FragmentShader.h"

#include <algorithm>
glm::vec3 FragmentShader::fragment_shader(const FragmentShaderPayload& payload) {
    auto uv = payload.vertex.texCoord * 5.f;
    uv.x = std::fmod(uv.x, 1.0);
    uv.y = std::fmod(uv.y, 1.0);
    bool a = uv.x < 0.5;
    bool b = uv.y < 0.5;
    static glm::vec3 dirLight = glm::vec3(0, 0, 0);

    auto L = glm::normalize(dirLight - glm::vec3(payload.vertex.pos));
    auto diffuse = std::max(glm::dot(payload.vertex.normal, L), 0.f);
    return glm::vec3(0.3f) + glm::vec3(0.7f) * diffuse;
}
