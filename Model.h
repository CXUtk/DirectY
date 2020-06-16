#pragma once
#include <vector>
#include <glm\glm.hpp>
#include "Triangle.h"
#include "Shader.h"

typedef glm::vec3(*fragment_shader)(const fragment_shader_payload& payload);
class Model {
public:
    fragment_shader _shader;

    Model(const std::vector<Triangle>& triangles);
    ~Model();

    void SetTransform(const glm::mat4& matrix) { _transform = matrix; }
    std::vector<Triangle> getTriangles() const {
        std::vector<Triangle> res;
        for (const auto& t : _triangles) {
            Triangle tr = t;
            for (auto& v : tr.vs)
                v = _transform * v;
            for (auto& v : tr.normals)
                v = _transform * glm::vec4(v, 0);
            res.push_back(tr);
        }
        return res;
    }
    void SetColor(const glm::vec3& color) {
        for (auto& t : _triangles) {
            for (int i = 0; i < 3; i++)
                t.color[i] = color;
        }
    }

private:
    glm::mat4 _transform;
    std::vector<Triangle> _triangles;

};
