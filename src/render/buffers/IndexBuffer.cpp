#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(std::size_t size, unsigned int* data) {
    _indexArray = new unsigned int[size];
    memcpy(_indexArray, data, size);
    _size = size / sizeof(unsigned int);
}

IndexBuffer::~IndexBuffer() {
    delete[] _indexArray;
}
