#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>
#include <vector>
#include <Windows.h>

#include "FrameBuffer.h"
#include "VertexBuffer.h"

struct Vertex {
    glm::vec4 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    Vertex() : pos(0), color(0), texCoord(0) {}
    Vertex(glm::vec4 pos, glm::vec3 color, glm::vec2 texCoord) :pos(pos), color(color), texCoord(texCoord) {}
};


class Renderer {
public:
    Renderer(int width, int height);

    void SetFrameBuffer(FrameBuffer* frameBuffer) { _frameBuffer = frameBuffer; }

    void ClearFrameBuffer();

    void Present(HWND hwnd);

    /// <summary>
    /// Create a vertex buffer
    /// </summary>
    /// <param name="size">Total bytes of the buffer</param>
    /// <param name="sizePerVertex">Bytes per object</param>
    /// <param name="data">Pointer to data</param>
    /// <returns></returns>
    int CreateVertexBuffer(size_t size, size_t sizePerVertex, void* data);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="vbuff"></param>
    /// <param name="offset"></param>
    /// <param name="size">Number of objects from offset to draw</param>
    void Draw(int vbuff, size_t offset, size_t size);

private:
    FrameBuffer* _frameBuffer;
    std::vector<std::shared_ptr<VertexBuffer>> _vertexBuffers;

    void rasterize(Vertex vertices[3]);
};
