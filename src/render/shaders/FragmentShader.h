#pragma once
#include "../buffers/VertexBuffer.h"
#include <glm/glm.hpp>
#include <map>
#include <string>
struct FragmentShaderPayload {
    Vertex vertex;
};

class FragmentShader {
public:
    FragmentShader() {}
    virtual glm::vec3 fragment_shader(const FragmentShaderPayload& payload);
};
