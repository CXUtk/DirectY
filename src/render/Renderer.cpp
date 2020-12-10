#include "Renderer.h"
#include <algorithm>


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


void vertex_shader(Vertex& vertex) {

}

void viewPort_transform(Vertex& vertex, int width, int height) {
    vertex.pos /= vertex.pos.w;

    float x = vertex.pos.x;
    float y = vertex.pos.y;
    vertex.pos.x = (x * 0.5 + 0.5) * width;
    vertex.pos.y = (y * 0.5 + 0.5) * height;
}

glm::vec3 fragment_shader(glm::vec3& pos, glm::vec3& color) {
    return color;
}

inline float cross2d(glm::vec2 a, glm::vec2 b) {
    return a.x * b.y - a.y * b.x;
}

void Renderer::Draw(int vbuff, size_t offset, size_t size) {
    auto vBuffer = _vertexBuffers[vbuff];

    auto sizePerVertex = vBuffer->getSizePerVertex();
    auto buffer = vBuffer->getBuffer();

    auto viewPortWidth = _frameBuffer->GetWidth();
    auto viewPortHeight = _frameBuffer->GetHeight();

    for (int i = offset; i < size; i += 3) {
        std::size_t idx = i * sizePerVertex;
        Vertex V[3];
        for (int j = 0; j < 3; j++) {
            memcpy(&V[j], buffer + idx + j * sizePerVertex, sizePerVertex);
            V[j].pos.w = 1.0f;

            vertex_shader(V[j]);
            viewPort_transform(V[j], viewPortWidth, viewPortHeight);
        }
        rasterize(V);
    }
}

void Renderer::rasterize(Vertex vertices[3]) {
    int width = _frameBuffer->GetWidth();
    int height = _frameBuffer->GetHeight();

    // Enclose a rectangular area for rasterize
    glm::ivec2 minnpos = glm::ivec2(vertices[0].pos.x, vertices[0].pos.y);
    glm::ivec2 maxxpos = glm::ivec2(vertices[0].pos.x + 0.5, vertices[0].pos.y + 0.5);

    for (int i = 1; i < 3; i++) {
        minnpos.x = min(minnpos.x, (int)vertices[i].pos.x);
        maxxpos.x = max(maxxpos.x, (int)(vertices[i].pos.x + 0.5));
        minnpos.y = min(minnpos.y, (int)vertices[i].pos.y);
        maxxpos.y = max(maxxpos.y, (int)(vertices[i].pos.y + 0.5));
    }
    minnpos.x = max(minnpos.x, 0);
    minnpos.y = max(minnpos.y, 0);
    maxxpos.x = min(maxxpos.x, width);
    maxxpos.y = min(maxxpos.y, height);

    glm::vec2 v[3];

    for (int i = 0; i < 3; i++)
        v[i] = glm::vec2(vertices[i].pos);

    glm::vec2 t[3];
    for (int i = 0; i < 3; i++)
        t[i] = v[(i + 1) % 3] - v[i];

    glm::mat2 interpTransform(v[1] - v[0], v[2] - v[0]);
    interpTransform = glm::inverse(interpTransform);

    glm::mat3 colorMat(vertices[0].color, vertices[1].color, vertices[2].color);
    glm::mat3 posMat(vertices[0].pos, vertices[1].pos, vertices[2].pos);

    for (int j = minnpos.y; j <= maxxpos.y; j++) {
        for (int i = minnpos.x; i <= maxxpos.x; i++) {
            glm::vec2 pos = glm::vec2(i + 0.5, j + 0.5);
            bool can = true;
            for (int s = 0; s < 3; s++) {
                if (cross2d(t[s], pos - v[s]) < 0) {
                    can = false;
                    break;
                }
            }
            if (can) {
                glm::vec2 uv = interpTransform * (pos - v[0]);
                glm::vec3 interp = glm::vec3(1 - uv.x - uv.y, uv.x, uv.y);
                glm::vec3 color = colorMat * interp;
                glm::vec3 position = posMat * interp;
                _frameBuffer->Write(i, j, fragment_shader(position, color));
            }
        }
    }
}
