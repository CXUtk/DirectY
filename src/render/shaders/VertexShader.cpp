#include "VertexShader.h"

void VertexShader::vertex_shader(Vertex& vertex) {
    vertex.pos = _modelTransform * vertex.pos;
    vertex.screenPos = _totalTransform * vertex.pos;

    //auto normalT = glm::transpose(glm::inverse(_modelTransform));
    vertex.normal = glm::vec3(_normalTransform * glm::vec4(vertex.normal, 0));
}
