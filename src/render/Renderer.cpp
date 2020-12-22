#include "Renderer.h"
#include "../framework/geometry.h"
#include <algorithm>


static float rad = 0;
static constexpr float EPS = 1e-7f;

void viewPort_transform(Vertex& vertex, int width, int height) {

    float x = vertex.screenPos.x;
    float y = vertex.screenPos.y;
    vertex.screenPos.x = (int)((x * 0.5f + 0.5f) * (width - 1) + 0.5f);
    vertex.screenPos.y = (int)((y * 0.5f + 0.5f) * (height - 1) + 0.5f);

    //assert(vertex.pos.x >= 0 && vertex.pos.x <= width);
    //assert(vertex.pos.y >= 0 && vertex.pos.y <= height);
}


Renderer::Renderer(int width, int height, std::shared_ptr<GraphicDevice> graphicDevice) {


    //_sampleConfig.samplePoints[0] = glm::vec2(0.5, 0.5);
    //_sampleConfig.sampleSize = 1;
    _sampleConfig.samplePoints[0] = glm::vec2(0.125, 0.625);
    _sampleConfig.samplePoints[1] = glm::vec2(0.325, 0.125);
    _sampleConfig.samplePoints[2] = glm::vec2(0.625, 0.875);
    _sampleConfig.samplePoints[3] = glm::vec2(0.875, 0.325);
    _sampleConfig.sampleSize = 4;

    _frameBuffer = new FrameBuffer(width, height, _sampleConfig.sampleSize);
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
                _vertexShader->vertex_shader(V[j]);
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
                _vertexShader->vertex_shader(V[j]);
            }
            inner_draw_line(V);
        }
        break;
    }
    default:
        break;
    }
    rad += 0.008f;
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
                _vertexShader->vertex_shader(V[j]);
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
                _vertexShader->vertex_shader(V[j]);
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

    glm::vec3 v1 = vertices[0].screenPos / vertices[0].screenPos.w;
    glm::vec3 v2 = vertices[1].screenPos / vertices[1].screenPos.w;

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
    glm::ivec2 minnpos = glm::ivec2(v1.screenPos.x, v1.screenPos.y);
    glm::ivec2 maxxpos = glm::ivec2(v1.screenPos.x + 0.5, v1.screenPos.y + 0.5);

    minnpos.x = std::min(minnpos.x, (int)v2.screenPos.x);
    maxxpos.x = std::max(maxxpos.x, (int)(v2.screenPos.x + 0.5));
    minnpos.y = std::min(minnpos.y, (int)v2.screenPos.y);
    maxxpos.y = std::max(maxxpos.y, (int)(v2.screenPos.y + 0.5));

    minnpos.x = std::min(minnpos.x, (int)v3.screenPos.x);
    maxxpos.x = std::max(maxxpos.x, (int)(v3.screenPos.x + 0.5));
    minnpos.y = std::min(minnpos.y, (int)v3.screenPos.y);
    maxxpos.y = std::max(maxxpos.y, (int)(v3.screenPos.y + 0.5));

    minnpos.x = std::max(minnpos.x, 0);
    minnpos.y = std::max(minnpos.y, 0);
    maxxpos.x = std::min(maxxpos.x, width);
    maxxpos.y = std::min(maxxpos.y, height);

    glm::vec2 v[3];

    v[0] = v1.screenPos;
    v[1] = v2.screenPos;
    v[2] = v3.screenPos;

    glm::vec2 t[3];
    for (int i = 0; i < 3; i++) t[i] = v[(i + 1) % 3] - v[i];

    glm::mat2 interpTransform(v[1] - v[0], v[2] - v[0]);
    interpTransform = glm::inverse(interpTransform);

    if (std::isnan(interpTransform[0][0]) || std::isnan(interpTransform[0][1]) || std::isnan(interpTransform[1][0]) || std::isnan(interpTransform[1][1])) {
        /*printf("%lf %lf, %lf %lf, %lf %lf\n", v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y);*/
        return;
    }


    float w1 = v1.screenPos.w, w2 = v2.screenPos.w, w3 = v3.screenPos.w;
    glm::mat3x3 colorMat(v1.color / w1, v2.color / w2, v3.color / w3);
    glm::mat3x4 posMat(v1.pos / w1, v2.pos / w2, v3.pos / w3);
    glm::mat3x2 texMat(v1.texCoord / w1, v2.texCoord / w2, v3.texCoord / w3);
    glm::mat3x3 normalMat(v1.normal / w1, v2.normal / w2, v3.normal / w3);
    FragmentShaderPayload payload;


    const int sampleNum = _sampleConfig.sampleSize;

    for (int j = minnpos.y; j <= maxxpos.y; j++) {
        for (int i = minnpos.x; i <= maxxpos.x; i++) {
            for (int k = 0; k < sampleNum; k++) {
                glm::vec2 pos = glm::vec2(i, j) + _sampleConfig.samplePoints[k];

                // Barycentric calculation
                glm::vec2 uv = interpTransform * (pos - v[0]);


                if (isnan(uv.x) || isnan(uv.y) || uv.x < -EPS || uv.x > 1 + EPS || uv.y < -EPS || uv.y > 1 + EPS || uv.x + uv.y > 1 + EPS) continue;
                glm::vec3 interp = glm::vec3(1 - uv.x - uv.y, uv.x, uv.y);

                bool b1 = (std::abs(interp.x) > EPS || (std::abs(interp.x) < EPS && isTopLeftEdge(t[1])));
                bool b2 = (std::abs(interp.y) > EPS || (std::abs(interp.y) < EPS && isTopLeftEdge(t[2])));
                bool b3 = (std::abs(interp.z) > EPS || (std::abs(interp.z) < EPS && isTopLeftEdge(t[0])));
                if (!b1 || !b2 || !b3)continue;
                float z = 1 / (interp.x / w1 + interp.y / w2 + interp.z / w3);
                interp *= z;

                // Check z-buffer
                if (z > _frameBuffer->GetZBuffer(i, j, k)) continue;

                payload.vertex.color = colorMat * interp;
                payload.vertex.pos = posMat * interp;
                payload.vertex.texCoord = texMat * interp;
                payload.vertex.normal = normalMat * interp;

                _frameBuffer->Write(i, j, k, _fragmentShader->fragment_shader(payload));
                _frameBuffer->WriteZBuffer(i, j, k, z);
            }
        }
    }
}
void Renderer::bresenham(const Vertex* v1, const Vertex* v2) {
    glm::ivec2 start(v1->screenPos.x + 0.5, v1->screenPos.y + 0.5), end(v2->screenPos.x + 0.5, v2->screenPos.y + 0.5);

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

    glm::vec4 posM[2] = { v1->pos / v1->screenPos.w, v2->pos / v2->screenPos.w };
    glm::vec3 colorM[2] = { v1->color / v1->screenPos.w, v2->color / v2->screenPos.w };
    //glm::vec2 texCoord[2] = { v1->texCoord / v1->pos.w, v2->texCoord / v2->pos.w };
    //glm::vec3 normal[2] = { v1->normal / v1->pos.w, v2->normal / v2->pos.w };
    for (int i = start.x, j = start.y; i <= end.x; i++) {
        float t = (i - start.x) / (float)(end.x - start.x);
        float z = 1 / ((1 - t) / v1->screenPos.w + t / v2->screenPos.w);
        glm::vec3 color = glm::mix(colorM[0], colorM[1], t) * z /** (-glm::mix(posM[0], posM[1], t).z * 0.5f + 0.5f)*/;
        if (swp) {
            if (_frameBuffer->GetZBuffer(j, i, 0) >= z) {
                _frameBuffer->WriteFull(j, i, color);/*fragment_shader(position, color, texCoord)*/
                _frameBuffer->WriteZBuffer(j, i, 0, z);
            }
        }
        else {
            if (_frameBuffer->GetZBuffer(i, j, 0) >= z) {
                _frameBuffer->WriteFull(i, j, color);
                _frameBuffer->WriteZBuffer(i, j, 0, z);
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
    //    output[i].screenPos.x /= input[i].screenPos.w;
    //    output[i].screenPos.y /= input[i].screenPos.w;
    //    output[i].screenPos.z /= input[i].screenPos.w;
    //}
    //for (int i = 0; i < 3; i++) indices[i] = i;
    //return 3;
    static glm::vec3 initial[3];
    for (int i = 0; i < 3; i++) {
        initial[i] = glm::vec3(input[i].screenPos / input[i].screenPos.w);
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
            output[i].screenPos.x /= input[i].screenPos.w;
            output[i].screenPos.y /= input[i].screenPos.w;
            output[i].screenPos.z /= input[i].screenPos.w;
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
            output[i].screenPos.x /= input[i].screenPos.w;
            output[i].screenPos.y /= input[i].screenPos.w;
            output[i].screenPos.z /= input[i].screenPos.w;
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

    float w1 = v1.screenPos.w, w2 = v2.screenPos.w;

    Vertex v;
    float z = 1 / ((1 - t) / w1 + t / w2);
    v.pos = glm::mix(v1.pos / w1, v2.pos / w2, t) * z; // tmp[0].pos* (1 - tMin) + tmp[1].pos * tMin;
    v.screenPos = glm::mix(v1.screenPos / w1, v2.screenPos / w2, t) * z;
    v.color = glm::mix(v1.color / w1, v2.color / w2, t) * z;
    v.texCoord = glm::mix(v1.texCoord / w1, v2.texCoord / w2, t) * z;
    v.normal = glm::mix(v1.normal / w1, v2.normal / w2, t) * z;

    v.screenPos.x /= v.screenPos.w;
    v.screenPos.y /= v.screenPos.w;
    v.screenPos.z /= v.screenPos.w;
    return v;
}

Vertex Renderer::barycentric_interpolation_Perspect(const Vertex& v1, const Vertex& v2, const Vertex& v3, glm::vec3 bary) const {

    float w1 = std::abs(v1.screenPos.w), w2 = std::abs(v2.screenPos.w), w3 = std::abs(v3.screenPos.w);
    Vertex v;
    float z = 1 / (bary.x / w1 + bary.y / w2 + bary.z / w3);
    bary *= z;

    v.pos = glm::mat3x4(v1.pos / w1, v2.pos / w2, v3.pos / w3) * bary;
    v.screenPos = glm::mat3x4(v1.screenPos / w1, v2.screenPos / w2, v3.screenPos / w3) * bary;
    v.color = glm::mat3x3(v1.color / w1, v2.color / w2, v3.color / w3) * bary;
    v.texCoord = glm::mat3x2(v1.texCoord / w1, v2.texCoord / w2, v3.texCoord / w3) * bary;
    v.normal = glm::mat3x3(v1.normal / w1, v2.normal / w2, v3.normal / w3) * bary;

    v.screenPos.x /= v.screenPos.w;
    v.screenPos.y /= v.screenPos.w;
    v.screenPos.z /= v.screenPos.w;
    return v;
}
