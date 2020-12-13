#include "geometry.h"

bool isTopLeftEdge(glm::vec2 v) {
    return v.y < 0 || (v.y == 0 && v.x < 0);
}

float cross2d(glm::vec2 a, glm::vec2 b) {
    return a.x * b.y - a.y * b.x;
}

bool plane_segment_intersection(const LineSegment& seg, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, glm::vec3& result) {
    glm::vec3 augment = seg.start - A;
    glm::mat3 M(B - A, C - A, seg.start - seg.end);
    M = glm::inverse(M);
    glm::vec3 res = M * augment;
    if (isnan(res.z) || res.z < 0 || res.z > 1) return false;
    result = res;
    return true;
}

bool segment_segment_intersection(const LineSegment2D& l1, const LineSegment2D& l2, glm::vec2& t) {
    glm::vec2 augment = l2.start - l1.start;
    glm::mat2 M(l1.end - l1.start, l2.start - l2.end);
    M = glm::inverse(M);
    t = M * augment;
    if (isnan(t.x) || isnan(t.y)) return false;
    return true;
}

std::vector<glm::vec2> cut_polygon(const std::vector<glm::vec2>& poly, const LineSegment2D& line) {
    std::vector<glm::vec2> newpoly;
    int sz = poly.size();
    for (int i = 0; i < sz; i++) {
        glm::vec2 A = poly[i];
        glm::vec2 B = poly[(i + 1) % sz];
        if (cross2d(line.end - line.start, A - line.start) >= 0) newpoly.push_back(A);
        LineSegment2D L(A, B);
        glm::vec2 test;
        if (segment_segment_intersection(LineSegment2D(line.start, line.end), L, test)) {
            if (test.y <= 0 || test.y >= 1) continue;
            newpoly.push_back(L.interp(test.y));

        }
    }
    return newpoly;
}
