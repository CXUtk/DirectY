#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(std::size_t size, std::size_t sizePerEntry, void* data) : _sizePerVertex(sizePerEntry), _totalSize(size) {
    _vertexArray = new unsigned char[size];
    memcpy(_vertexArray, data, size);
}

VertexBuffer::~VertexBuffer() {
    delete[] _vertexArray;
}

void VertexBuffer::ChangeBuffer(void* data) {
    memcpy(_vertexArray, data, _totalSize);
}
