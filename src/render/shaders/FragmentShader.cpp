#include "FragmentShader.h"


glm::vec3 FragmentShader::fragment_shader(const FragmentShaderPayload& payload) {
    auto uv = payload.vertex.texCoord * 5.f;
    uv.x = std::fmod(uv.x, 1.0);
    uv.y = std::fmod(uv.y, 1.0);
    bool a = uv.x < 0.5;
    bool b = uv.y < 0.5;
    return payload.vertex.color * ((a + b == 1) ? glm::vec3(0.5) : glm::vec3(1));
}
