#include "Rasterizer.h"
#include <glm\gtx\transform.hpp>
#include <glm\gtx\string_cast.hpp>
#include <iostream>
#include <algorithm>
#include <tuple>

RColor to_color(const glm::vec3& vec) {
    return { (unsigned char)glm::round(glm::clamp(vec.z, 0.f, 1.f) * 255),
        (unsigned char)glm::round(glm::clamp(vec.y, 0.f, 1.f) * 255),
        (unsigned char)glm::round(glm::clamp(vec.x, 0.f, 1.f) * 255) };
}

Rasterizer::Rasterizer(int width, int height, SampleStyle style) {
    _sampleStyle = style;
    _sampleCount = 1;
    if (style == SampleStyle::X2)_sampleCount = 4;
    _width = width, _height = height;

    _framebuffer = new RColor[width * height];
    _sampleBuffer = new glm::vec3[width * height * _sampleCount];
    _depthBuffer = new float[width * height * _sampleCount];
    _shadowMap = new float[width * height * _sampleCount];

    _lights = Light(glm::vec3(0, 5, -5), glm::vec3(15, 15, 15));
}

Rasterizer::~Rasterizer() {
    delete[] _framebuffer;
    delete[] _sampleBuffer;
    delete[] _depthBuffer;
    delete[] _shadowMap;
}


glm::vec3 to_vec3(glm::vec4 v) {
    return glm::vec3(v.x, v.y, v.z);
}

void Rasterizer::DrawTriangles(const std::vector<Triangle>& triangles, DrawStyle style) {
    glm::mat4 vm = _view * _model;
    for (const auto& t : triangles) {
        Triangle tmp;
        glm::vec4 viewPos[3];
        glm::vec3 vp[3];
        memcpy(&tmp, &t, sizeof(Triangle));
        memcpy(viewPos, t.vs, sizeof(t.vs));
        for (int i = 0; i < 3; i++) {
            viewPos[i] = vm * tmp.vs[i];
            tmp.vs[i] = _projection * viewPos[i];
        }
        for (int i = 0; i < 3; i++) {
            tmp.vs[i].x /= tmp.vs[i].w;
            tmp.vs[i].y /= tmp.vs[i].w;
            tmp.vs[i].z /= tmp.vs[i].w;
        }
        for (int i = 0; i < 3; i++) {
            tmp.vs[i].x = _width * 0.5f * (tmp.vs[i].x + 1);
            tmp.vs[i].y = _height * 0.5f * (tmp.vs[i].y + 1);
        }
        for (int i = 0; i < 3; i++) {
            vp[i] = to_vec3(viewPos[i]);
        }

        glm::mat4 inverse = _view * _model;
        for (int i = 0; i < 3; i++) {
            tmp.normals[i] = inverse * glm::vec4(tmp.normals[i], 0.f);
        }
        if (style == DrawStyle::WireFrame) {
            draw_line(tmp.vs[0], tmp.vs[1]);
            draw_line(tmp.vs[1], tmp.vs[2]);
            draw_line(tmp.vs[2], tmp.vs[0]);
        }
        else {
            raster_triangle(tmp, vp);
        }
    }
}

void Rasterizer::DrawModels(const std::vector<Model>& models, DrawStyle style) {

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;
    for (const auto& model : models) {
        _currentShader = model._shader;
        for (const auto& t : model.getTriangles()) {
            Triangle screenTri;
            glm::vec3 viewPos[3];
            memcpy(&screenTri, &t, sizeof(Triangle));
            for (int i = 0; i < 3; i++) {
                viewPos[i] = to_vec3(_view * t.vs[i]);
            }
            for (int i = 0; i < 3; i++) {
                screenTri.vs[i] = _projection * _view * t.vs[i];
            }
            for (int i = 0; i < 3; i++) {
                float w = screenTri.vs[i].w;
                screenTri.vs[i].x /= w;
                screenTri.vs[i].y /= w;
                screenTri.vs[i].z /= w;
            }
            // Viewport Transform
            for (int i = 0; i < 3; i++) {
                screenTri.vs[i].x = _width * 0.5f * (screenTri.vs[i].x + 1);
                screenTri.vs[i].y = _height * 0.5f * (screenTri.vs[i].y + 1);
                screenTri.vs[i].z = screenTri.vs[i].z * f1 + f2;
            }
            for (int i = 0; i < 3; i++) {
                screenTri.normals[i] = _view * glm::vec4(t.normals[i], 0.f);
            }
            raster_triangle(screenTri, viewPos);
        }
    }
}


static glm::mat4 view_matrix(glm::vec3 eyePos, glm::vec3 lookat, glm::vec3 up) {
    glm::vec3 Z = glm::normalize(-lookat);
    glm::vec3 Y = glm::normalize(up);
    glm::vec3 X = glm::cross(-Z, Y);
    glm::mat4 view(glm::vec4(X, 0.f), glm::vec4(Y, 0.f), glm::vec4(Z, 0.f), glm::vec4(0, 0, 0, 1.f));
    return glm::transpose(view) * glm::translate(-eyePos);
}
void Rasterizer::CreateShadowMap(const std::vector<Model>& models) {
    auto v = glm::normalize(-_lights.position);
    glm::mat4 viewM = glm::lookAtRH(_lights.position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    for (const auto& model : models) {
        for (const auto& t : model.getTriangles()) {
            Triangle screenTri;
            memcpy(&screenTri, &t, sizeof(Triangle));
            for (int i = 0; i < 3; i++) {
                screenTri.vs[i] = _projection * viewM * t.vs[i];
            }
            for (int i = 0; i < 3; i++) {
                float w = screenTri.vs[i].w;
                screenTri.vs[i].x /= w;
                screenTri.vs[i].y /= w;
                screenTri.vs[i].z /= w;
            }
            // Viewport Transform
            for (int i = 0; i < 3; i++) {
                screenTri.vs[i].x = _width * 0.5f * (screenTri.vs[i].x + 1);
                screenTri.vs[i].y = _height * 0.5f * (screenTri.vs[i].y + 1);
            }
            raster_shadowmap(screenTri);
        }
    }
    _shadowMapTransform = _projection * viewM;
}


glm::vec2 _bezier_rec(const std::vector<glm::vec2>& pts, float t) {
    int sz = pts.size();
    if (sz == 2) {
        return pts[0] * t + pts[1] * (1 - t);
    }
    std::vector<glm::vec2> p2;
    for (int i = 1; i < sz; i++) {
        p2.push_back(pts[i - 1] * t + pts[i] * (1 - t));
    }
    return _bezier_rec(p2, t);
}

void Rasterizer::DrawBezier(const std::vector<glm::vec2>& points) {
    static glm::vec2 samplePoint[4] = { {0.33f, 0.33f}, {0.33f, 0.67f}, {0.67f, 0.33f}, {0.67f, 0.67f} };
    static int dx[9] = { 1, 1, 1, -1, -1, -1, 0, 0, 0, };
    static int dy[9] = { 1, 0, -1, 1, 0, -1, 1, -1, 0, };
    for (float i = 0; i <= 1; i += 0.001f) {
        glm::vec2 pt = _bezier_rec(points, i);
        if (_sampleStyle == SampleStyle::None) {
            for (int j = 0; j < 9; j++) {
                int nx = (int)pt.x + dx[j];
                int ny = (int)pt.y + dy[j];
                if (nx < 0 || nx >= _width || ny < 0 || ny >= _height)continue;
                glm::vec2 sp = glm::vec2(nx + 0.5f, ny + 0.5f);
                float d = glm::distance(pt, sp);
                float f = glm::clamp(1.f - 0.8f * d, 0.f, 1.f);
                if (d < 0.5f * 1.4142f) {
                    set_pixel_alpha_blend(nx, ny, 0, glm::vec3(1, 1, 1), 1);
                }
                else {
                    set_pixel_alpha_blend(nx, ny, 0, glm::vec3(1, 1, 1), f);
                }
            }
        }
        else {
            for (int j = 0; j < 9; j++) {
                int nx = (int)pt.x + dx[j];
                int ny = (int)pt.y + dy[j];
                if (nx < 0 || nx >= _width || ny < 0 || ny >= _height)continue;
                for (int k = 0; k < _sampleCount; k++) {
                    glm::vec2 sp = glm::vec2(nx + samplePoint[k].x, ny + samplePoint[k].y);
                    float d = glm::distance(pt, sp);
                    float f = glm::clamp(1.f - 0.8f * d, 0.f, 1.f);
                    if (d < 0.6f * 1.4142f) {
                        set_pixel_alpha_blend(nx, ny, k, glm::vec3(1, 1, 1), 1);
                    }
                    else {
                        set_pixel_alpha_blend(nx, ny, k, glm::vec3(1, 1, 1), f);
                    }
                }
            }
        }
    }
}

void Rasterizer::Present() {
    copy_to_frameBuffer();
}

void Rasterizer::Clear() {
    memset(_framebuffer, 0, _width * _height * sizeof(RColor));

    memset(_sampleBuffer, 0, _width * _height * _sampleCount * sizeof(glm::vec3));
    memset(_depthBuffer, 0x7f, _width * _height * _sampleCount * sizeof(float));
    memset(_shadowMap, 0x7f, _width * _height * _sampleCount * sizeof(float));
}


void Rasterizer::set_pixel(int x, int y, int id, const glm::vec3& color) {
    if (x >= _width || x < 0 || y >= _height || y < 0) return;
    int idx = y * _width + x;
    _sampleBuffer[idx * _sampleCount + id] = color;
}

void Rasterizer::set_depth_shadow(int x, int y, int id, float depth) {
    if (x >= _width || x < 0 || y >= _height || y < 0) return;
    int idx = y * _width + x;
    if (_shadowMap[idx * _sampleCount + id] < depth) return;
    _shadowMap[idx * _sampleCount + id] = depth;
}

void Rasterizer::set_pixel_alpha_blend(int x, int y, int id, const glm::vec3& color, float alpha) {
    if (x >= _width || x < 0 || y >= _height || y < 0) return;
    int idx = y * _width + x;
    _sampleBuffer[idx * _sampleCount + id] = _sampleBuffer[idx * _sampleCount + id] * (1 - alpha) + color * alpha;
}

bool Rasterizer::depth_test(int x, int y, int id, float depth) {
    if (x >= _width || x < 0 || y >= _height || y < 0) return false;
    int idx = y * _width + x;
    if (_depthBuffer[idx * _sampleCount + id] < depth) return false;
    _depthBuffer[idx * _sampleCount + id] = depth;
    return true;
}

void Rasterizer::draw_line(const glm::vec3& st, const glm::vec3& ed) {
    float dx = ed.x - st.x;
    float dy = ed.y - st.y;
    float y0 = st.y, x0 = st.x;
    float y1 = ed.y, x1 = ed.x;
    bool swp = false;
    if (fabs(dx) < fabs(dy)) {
        std::swap(x0, y0), std::swap(x1, y1);
        std::swap(dx, dy);
        swp = true;
    }
    if (dx < 0) {
        std::swap(x0, x1), std::swap(y0, y1);
        dx *= -1, dy *= -1;
    }
    float dr = abs(dy / dx);
    float err = 0;
    glm::vec3 white = { 1, 1, 1 };
    y0 = (int)y0;
    for (int x = x0; x <= x1; x++) {
        if (swp) {
            set_pixel(y0, x, 0, white);
        }
        else {
            set_pixel(x, y0, 0, white);
        }
        err += dr;
        if (err >= 0.5f) {
            y0 = y0 + ((dy > 0) ? 1 : -1);
            err -= 1.0f;
        }
    }
}


int dcmp(double x) {
    if (fabs(x) < 1e-9) return 0;
    return x > 0 ? 1 : -1;
}

float cross2(glm::vec2 a, glm::vec2 b) {
    return (a.x * b.y - a.y * b.x);
}

//static bool insideTriangle(glm::vec2 point, const glm::vec2* _v) {
//    glm::vec3 v[3];
//    for (int i = 0; i < 3; i++)
//        v[i] = { _v[i].x,_v[i].y, 1.0 };
//    glm::vec3 f0, f1, f2;
//    f0 = glm::cross(v[1], v[0]);
//    f1 = glm::cross(v[2], v[1]);
//    f2 = glm::cross(v[0], v[2]);
//    glm::vec3 p(point.x, point.y, 1.);
//    if (glm::dot(p, f0) * glm::dot(f0, v[2]) > 0 && (glm::dot(p, f1)) * glm::dot(f1, v[0]) > 0 && glm::dot(p, f2) * glm::dot(f2, v[1]) > 0)
//        return true;
//    return false;
//}
//

float f(const glm::vec2& pos, const glm::vec2& v1, const glm::vec2& v2) {
    return (v1.y - v2.y) * pos.x + (v2.x - v1.x) * pos.y + v1.x * v2.y - v2.x * v1.y;
}
bool in_triangle(const glm::vec3& u, const glm::vec2* v) {
    static glm::vec2 testPoint(0.114514, 0.258943);
    if (u.x >= 0 && u.y >= 0 && u.z >= 0) {
        return true;
    }
    return false;
}

static glm::vec3 computeBarycentric2D(const glm::vec2& p, const glm::vec4* v2) {
    float alpha = f(p, v2[1], v2[2]) / f(v2[0], v2[1], v2[2]);
    float beta = f(p, v2[2], v2[0]) / f(v2[1], v2[2], v2[0]);
    return { alpha, beta, 1 - alpha - beta };
}

static glm::vec3 interpolate(float alpha, float beta, float gamma, const glm::vec3& vert1, const glm::vec3& vert2, const glm::vec3& vert3, float weight) {
    return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}
static glm::vec2 interpolate(float alpha, float beta, float gamma, const glm::vec2& vert1, const glm::vec2& vert2, const glm::vec2& vert3, float weight) {
    return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

static float interpolate(float alpha, float beta, float gamma, float v1, float v2, float v3, float weight) {
    return (alpha * v1 + beta * v2 + gamma * v3) / weight;
}


void Rasterizer::raster_triangle(const Triangle& t, const glm::vec3* viewPos) {
    int minnX = _width - 1, minnY = _height - 1;
    int maxxX = 0, maxxY = 0;
    for (int i = 0; i < 3; i++) {
        minnX = std::min(minnX, (int)t.vs[i].x);
        minnY = std::min(minnY, (int)t.vs[i].y);
        maxxX = std::max(maxxX, (int)(t.vs[i].x + 0.5f));
        maxxY = std::max(maxxY, (int)(t.vs[i].y + 0.5f));
    }
    maxxX = std::min(maxxX, _width - 1);
    maxxY = std::min(maxxY, _height - 1);
    minnX = std::max(minnX, 0);
    minnY = std::max(minnY, 0);
    static glm::vec2 samplePoint[4] = { {0.3f, 0.1f}, {0.9f, 0.3f}, {0.1f, 0.7f}, {0.7f, 0.9f} };
    glm::vec2 tv[3] = { {t.vs[0].x, t.vs[0].y}, {t.vs[1].x, t.vs[1].y}, {t.vs[2].x, t.vs[2].y} };
    for (int i = minnX; i <= maxxX; i++) {
        for (int j = minnY; j <= maxxY; j++) {
            if (_sampleStyle == SampleStyle::None) {
                glm::vec2 sp = glm::vec2(i + 0.5f, j + 0.5f);
                auto interp = computeBarycentric2D(sp, t.vs);
                if (in_triangle(interp, tv)) {
                    float w_reciprocal = 1.0f / (interp.x / t.vs[0].w + interp.y / t.vs[1].w + interp.z / t.vs[2].w);
                    auto z_interp = interpolate(interp.x, interp.y, interp.z, 1, 1, 1, 1) * w_reciprocal;
                    auto interpolated_shadingcoords = interpolate(interp.x, interp.y, interp.z, viewPos[0] / t.vs[0].w, viewPos[1] / t.vs[1].w, viewPos[2] / t.vs[2].w, 1) * w_reciprocal;
                    if (depth_test(i, j, 0, z_interp)) {
                        auto interpolated_color = interpolate(interp.x, interp.y, interp.z, t.color[0] / t.vs[0].w, t.color[1] / t.vs[1].w, t.color[2] / t.vs[2].w, 1) * w_reciprocal;
                        auto interpolated_normal = interpolate(interp.x, interp.y, interp.z, t.normals[0] / t.vs[0].w, t.normals[1] / t.vs[1].w, t.normals[2] / t.vs[2].w, 1) * w_reciprocal;
                        auto tex_coord = interpolate(interp.x, interp.y, interp.z, t.tex_coords[0] / t.vs[0].w, t.tex_coords[1] / t.vs[1].w, t.tex_coords[2] / t.vs[2].w, 1) * w_reciprocal;
                        fragment_shader_payload info(interpolated_color, interpolated_normal, tex_coord);
                        info.view_pos = interpolated_shadingcoords;
                        set_pixel(i, j, 0, _currentShader ? _currentShader(info) : shader_function(info));
                    }
                }
            }
            else {
                for (int k = 0; k < _sampleCount; k++) {
                    glm::vec2 sp = glm::vec2(i + samplePoint[k].x, j + samplePoint[k].y);
                    auto interp = computeBarycentric2D(sp, t.vs);
                    if (in_triangle(interp, tv)) {
                        float w_reciprocal = 1.0 / (interp.x / t.vs[0].w + interp.y / t.vs[1].w + interp.z / t.vs[2].w);
                        auto z_interp = interpolate(interp.x, interp.y, interp.z, t.vs[0].z / t.vs[0].w, t.vs[1].z / t.vs[1].w, t.vs[2].z / t.vs[2].w, 1) * w_reciprocal;
                        if (depth_test(i, j, k, z_interp)) {
                            auto interpolated_color = interpolate(interp.x, interp.y, interp.z, t.color[0] / t.vs[0].w, t.color[1] / t.vs[1].w, t.color[2] / t.vs[2].w, 1) * w_reciprocal;
                            auto interpolated_normal = interpolate(interp.x, interp.y, interp.z, t.normals[0] / t.vs[0].w, t.normals[1] / t.vs[1].w, t.normals[2] / t.vs[2].w, 1) * w_reciprocal;
                            auto tex_coord = interpolate(interp.x, interp.y, interp.z, t.tex_coords[0] / t.vs[0].w, t.tex_coords[1] / t.vs[1].w, t.tex_coords[2] / t.vs[2].w, 1) * w_reciprocal;
                            auto interpolated_shadingcoords = interpolate(interp.x, interp.y, interp.z, viewPos[0] / t.vs[0].w, viewPos[1] / t.vs[1].w, viewPos[2] / t.vs[2].w, 1) * w_reciprocal;
                            fragment_shader_payload info(interpolated_color, interpolated_normal, tex_coord);
                            info.view_pos = interpolated_shadingcoords;
                            set_pixel(i, j, k, shader_function(info));
                        }
                    }
                }
            }
        }
    }
}

void Rasterizer::raster_shadowmap(const Triangle& t) {
    int minnX = _width - 1, minnY = _height - 1;
    int maxxX = 0, maxxY = 0;
    for (int i = 0; i < 3; i++) {
        minnX = std::min(minnX, (int)t.vs[i].x);
        minnY = std::min(minnY, (int)t.vs[i].y);
        maxxX = std::max(maxxX, (int)(t.vs[i].x + 0.5f));
        maxxY = std::max(maxxY, (int)(t.vs[i].y + 0.5f));
    }
    maxxX = std::min(maxxX, _width - 1);
    maxxY = std::min(maxxY, _height - 1);
    minnX = std::max(minnX, 0);
    minnY = std::max(minnY, 0);
    static glm::vec2 samplePoint[4] = { {0.3f, 0.1f}, {0.9f, 0.3f}, {0.1f, 0.7f}, {0.7f, 0.9f} };
    glm::vec2 tv[3] = { {t.vs[0].x, t.vs[0].y}, {t.vs[1].x, t.vs[1].y}, {t.vs[2].x, t.vs[2].y} };
    for (int i = minnX; i <= maxxX; i++) {
        for (int j = minnY; j <= maxxY; j++) {
            if (_sampleStyle == SampleStyle::None) {
                glm::vec2 sp = glm::vec2(i + 0.5f, j + 0.5f);
                auto interp = computeBarycentric2D(sp, t.vs);
                if (in_triangle(interp, tv)) {
                    float w_reciprocal = 1.0 / (interp.x / t.vs[0].w + interp.y / t.vs[1].w + interp.z / t.vs[2].w);
                    auto z_interp = interpolate(interp.x, interp.y, interp.z, 1, 1, 1, 1) * w_reciprocal;
                    set_depth_shadow(i, j, 0, z_interp);
                }
            }
            else {
                for (int k = 0; k < _sampleCount; k++) {
                    glm::vec2 sp = glm::vec2(i + samplePoint[k].x, j + samplePoint[k].y);
                    auto interp = computeBarycentric2D(sp, t.vs);
                    if (in_triangle(interp, tv)) {
                        float w_reciprocal = 1.0 / (interp.x / t.vs[0].w + interp.y / t.vs[1].w + interp.z / t.vs[2].w);
                        auto z_interp = interpolate(interp.x, interp.y, interp.z, 1, 1, 1, 1) * w_reciprocal;
                        set_depth_shadow(i, j, k, z_interp);
                    }
                }
            }
        }
    }
}

bool Rasterizer::shadowmap_test(const glm::vec4& worldPos) const {
    auto p = _shadowMapTransform * worldPos;
    float w = p.w;
    p.x /= w, p.y /= w;
    int x = _width * 0.5f * (p.x + 1);
    int y = _height * 0.5f * (p.y + 1);
    if (x >= _width || x < 0 || y >= _height || y < 0) return true;
    int idx = y * _width + x;
    return w - 1e-2 <= _shadowMap[idx];
}


void Rasterizer::copy_to_frameBuffer() {
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            int id = i * _width + j;
            glm::vec3 color = glm::vec3(0);
            for (int k = 0; k < _sampleCount; k++) {
                color += glm::vec3(_sampleBuffer[id * _sampleCount + k]);
            }
            color /= (float)_sampleCount;
            _framebuffer[id] = to_color(color);
        }
    }
}

glm::vec3 Rasterizer::shader_function(const fragment_shader_payload& info) {
    glm::vec3 ka = glm::vec3(0.1, 0.1, 0.1);
    glm::vec3 kd = info.color;
    glm::vec3 ks = glm::vec3(0.7f, 0.7f, 0.7f);
    float amb_intensity = 0.5f;

    glm::vec3 normal = glm::normalize(info.normal);
    glm::vec3 lightPos = to_vec3(_view * glm::vec4(_lights.position, 1));
    glm::vec3 finalLight = glm::vec3(0, 0, 0);


    glm::vec3 amb = ka * amb_intensity * info.color;
    finalLight += amb;

    if (shadowmap_test(_invView * glm::vec4(info.view_pos, 1.f))) {
        glm::vec3 lightDir = glm::normalize(lightPos - info.view_pos);
        glm::vec3 tmp = lightPos - info.view_pos;
        float dis = tmp.x * tmp.x + tmp.y * tmp.y;
        glm::vec3 diffuse = kd * std::max(glm::dot(normal, lightDir), 0.f) * _lights.intensity / dis;
        finalLight += diffuse;

        glm::vec3 eyeDir = glm::normalize(-info.view_pos);
        glm::vec3 h = glm::normalize(lightDir + eyeDir);
        glm::vec3 specular = ks * std::pow(std::max(glm::dot(normal, h), 0.f), 256) * _lights.intensity / dis;
        finalLight += specular;
    }
    return finalLight * info.color;
}

//glm::vec3 Rasterizer::shader_function(const fragment_shader_payload& info) {
//    return glm::normalize(info.normal);
//}
