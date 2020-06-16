#pragma once
#include <glm/glm.hpp>
class Triangle {
public:
    glm::vec4 vs[3];
    glm::vec3 color[3];
    glm::vec2 tex_coords[3];
    glm::vec3 normals[3];
};
