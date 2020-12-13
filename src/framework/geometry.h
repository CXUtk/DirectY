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

bool isTopLeftEdge(glm::vec2 v);

float cross2d(glm::vec2 a, glm::vec2 b);


bool plane_segment_intersection(const LineSegment& seg, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, glm::vec3& result);
bool segment_segment_intersection(const LineSegment2D& l1, const LineSegment2D& l2, glm::vec2& t);

std::vector<glm::vec2> cut_polygon(const std::vector<glm::vec2>& poly, const LineSegment2D& line);
