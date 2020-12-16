#pragma once
#include <memory>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec4 pos;
    glm::vec4 screenPos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;

    Vertex() : pos(0), screenPos(0), color(0), texCoord(0), normal(0) {}
    Vertex(glm::vec4 pos, glm::vec4 screenPos, glm::vec3 color, glm::vec2 texCoord, glm::vec3 normal) :
        pos(pos), screenPos(screenPos), color(color), texCoord(texCoord), normal(normal) {
    }
};


class VertexBuffer {
public:
    VertexBuffer(std::size_t size, std::size_t sizePerEntry, void* data);
    ~VertexBuffer();

    size_t getSizePerVertex() const { return _sizePerVertex; }
    unsigned char* getBuffer()const { return _vertexArray; }

private:
    unsigned char* _vertexArray;
    size_t _sizePerVertex;
};
