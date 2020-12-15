#pragma once
#include <glm/glm.hpp>
#include <vector>
struct LineSegment {
    glm::vec3 start, end;
    LineSegment(const  glm::vec3& start, const  glm::vec3& end) :start(start), end(end) {}
    glm::vec3 interp(float t) { return glm::mix(start, end, t); }
};

struct LineSegment2D {
    glm::vec2 start, end;
    LineSegment2D(const  glm::vec2& start, const  glm::vec2& end) :start(start), end(end) {}
    glm::vec2 interp(float t) { return glm::mix(start, end, t); }
};

struct Plane3D {
    glm::vec4 eqn;
    Plane3D() = default;
    Plane3D(glm::vec4 eqn) : eqn(eqn) {}
    Plane3D(glm::vec3 normal, float d) : eqn(glm::vec4(normal, d)) {}
    Plane3D(glm::vec3 p, glm::vec3 v1, glm::vec3 v2) {
        glm::vec3 normal = glm::cross(v1, v2);
        eqn = glm::vec4(normal, -glm::dot(normal, p));
    }
    float side(glm::vec3 p) const { return glm::dot(glm::vec4(p, 1.0f), eqn); }
};

bool isTopLeftEdge(glm::vec2 v);

float cross2d(glm::vec2 a, glm::vec2 b);


bool plane_segment_intersection(const LineSegment& seg, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, glm::vec3& result);
bool segment_segment_intersection(const LineSegment2D& l1, const LineSegment2D& l2, glm::vec2& t);

std::vector<glm::vec2> cut_polygon(const std::vector<glm::vec2>& poly, const LineSegment2D& line);
