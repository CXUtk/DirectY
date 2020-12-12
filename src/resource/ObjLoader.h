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
    int v[3];
    Face(int a, int b, int c) {
        v[0] = a, v[1] = b, v[2] = c;
    }
};

class ObjLoader {
public:
    void load(const std::string& path);
    std::vector<Vertex> Vertices;
    std::vector<Face> Triangles;

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
