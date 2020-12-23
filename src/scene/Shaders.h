#pragma once
#include "../render/shaders/FragmentShader.h"
#include "../render/shaders/VertexShader.h"

class SimpleVertexShader : public VertexShader {
public:
    SimpleVertexShader() : VertexShader() {}
    void vertex_shader(Vertex& vertex) override;
};

class CircleFragmentShader : public FragmentShader {
public:
    CircleFragmentShader() {}
    glm::vec3 fragment_shader(const FragmentShaderPayload& payload) override;
};

class SimpleFragmentShader : public FragmentShader {
public:
    SimpleFragmentShader() {}
    glm::vec3 fragment_shader(const FragmentShaderPayload& payload) override;
};
