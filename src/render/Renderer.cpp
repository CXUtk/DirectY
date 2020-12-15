#include "Renderer.h"
#include "../framework/geometry.h"
#include <algorithm>


static float rad = 0;
static constexpr float EPS = 1e-7f;
void vertex_shader(Vertex& vertex) {
    static glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 2.0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    static glm::mat4 proj = glm::perspective(glm::pi<float>() / 4, 800.f / 600.f, 0.5f, 100.f);
    vertex.pos = proj * view * glm::rotate(-rad, glm::vec3(0, 1, 1)) * vertex.pos;
}

void viewPort_transform(Vertex& vertex, int width, int height) {

    float x = vertex.pos.x;
    float y = vertex.pos.y;
    vertex.pos.x = (x * 0.5f + 0.5f) * width;
    vertex.pos.y = (y * 0.5f + 0.5f) * height;

    //assert(vertex.pos.x >= 0 && vertex.pos.x <= width);
    //assert(vertex.pos.y >= 0 && vertex.pos.y <= height);
}

glm::vec3 fragment_shader(const glm::vec4& pos, const glm::vec3& color, const glm::vec2& texCoord, const glm::vec3& normal) {
    auto uv = texCoord * 5.f;
    uv.x = std::fmod(uv.x, 1.0);
    uv.y = std::fmod(uv.y, 1.0);
    bool a = uv.x < 0.5;
    bool b = uv.y < 0.5;
    return color * ((a + b == 1) ? glm::vec3(0.5) : glm::vec3(1));
}


Renderer::Renderer(int width, int height, std::shared_ptr<GraphicDevice> graphicDevice) {
    _frameBuffer = new FrameBuffer(width, height);
    _graphicDevice = graphicDevice;
    _drawMode = DrawMode::Fill;
    _cullMode = CullMode::None;
}

Renderer::~Renderer() {
    delete _frameBuffer;
}

void Renderer::ClearFrameBuffer() {
    _frameBuffer->Clear();
}


void Renderer::Present() {
    _graphicDevice->Present(_frameBuffer);
}

int Renderer::CreateVertexBuffer(size_t size, size_t sizePerVertex, void* data) {
    _vertexBuffers.push_back(std::make_shared<VertexBuffer>(size, sizePerVertex, data));
    return _vertexBuffers.size() - 1;
}

int Renderer::CreateIndexBuffer(size_t size, void* data) {
    _indexBuffers.push_back(std::make_shared<IndexBuffer>(size, (unsigned int*)data));
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
    //rad += 0.008f;
}

void Renderer::DrawElementsWithIndex(int vBuff, size_t offset, size_t size, Primitives primType, int idBuff) {
    auto vBuffer = _vertexBuffers[vBuff];
    auto iBuffer = _indexBuffers[idBuff];

    auto sizePerVertex = vBuffer->getSizePerVertex();
    auto buffer = vBuffer->getBuffer();

    auto viewPortWidth = _frameBuffer->GetWidth();
    auto viewPortHeight = _frameBuffer->GetHeight();

    auto idArray = iBuffer->getBuffer();
    switch (primType) {
    case Primitives::Triangles: {
        for (int i = 0; i < iBuffer->getSize(); i += 3) {
            Vertex V[3];
            for (int j = 0; j < 3; j++) {
                memcpy(&V[j], buffer + (idArray[i + j]) * sizePerVertex, sizePerVertex);
                V[j].pos.w = 1.0f;
                vertex_shader(V[j]);
            }
            inner_draw_triangle(V);

        }
        break;
    }
    case Primitives::Lines: {
        for (int i = offset; i < size; i += 2) {
            Vertex V[2];
            for (int j = 0; j < 2; j++) {
                memcpy(&V[j], buffer + (idArray[i + j]) * sizePerVertex, sizePerVertex);
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

}



void Renderer::inner_draw_triangle(Vertex vertices[3]) {
    _numRaster++;

    int width = _frameBuffer->GetWidth();
    int height = _frameBuffer->GetHeight();
    static Vertex V[32];
    static int indices[32];
    int numV;
    int numIdx = homo_clipping(vertices, V, indices, numV);
    // printf("************\n");
    for (int i = 0; i < numV; i++) {
        // printf("%lf %lf %lf %lf\n", V[i].pos.x, V[i].pos.y, V[i].pos.z, V[i].pos.w);
        viewPort_transform(V[i], width, height);
    }
    for (int i = 0; i < numIdx; i += 3) {
        if (_drawMode == DrawMode::Fill) {
            rasterize(V[indices[i]], V[indices[i + 1]], V[indices[i + 2]]);
        }
        else if (_drawMode == DrawMode::WireFrame) {
            for (int j = 0; j < 3; j++) {
                bresenham(&V[indices[i + j]], &V[indices[i + (j + 1) % 3]]);
            }
        }
    }
}



void Renderer::inner_draw_line(Vertex vertices[2]) {
    int width = _frameBuffer->GetWidth();
    int height = _frameBuffer->GetHeight();

    Vertex tmp[2];
    memcpy(tmp, vertices, sizeof(tmp));

    glm::vec3 v1 = vertices[0].pos / vertices[0].pos.w;
    glm::vec3 v2 = vertices[1].pos / vertices[1].pos.w;

    glm::vec3 dir = v2 - v1;

    glm::vec3 minn = glm::vec3(-1);
    glm::vec3 maxx = glm::vec3(1);

    // Homogenous space clip of lines, range from [-1, 1]
    float tMin = 0.0f, tMax = 1.0f;
    for (int i = 0; i < 3; i++) {
        float invD = 1.0f / dir[i];
        float t0 = (minn[i] - v1[i]) * invD;
        float t1 = (maxx[i] - v1[i]) * invD;
        if (invD < 0) std::swap(t0, t1);
        tMin = std::max(tMin, t0);
        tMax = std::min(tMax, t1);
        if (tMin >= tMax) return;
    }
    vertices[0] = linear_interpolation_Perspect(tmp[0], tmp[1], tMin);
    vertices[1] = linear_interpolation_Perspect(tmp[0], tmp[1], tMax);

    for (int i = 0; i < 2; i++)
        viewPort_transform(vertices[i], width, height);

    bresenham(&vertices[0], &vertices[1]);
}

void Renderer::rasterize(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    //_numRaster++;
    int width = _frameBuffer->GetWidth();
    int height = _frameBuffer->GetHeight();

    // Enclose a rectangular area for rasterize
    glm::ivec2 minnpos = glm::ivec2(v1.pos.x, v1.pos.y);
    glm::ivec2 maxxpos = glm::ivec2(v1.pos.x + 0.5, v1.pos.y + 0.5);

    minnpos.x = std::min(minnpos.x, (int)v2.pos.x);
    maxxpos.x = std::max(maxxpos.x, (int)(v2.pos.x + 0.5));
    minnpos.y = std::min(minnpos.y, (int)v2.pos.y);
    maxxpos.y = std::max(maxxpos.y, (int)(v2.pos.y + 0.5));

    minnpos.x = std::min(minnpos.x, (int)v3.pos.x);
    maxxpos.x = std::max(maxxpos.x, (int)(v3.pos.x + 0.5));
    minnpos.y = std::min(minnpos.y, (int)v3.pos.y);
    maxxpos.y = std::max(maxxpos.y, (int)(v3.pos.y + 0.5));

    minnpos.x = std::max(minnpos.x, 0);
    minnpos.y = std::max(minnpos.y, 0);
    maxxpos.x = std::min(maxxpos.x, width);
    maxxpos.y = std::min(maxxpos.y, height);

    glm::vec2 v[3];

    v[0] = v1.pos;
    v[1] = v2.pos;
    v[2] = v3.pos;

    glm::vec2 t[3];
    for (int i = 0; i < 3; i++) t[i] = v[(i + 1) % 3] - v[i];

    glm::mat2 interpTransform(v[1] - v[0], v[2] - v[0]);
    interpTransform = glm::inverse(interpTransform);

    if (std::isnan(interpTransform[0][0]) || std::isnan(interpTransform[0][1]) || std::isnan(interpTransform[1][0]) || std::isnan(interpTransform[1][1])) {
        /*printf("%lf %lf, %lf %lf, %lf %lf\n", v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y);*/
        return;
    }
    glm::mat3x3 colorMat(v1.color / v1.pos.w, v2.color / v2.pos.w, v3.color / v3.pos.w);
    glm::mat3x4 posMat(v1.pos / v1.pos.w, v2.pos / v2.pos.w, v3.pos / v3.pos.w);
    glm::mat3x2 texMat(v1.texCoord / v1.pos.w, v2.texCoord / v2.pos.w, v3.texCoord / v3.pos.w);
    glm::mat3x3 normalMat(v1.normal / v1.pos.w, v2.normal / v2.pos.w, v3.normal / v3.pos.w);

    for (int j = minnpos.y; j <= maxxpos.y; j++) {
        for (int i = minnpos.x; i <= maxxpos.x; i++) {
            glm::vec2 pos = glm::vec2(i + 0.5f, j + 0.5f);

            // Barycentric calculation
            glm::vec2 uv = interpTransform * (pos - v[0]);


            if (isnan(uv.x) || isnan(uv.y) || uv.x < -EPS || uv.x > 1 + EPS || uv.y < -EPS || uv.y > 1 + EPS || uv.x + uv.y > 1 + EPS) continue;
            glm::vec3 interp = glm::vec3(1 - uv.x - uv.y, uv.x, uv.y);

            bool b1 = (std::abs(interp.x) > EPS || (std::abs(interp.x) < EPS && isTopLeftEdge(t[1])));
            bool b2 = (std::abs(interp.y) > EPS || (std::abs(interp.y) < EPS && isTopLeftEdge(t[2])));
            bool b3 = (std::abs(interp.z) > EPS || (std::abs(interp.z) < EPS && isTopLeftEdge(t[0])));
            if (!b1 || !b2 || !b3)continue;
            float z = 1 / (interp.x / v1.pos.w + interp.y / v2.pos.w + interp.z / v3.pos.w);
            interp *= z;

            // Check z-buffer
            if (z > _frameBuffer->GetZBuffer(i, j)) continue;

            glm::vec3 color = colorMat * interp;
            glm::vec4 position = posMat * interp;
            glm::vec2 texCoord = texMat * interp;
            glm::vec3 normal = normalMat * interp;

            _frameBuffer->Write(i, j, fragment_shader(position, color, texCoord, normal));
            _frameBuffer->WriteZBuffer(i, j, z);
        }
    }
}
void Renderer::bresenham(const Vertex* v1, const Vertex* v2) {
    glm::ivec2 start(v1->pos.x + 0.5, v1->pos.y + 0.5), end(v2->pos.x + 0.5, v2->pos.y + 0.5);

    bool swp = false;
    int dx = end.x - start.x, dy = end.y - start.y;
    int yInc = 1;
    if (abs(dx) < abs(dy)) {
        std::swap(dx, dy);
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
        swp = true;
    }
    if (dx < 0) {
        std::swap(start.x, end.x);
        std::swap(start.y, end.y);
        dx *= -1, dy *= -1;
        std::swap(v1, v2);
    }
    if (dy < 0) {
        yInc = -1;
        dy *= -1;
    }
    int a = 2 * dy, b = 2 * (dy - dx);
    int cur = 2 * dy - dx;

    glm::vec4 posM[2] = { v1->pos / v1->pos.w, v2->pos / v2->pos.w };
    glm::vec3 colorM[2] = { v1->color / v1->pos.w, v2->color / v2->pos.w };
    //glm::vec2 texCoord[2] = { v1->texCoord / v1->pos.w, v2->texCoord / v2->pos.w };
    //glm::vec3 normal[2] = { v1->normal / v1->pos.w, v2->normal / v2->pos.w };
    for (int i = start.x, j = start.y; i <= end.x; i++) {
        float t = (i - start.x) / (float)(end.x - start.x);
        float z = 1 / ((1 - t) / v1->pos.w + t / v2->pos.w);
        glm::vec3 color = glm::mix(colorM[0], colorM[1], t) * z /** (-glm::mix(posM[0], posM[1], t).z * 0.5f + 0.5f)*/;
        if (swp) {
            if (_frameBuffer->GetZBuffer(j, i) >= z) {
                _frameBuffer->Write(j, i, color);/*fragment_shader(position, color, texCoord)*/
                _frameBuffer->WriteZBuffer(j, i, z);
            }
        }
        else {
            if (_frameBuffer->GetZBuffer(i, j) >= z) {
                _frameBuffer->Write(i, j, color);
                _frameBuffer->WriteZBuffer(i, j, z);
            }
        }
        if (cur <= 0) {
            cur += a;
        }
        else {
            j += yInc;
            cur += b;
        }
    }
}



static LineSegment cubeSeg[12] = {
    // Front 4 edges
    LineSegment(glm::vec3(-1, -1, -1), glm::vec3(1, -1, -1)),
    LineSegment(glm::vec3(1, -1, -1), glm::vec3(1, 1, -1)),
    LineSegment(glm::vec3(1, 1, -1), glm::vec3(-1, 1, -1)),
    LineSegment(glm::vec3(-1, 1, -1), glm::vec3(-1, -1, -1)),

    // Back 4 edges
    LineSegment(glm::vec3(-1, -1, 1), glm::vec3(1, -1, 1)),
    LineSegment(glm::vec3(1, -1, 1), glm::vec3(1, 1, 1)),
    LineSegment(glm::vec3(1, 1, 1), glm::vec3(-1, 1, 1)),
    LineSegment(glm::vec3(-1, 1, 1), glm::vec3(-1, -1, 1)),

    // Middle 4 edges
    LineSegment(glm::vec3(-1, -1, -1), glm::vec3(-1, -1, 1)),
    LineSegment(glm::vec3(1, -1, -1), glm::vec3(1, -1, 1)),
    LineSegment(glm::vec3(1, 1, -1), glm::vec3(1, 1, 1)),
    LineSegment(glm::vec3(-1, 1, -1), glm::vec3(-1, 1, 1)),
};

static Plane3D unitPlanes[6] = {
    // Near
    Plane3D(glm::vec4(0, 0, 1, 1)),
    // Far
    Plane3D(glm::vec4(0, 0, -1, 1)),
    // Left
    Plane3D(glm::vec4(1, 0, 0, 1)),
    // Right
    Plane3D(glm::vec4(-1, 0, 0, 1)),
    // Top
    Plane3D(glm::vec4(0, -1, 0, 1)),
    // Bottom
    Plane3D(glm::vec4(0, 1, 0, 1)),
};

// 齐次坐标剪裁算法
int Renderer::homo_clipping(Vertex input[3], Vertex* output, int* indices, int& numVertices) {
    //numVertices = 3;
    //memcpy(output, input, sizeof(Vertex) * 3);
    //for (int i = 0; i < numVertices; i++) {
    //    output[i].pos.x /= input[i].pos.w;
    //    output[i].pos.y /= input[i].pos.w;
    //    output[i].pos.z /= input[i].pos.w;
    //}
    //for (int i = 0; i < 3; i++) indices[i] = i;
    //return 3;
    static glm::vec3 initial[3];
    for (int i = 0; i < 3; i++) {
        initial[i] = glm::vec3(input[i].pos / input[i].pos.w);
    }
    numVertices = 0;
    if (_drawMode == DrawMode::Fill && !backFaceCulling(initial[0], initial[1], initial[2])) return 0;
    auto cliptype = need_clipping(initial);
    if (cliptype == ClippingType::AllOut) {
        return 0;
    }
    else if (cliptype == ClippingType::AllIn) {
        numVertices = 3;
        memcpy(output, input, sizeof(Vertex) * 3);
        for (int i = 0; i < numVertices; i++) {
            output[i].pos.x /= input[i].pos.w;
            output[i].pos.y /= input[i].pos.w;
            output[i].pos.z /= input[i].pos.w;
        }
        for (int i = 0; i < 3; i++) indices[i] = i;
        return 3;
    }
    std::vector<glm::vec2> points;
    // 求出三角形所在平面和立方体的截面，并进行基底变换投影到2维平面上
    // 求出的点集一定构成一个凸多边形
    for (int i = 0; i < 12; i++) {
        glm::vec3 parameter;
        if (plane_segment_intersection(cubeSeg[i], initial[0], initial[1], initial[2], parameter)) {
            if (parameter.z == 0 || parameter.z == 1) continue;

            points.push_back(glm::vec2(parameter.x, parameter.y));
        }
    }
    // 如果是非退化情况，将此凸多边形和三角形进行半平面交剪裁
    if (!points.empty()) {

        std::sort(points.begin(), points.end(), [](glm::vec2 a, glm::vec2 b) {
            return a.y < b.y || (a.y == b.y && a.x < b.x);
            });
        glm::vec2 origin = points[0];
        std::sort(points.begin(), points.end(), [=](glm::vec2 a, glm::vec2 b) {
            return cross2d(a - origin, b - origin) > 0;
            });
        //if (_numRaster == 3) {
        //    for (auto p : points) {
        //        printf("%lf %lf\n", p.x, p.y);
        //    }
        //}
        points = cut_polygon(points, LineSegment2D(glm::vec2(0, 0), glm::vec2(1, 0)));
        points = cut_polygon(points, LineSegment2D(glm::vec2(1, 0), glm::vec2(0, 1)));
        points = cut_polygon(points, LineSegment2D(glm::vec2(0, 1), glm::vec2(0, 0)));

        int tot = 0;
        for (auto p : points) {
            // printf("%lf %lf\n", p.x, p.y);
            output[tot++] = barycentric_interpolation_Perspect(input[0], input[1], input[2], glm::vec3(1 - p.x - p.y, p.x, p.y));
        }
        numVertices = tot;
        int inds = 0;
        for (int i = 1; i < numVertices - 1; i++) {
            indices[inds++] = 0;
            indices[inds++] = i;
            indices[inds++] = i + 1;
        }
        return inds;
    }
    else {
        numVertices = 3;
        memcpy(output, input, sizeof(Vertex) * 3);
        for (int i = 0; i < numVertices; i++) {
            output[i].pos.x /= input[i].pos.w;
            output[i].pos.y /= input[i].pos.w;
            output[i].pos.z /= input[i].pos.w;
        }
        for (int i = 0; i < 3; i++) indices[i] = i;
        return 3;
    }
}

ClippingType Renderer::need_clipping(glm::vec3 v[3]) const {
    bool inside = true;
    for (int i = 0; i < 3; i++) {
        if (v[i].x < -1 || v[i].x > 1 || v[i].y < -1 || v[i].y > 1 || v[i].z < -1 || v[i].z > 1) {
            inside = false;
            break;
        }
    }
    if (inside) return ClippingType::AllIn;
    for (int i = 0; i < 6; i++) {
        bool allOut = true;
        for (int j = 0; j < 3; j++) {
            if (unitPlanes[i].side(v[j]) >= 0) {
                allOut = false;
                break;
            }
        }
        if (allOut) return ClippingType::AllOut;
    }
    return ClippingType::NeedClip;
}

bool Renderer::backFaceCulling(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) const {
    glm::vec3 normal = glm::cross(v2 - v1, v3 - v1);
    static const glm::vec3 eyeDir = glm::vec3(0, 0, 1);
    // Select Cull Mode
    if (_cullMode == CullMode::CullClockwise)
        return glm::dot(eyeDir, normal) >= 0;
    else if (_cullMode == CullMode::CullCounterClockwise)
        return glm::dot(eyeDir, normal) <= 0;
    return true;
}

Vertex Renderer::linear_interpolation_Perspect(const Vertex& v1, const Vertex& v2, float t) const {
    Vertex v;
    float z = 1 / ((1 - t) / v1.pos.w + t / v2.pos.w);
    v.pos = glm::mix(v1.pos / v1.pos.w, v2.pos / v2.pos.w, t) * z; // tmp[0].pos* (1 - tMin) + tmp[1].pos * tMin;
    v.color = glm::mix(v1.color / v1.pos.w, v2.color / v2.pos.w, t) * z;
    v.texCoord = glm::mix(v1.texCoord / v1.pos.w, v2.texCoord / v2.pos.w, t) * z;
    v.normal = glm::mix(v1.normal / v1.pos.w, v2.normal / v2.pos.w, t) * z;

    v.pos.x /= v.pos.w;
    v.pos.y /= v.pos.w;
    v.pos.z /= v.pos.w;
    return v;
}

Vertex Renderer::barycentric_interpolation_Perspect(const Vertex& v1, const Vertex& v2, const Vertex& v3, glm::vec3 bary) const {
    Vertex v;
    float z = 1 / (bary.x / v1.pos.w + bary.y / v2.pos.w + bary.z / v3.pos.w);
    bary *= z;

    v.pos = glm::mat3x4(v1.pos / v1.pos.w, v2.pos / v2.pos.w, v3.pos / v3.pos.w) * bary;
    v.color = glm::mat3x3(v1.color / v1.pos.w, v2.color / v2.pos.w, v3.color / v3.pos.w) * bary;
    v.texCoord = glm::mat3x2(v1.texCoord / v1.pos.w, v2.texCoord / v2.pos.w, v3.texCoord / v3.pos.w) * bary;
    v.normal = glm::mat3x3(v1.normal / v1.pos.w, v2.normal / v2.pos.w, v3.normal / v3.pos.w) * bary;

    v.pos.x /= v.pos.w;
    v.pos.y /= v.pos.w;
    v.pos.z /= v.pos.w;
    return v;
}
