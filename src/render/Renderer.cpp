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

float cross2d(glm::vec2 a, glm::vec2 b) {
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

            // printf("T%d, C%d: %lf %lf %lf\n", i, j, V[j].pos.x, V[j].pos.y, V[j].pos.z);
        }

        rasterize(V);
    }
    rad += 0.005f;
}

bool isTopLeftEdge(glm::vec2 v) {
    return v.y < 0 || (v.y == 0 && v.x < 0);
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

    glm::mat3 colorMat(vertices[0].color / vertices[0].pos.w, vertices[1].color / vertices[1].pos.w, vertices[2].color / vertices[2].pos.w);
    glm::mat3 posMat(vertices[0].pos / vertices[0].pos.w, vertices[1].pos / vertices[1].pos.w, vertices[2].pos / vertices[2].pos.w);
    glm::mat3x2 texMat(vertices[0].texCoord / vertices[0].pos.w, vertices[1].texCoord / vertices[1].pos.w, vertices[2].texCoord / vertices[2].pos.w);
    int cnt = 0;
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
            float z = 1 / (interp.x / vertices[0].pos.w + interp.y / vertices[1].pos.w + interp.z / vertices[2].pos.w);
            // Check -z because the default depth is 0
            if (z > _frameBuffer->GetZBuffer(i, j)) continue;

            glm::vec3 color = colorMat * interp * z;
            glm::vec3 position = posMat * interp * z;
            glm::vec2 texCoord = texMat * interp * z;
            //if (i == 300 && j == 374) {
            //    printf("%lf %lf %lf\n", a, b, c);
            //}
            _frameBuffer->Write(i, j, fragment_shader(position, color, texCoord));
            _frameBuffer->WriteZBuffer(i, j, z);
            cnt++;
        }
    }
}
