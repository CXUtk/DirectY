#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>
#include <vector>

#include "device/GraphicDevice.h"
#include "buffers/FrameBuffer.h"
#include "buffers/VertexBuffer.h"
#include "buffers/IndexBuffer.h"

enum class Primitives {
    Points,
    Lines,
    Triangles,
};

enum class DrawMode {
    WireFrame,
    Fill,
};

enum class CullMode {
    None,
    CullClockwise,
    CullCounterClockwise,
};

struct Vertex {
    glm::vec4 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    Vertex() : pos(0), color(0), texCoord(0) {}
    Vertex(glm::vec4 pos, glm::vec3 color, glm::vec2 texCoord) :pos(pos), color(color), texCoord(texCoord) {}
};


class Renderer {
public:
    Renderer(int width, int height, std::shared_ptr<GraphicDevice> graphicDevice);
    ~Renderer();

    void SetFrameBuffer(FrameBuffer* frameBuffer) { _frameBuffer = frameBuffer; }
    void SetDrawMode(DrawMode drawMode) { _drawMode = drawMode; }

    void ClearFrameBuffer();

    void Present();

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
    /// <param name="size"></param>
    /// <param name="data"></param>
    /// <returns></returns>
    int CreateIndexBuffer(size_t size, void* data);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="vbuff"></param>
    /// <param name="offset"></param>
    /// <param name="size">Number of objects to draw from offset</param>
    void DrawElements(int vbuff, size_t offset, size_t size, Primitives primType);

    void DrawElementsWithIndex(int vBuff, size_t offset, size_t size, Primitives primType, int idBuff);

private:
    FrameBuffer* _frameBuffer;
    std::vector<std::shared_ptr<VertexBuffer>> _vertexBuffers;
    std::vector<std::shared_ptr<IndexBuffer>> _indexBuffers;
    std::shared_ptr<GraphicDevice> _graphicDevice;

    DrawMode _drawMode;
    CullMode _cullMode;

    void inner_draw_triangle(Vertex vertices[3]);
    void inner_draw_line(Vertex vertices[2]);

    void rasterize(const Vertex& v1, const Vertex& v2, const Vertex& v3);
    void bresenham(const Vertex* v1, const Vertex* v2);

    int homo_clipping(Vertex input[3], Vertex* output, int* indices, int* numV);

    Vertex linear_interpolation(const Vertex& v1, const Vertex& v2, float t) const;
    Vertex barycentric_interpolation(const Vertex& v1, const Vertex& v2, const Vertex& v3, const glm::vec3& bary) const;
};
