#include "VertexShader.h"

void VertexShader::vertex_shader(Vertex& vertex) {
    vertex.screenPos = _totalTransform * vertex.pos;
    vertex.pos = _modelTransform * vertex.pos;
    //auto normalT = glm::transpose(glm::inverse(_modelTransform));
    //vertex.normal = glm::vec3(normalT * glm::vec4(vertex.normal, 0));
}
