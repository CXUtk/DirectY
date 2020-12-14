#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <algorithm>

#include <glm/glm.hpp>

#include "../render/Renderer.h"

struct Face {
    int v[3], vt[3], vn[3];
    Face(const std::vector<std::tuple<int, int, int>>& vs) {
        int i = 0;
        for (auto [a, b, c] : vs) {
            v[i] = a;
            vt[i] = b;
            vn[i] = c;
            i++;
        }
    }
};

class ObjLoader {
public:
    void load(const std::string& path);
    std::vector<glm::vec3> Vertices;
    std::vector<glm::vec2> TexCoords;
    std::vector<glm::vec3> Normals;
    std::vector<Face> Triangles;


    std::vector<Vertex> getVertices() const;

private:
    static constexpr int MAX_BUFFER = 100005;
    static char lineBuffer[MAX_BUFFER];
    int _ptr;
    void process();
    char getNext() { return lineBuffer[++_ptr]; }
    char getCur() { return lineBuffer[_ptr]; }
    char getPrev() { return lineBuffer[--_ptr]; }
    void delSpace() {
        char c = lineBuffer[_ptr];
        while (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
            c = getNext();
        }
    }
};
