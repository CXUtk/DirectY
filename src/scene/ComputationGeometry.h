#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

struct Line {
    glm::vec2 start, dir;
    Line() {}
    Line(glm::vec2 start, glm::vec2 dir) : start(start), dir(dir) {}
};
