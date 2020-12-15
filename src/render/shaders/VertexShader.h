#pragma once
#include "../buffers/VertexBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
class VertexShader {
public:
    VertexShader() : _viewProjTransform(glm::identity<glm::mat4>()) {}
    virtual void vertex_shader(Vertex& vertex);
    void SetViewProjTransform(glm::mat4 viewProj) {
        _viewProjTransform = viewProj;
    }
private:
    glm::mat4 _viewProjTransform;
};
