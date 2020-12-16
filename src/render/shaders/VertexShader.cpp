#include "VertexShader.h"

void VertexShader::vertex_shader(Vertex& vertex) {
    auto modelview = _viewTransform * _modelTransform;

    vertex.screenPos = _projTransform * modelview * vertex.pos;

    vertex.pos = modelview * vertex.pos;
    auto normalT = glm::transpose(glm::inverse(modelview));
    vertex.normal = glm::vec3(normalT * glm::vec4(vertex.normal, 0));
}
