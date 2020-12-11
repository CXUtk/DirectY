#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::size_t size, void* data) {
    _indexArray = new unsigned int[size];
    memcpy(_indexArray, data, size);
}

IndexBuffer::~IndexBuffer() {
    delete[] _indexArray;
}
