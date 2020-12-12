#pragma once
#include <memory>
class IndexBuffer {
public:
    IndexBuffer(std::size_t size, unsigned int* data);
    ~IndexBuffer();

    unsigned int* getBuffer() const { return _indexArray; }
    size_t getSize()const { return _size; }

private:
    unsigned int* _indexArray;
    size_t _size;
};
