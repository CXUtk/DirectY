#pragma once
#include "../buffers/VertexBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
class VertexShader {
public:
    VertexShader() : _viewTransform(glm::identity<glm::mat4>()), _projTransform(glm::identity<glm::mat4>()), _modelTransform(glm::identity<glm::mat4>()) {}
    virtual void vertex_shader(Vertex& vertex);
    void SetModelTransform(glm::mat4 model) {
        _modelTransform = model;
    }
    void SetViewTransform(glm::mat4 view) {
        _viewTransform = view;
    }
    void SetProjTransform(glm::mat4 proj) {
        _projTransform = proj;
    }
    void apply() {
        _totalTransform = _projTransform * _viewTransform;
        _normalTransform = glm::transpose(glm::inverse(_modelTransform));
    }
private:
    // 这些都相当于shader中的uniform变量
    glm::mat4 _viewTransform;
    glm::mat4 _projTransform;
    glm::mat4 _modelTransform;

    glm::mat4 _totalTransform;
    glm::mat4 _normalTransform;
};
