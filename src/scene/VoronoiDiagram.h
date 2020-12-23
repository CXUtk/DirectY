#pragma once
#include <glm/gtx/transform.hpp>
#include "../render/Renderer.h"
#include "../framework/system/DYWindow.h"
#include "ComputationGeometry.h"

class VoronoiDiagram {
public:
    VoronoiDiagram(std::shared_ptr<DYWindow> window);
    void Update();
    void Draw();
    ~VoronoiDiagram() {}
private:
    int _width, _height;
    std::shared_ptr<DYWindow> _window;

    MouseInfo _oldMouseInfo;
    MouseInfo _curMouseInfo;

    glm::ivec2 _oldMousePos;

    std::shared_ptr<Renderer> _renderer;
    std::shared_ptr<VertexShader> _vertexShader;
    std::shared_ptr<FragmentShader> _fragShader;
    std::shared_ptr<FragmentShader> _fragShaderC;

    std::shared_ptr<Camera> _camera;

    // Model Buffer
    int _vbuff, _vbuff2;

    glm::vec2 _curPoint;
    std::vector<glm::vec2> _points;
    std::vector<Line> _lines;

    void resetInputState();
};
