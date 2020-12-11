#include "Renderer.h"
#include <algorithm>


static float rad = 0.0;
void vertex_shader(Vertex& vertex) {
    static glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    static glm::mat4 proj = glm::perspective(glm::pi<float>() / 4, 0.75f, 0.5f, 100.f);
    vertex.pos = proj * view * glm::rotate(-rad, glm::vec3(1, 0, 0)) * vertex.pos;
}

void viewPort_transform(Vertex& vertex, int width, int height) {
    vertex.pos.x /= vertex.pos.w;
    vertex.pos.y /= vertex.pos.w;
    vertex.pos.z /= vertex.pos.w;

    float x = vertex.pos.x;
    float y = vertex.pos.y;
    vertex.pos.x = (x * 0.5 + 0.5) * width;
    vertex.pos.y = (y * 0.5 + 0.5) * height;
}

glm::vec3 fragment_shader(const glm::vec3& pos, const glm::vec3& color, const glm::vec2& texCoord) {
    auto uv = texCoord * 5.f;
    uv.x = std::fmod(uv.x, 1.0);
    uv.y = std::fmod(uv.y, 1.0);
    bool a = uv.x < 0.5;
    bool b = uv.y < 0.5;
    return color * ((a + b == 1) ? glm::vec3(1) : glm::vec3(0.5));
}


Renderer::Renderer(int width, int height) {
    _frameBuffer = new FrameBuffer(width, height);
}

void Renderer::ClearFrameBuffer() {
    _frameBuffer->Clear();
}


void Renderer::Present(HWND hwnd) {
    int width = _frameBuffer->GetWidth();
    int height = _frameBuffer->GetHeight();

    HDC hdc = GetDC(hwnd);
    BITMAPINFO info;
    ZeroMemory(&info, sizeof(BITMAPINFO));
    info.bmiHeader.biBitCount = 24;
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biSizeImage = 0;
    info.bmiHeader.biCompression = BI_RGB;
    StretchDIBits(hdc, 0, 0, width, height, 0, 0, width, height, _frameBuffer->GetBuffer(), &info, DIB_RGB_COLORS, SRCCOPY);
    ReleaseDC(hwnd, hdc);
}

int Renderer::CreateVertexBuffer(size_t size, size_t sizePerVertex, void* data) {
    _vertexBuffers.push_back(std::make_shared<VertexBuffer>(size, sizePerVertex, data));
    return _vertexBuffers.size() - 1;
}

int Renderer::CreateIndexBuffer(size_t size, void* data) {
    _indexBuffers.push_back(std::make_shared<IndexBuffer>(size, data));
    return _indexBuffers.size() - 1;
}

void Renderer::DrawElements(int vbuff, size_t offset, size_t size, Primitives primType) {
    auto vBuffer = _vertexBuffers[vbuff];

    auto sizePerVertex = vBuffer->getSizePerVertex();
    auto buffer = vBuffer->getBuffer();

    auto viewPortWidth = _frameBuffer->GetWidth();
    auto viewPortHeight = _frameBuffer->GetHeight();

    switch (primType) {
    case Primitives::Triangles: {
        for (int i = offset; i < size; i += 3) {
            std::size_t idx = i * sizePerVertex;
            Vertex V[3];
            for (int j = 0; j < 3; j++) {
                memcpy(&V[j], buffer + idx + j * sizePerVertex, sizePerVertex);
                V[j].pos.w = 1.0f;
                vertex_shader(V[j]);
            }
            inner_draw_triangle(V);
        }
        break;
    }
    case Primitives::Lines: {
        for (int i = offset; i < size; i += 2) {
            std::size_t idx = i * sizePerVertex;
            Vertex V[2];
            for (int j = 0; j < 2; j++) {
                memcpy(&V[j], buffer + idx + j * sizePerVertex, sizePerVertex);
                V[j].pos.w = 1.0f;
                vertex_shader(V[j]);
            }
            inner_draw_line(V);
        }
        break;
    }
    default:
        break;
    }
    rad += 0.005f;
}

void Renderer::DrawElementsWithIndex(int vBuff, size_t offset, size_t size, Primitives primType, int idBuff) {
}




bool isTopLeftEdge(glm::vec2 v) {
    return v.y < 0 || (v.y == 0 && v.x < 0);
}

float cross2d(glm::vec2 a, glm::vec2 b) {
    return a.x * b.y - a.y * b.x;
}

void Renderer::inner_draw_triangle(Vertex vertices[3]) {
    int width = _frameBuffer->GetWidth();
    int height = _frameBuffer->GetHeight();
    static Vertex V[32];
    static int indices[32];
    int numV;
    int numIdx = homo_clipping(vertices, V, indices, &numV);
    for (int i = 0; i < numV; i++)
        viewPort_transform(V[i], width, height);
    for (int i = 0; i < numIdx; i += 3)
        rasterize(V[indices[i]], V[indices[i + 1]], V[indices[i + 2]]);
}

void Renderer::inner_draw_line(Vertex vertices[2]) {
}

void Renderer::rasterize(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    int width = _frameBuffer->GetWidth();
    int height = _frameBuffer->GetHeight();

    // Enclose a rectangular area for rasterize
    glm::ivec2 minnpos = glm::ivec2(v1.pos.x, v1.pos.y);
    glm::ivec2 maxxpos = glm::ivec2(v1.pos.x + 0.5, v1.pos.y + 0.5);

    minnpos.x = min(minnpos.x, (int)v2.pos.x);
    maxxpos.x = max(maxxpos.x, (int)(v2.pos.x + 0.5));
    minnpos.y = min(minnpos.y, (int)v2.pos.y);
    maxxpos.y = max(maxxpos.y, (int)(v2.pos.y + 0.5));

    minnpos.x = min(minnpos.x, (int)v3.pos.x);
    maxxpos.x = max(maxxpos.x, (int)(v3.pos.x + 0.5));
    minnpos.y = min(minnpos.y, (int)v3.pos.y);
    maxxpos.y = max(maxxpos.y, (int)(v3.pos.y + 0.5));

    minnpos.x = max(minnpos.x, 0);
    minnpos.y = max(minnpos.y, 0);
    maxxpos.x = min(maxxpos.x, width);
    maxxpos.y = min(maxxpos.y, height);

    glm::vec2 v[3];

    v[0] = v1.pos;
    v[1] = v2.pos;
    v[2] = v3.pos;

    glm::vec2 t[3];
    for (int i = 0; i < 3; i++) t[i] = v[(i + 1) % 3] - v[i];

    glm::mat2 interpTransform(v[1] - v[0], v[2] - v[0]);
    interpTransform = glm::inverse(interpTransform);

    glm::mat3 colorMat(v1.color / v1.pos.w, v2.color / v2.pos.w, v3.color / v3.pos.w);
    glm::mat3 posMat(v1.pos / v1.pos.w, v2.pos / v2.pos.w, v3.pos / v3.pos.w);
    glm::mat3x2 texMat(v1.texCoord / v1.pos.w, v2.texCoord / v2.pos.w, v3.texCoord / v3.pos.w);


    for (int j = minnpos.y; j <= maxxpos.y; j++) {
        for (int i = minnpos.x; i <= maxxpos.x; i++) {
            glm::vec2 pos = glm::vec2(i + 0.5f, j + 0.5f);

            // Counter clockwise culling
            float a = cross2d(t[0], pos - v[0]);
            float b = cross2d(t[1], pos - v[1]);
            float c = cross2d(t[2], pos - v[2]);

            // Top-Left Rule
            bool b1 = (a > 0 || (a == 0 && isTopLeftEdge(t[0])));
            bool b2 = (b > 0 || (b == 0 && isTopLeftEdge(t[1])));
            bool b3 = (c > 0 || (c == 0 && isTopLeftEdge(t[2])));
            bool inside = b1 && b2 && b3;

            // Barycentric calculation
            if (!inside) continue;
            glm::vec2 uv = interpTransform * (pos - v[0]);
            glm::vec3 interp = glm::vec3(1 - uv.x - uv.y, uv.x, uv.y);
            float z = 1 / (interp.x / v1.pos.w + interp.y / v2.pos.w + interp.z / v3.pos.w);
            interp *= z;

            // Check z-buffer
            if (z > _frameBuffer->GetZBuffer(i, j)) continue;

            glm::vec3 color = colorMat * interp;
            glm::vec3 position = posMat * interp;
            glm::vec2 texCoord = texMat * interp;

            _frameBuffer->Write(i, j, fragment_shader(position, color, texCoord));
            _frameBuffer->WriteZBuffer(i, j, z);
        }
    }
}
int Renderer::homo_clipping(Vertex input[3], Vertex* output, int* indices, int* numVertices) {
    memcpy(output, input, sizeof(Vertex) * 3);
    for (int i = 0; i < 3; i++)indices[i] = i;
    *numVertices = 3;
    return 3;
}
