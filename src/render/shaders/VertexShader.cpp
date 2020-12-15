#include "VertexShader.h"

#include <glm/gtx/transform.hpp>
void VertexShader::vertex_shader(Vertex& vertex) {
    static glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 2.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    static glm::mat4 proj = glm::perspective(glm::pi<float>() / 4, 800.f / 600.f, 0.5f, 100.f);
    vertex.pos = proj * view * glm::rotate(0.f, glm::vec3(0, 1, 1)) * vertex.pos;
}
