#pragma once
#include <memory>
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
