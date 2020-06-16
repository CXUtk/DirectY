#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include "Triangle.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"
struct RColor {
    unsigned char R;
    unsigned char G;
    unsigned char B;
};
enum class DrawStyle {
    WireFrame,
    Fill,
};
enum class SampleStyle {
    None,
    X2,
};
struct Light {
    Light() {}
    Light(const glm::vec3& pos, const glm::vec3& intensity) :position(pos), intensity(intensity) {

    }
    glm::vec3 position;
    glm::vec3 intensity;
};

class Rasterizer {
public:
    Rasterizer(int width, int height, SampleStyle style);
    ~Rasterizer();

    void SetModelMatrix(const glm::mat4& mat) { _model = mat; }
    void SetCamera(Camera* camera) {
        _view = camera->getViewMatrix();
        _invView = glm::inverse(_view);
        _camera = camera;
    }
    void SetViewMatrix(const glm::mat4& view) { _view = view; }
    void SetProjectionMatrix(const glm::mat4& mat) {
        _projection = mat;
        _invView = glm::inverse(_view);
    }

    void DrawTriangles(const std::vector<Triangle>& triangles, DrawStyle style);
    void DrawModels(const std::vector<Model>& models, DrawStyle style);
    void CreateShadowMap(const std::vector<Model>& models);
    void DrawBezier(const std::vector<glm::vec2>& points);

    void Present();
    void Clear();

    const RColor* getFrameBuffer() { return _framebuffer; }
private:
    RColor* _framebuffer;
    glm::vec3* _sampleBuffer;
    float* _depthBuffer;
    float* _shadowMap;
    int _width, _height;

    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _invView;
    glm::mat4 _projection;
    glm::mat4 _shadowMapTransform;


    SampleStyle _sampleStyle;
    int _sampleCount;

    Camera* _camera;
    Light _lights;
    fragment_shader _currentShader;


    void set_pixel(int x, int y, int id, const glm::vec3& color);
    void set_depth_shadow(int x, int y, int id, float depth);
    void set_pixel_alpha_blend(int x, int y, int id, const glm::vec3& color, float alpha);
    bool depth_test(int x, int y, int id, float depth);
    void draw_line(const glm::vec3& st, const glm::vec3& ed);
    void raster_triangle(const Triangle& tmpTri, const glm::vec3* viewPos);
    void raster_shadowmap(const Triangle& tmpTri);
    bool shadowmap_test(const glm::vec4& worldPos) const;
    void copy_to_frameBuffer();

    glm::vec3 shader_function(const fragment_shader_payload& info);
};
