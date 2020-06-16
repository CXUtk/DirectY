#pragma once
#include <glm/glm.hpp>
#include "Texture.h"
struct fragment_shader_payload {
    fragment_shader_payload() {
    }

    fragment_shader_payload(const glm::vec3& col, const glm::vec3& nor, const glm::vec2& tc) :
        color(col), normal(nor), tex_coords(tc) {
    }


    glm::vec3 view_pos;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    Texture* texture;
};
class Shader {
public:
    Shader() {}
    ~Shader() {}
    glm::vec3 fragmentShader(const fragment_shader_payload& payload) const {}
};
