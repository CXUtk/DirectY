#pragma once
#include <memory>
class IndexBuffer {
public:
    IndexBuffer(std::size_t size, void* data);
    ~IndexBuffer();

    unsigned int* getBuffer() const { return _indexArray; }

private:
    unsigned int* _indexArray;
};
