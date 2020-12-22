#include "VertexShader.h"

void VertexShader::vertex_shader(Vertex& vertex) {
    vertex.pos = _modelTransform * vertex.pos;

    //static glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 0.5), glm::vec3(0), glm::vec3(0, 1, 0));
    //static glm::mat4 proj = glm::ortho(0.f, 800.f, 0.f, 600.f);
    //static glm::mat4 model = glm::translate(glm::vec3(400, 300, 0)) * glm::scale(glm::vec3(400, 300, 1));
    vertex.screenPos = _totalTransform * vertex.pos;

    //auto normalT = glm::transpose(glm::inverse(_modelTransform));
    vertex.normal = glm::vec3(_normalTransform * glm::vec4(vertex.normal, 0));
}
