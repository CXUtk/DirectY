#include "VertexShader.h"

void VertexShader::vertex_shader(Vertex& vertex) {
    vertex.pos = _viewProjTransform * vertex.pos;
}
