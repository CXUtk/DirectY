#include "Shaders.h"

void SimpleVertexShader::vertex_shader(Vertex& vertex) {
    vertex.screenPos = _totalTransform * vertex.pos;
}

glm::vec3 CircleFragmentShader::fragment_shader(const FragmentShaderPayload& payload) {
    auto coord = payload.vertex.texCoord;
    if (glm::length(coord - glm::vec2(0.5, 0.5)) < 0.5) {
        return glm::vec3(1);
    }
    return glm::vec3(0);
}

glm::vec3 SimpleFragmentShader::fragment_shader(const FragmentShaderPayload& payload) {
    return payload.vertex.color;
}
