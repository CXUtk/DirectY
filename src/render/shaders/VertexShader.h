#pragma once
#include "../buffers/VertexBuffer.h"
#include <glm/glm.hpp>
class VertexShader {
public:
    VertexShader() {}
    virtual void vertex_shader(Vertex& vertex);
};
