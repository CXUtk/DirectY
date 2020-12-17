#pragma once
#include <glm/gtx/transform.hpp>
#include "render/Renderer.h"
#include "framework/system/DYWindow.h"
#include "render/Camera.h"
class Main {
public:
    Main(std::shared_ptr<DYWindow> window);
    void Update();
    void Draw();
    ~Main() {}
private:
    int _width, _height;
    std::shared_ptr<DYWindow> _window;

    int _vbuff, _vbuff2, _vbuff3;
    int _modelBuff, _modelBuff1;
    int _numVertices, _numFaces;


    bool _wasMouseLeftDown;

    glm::ivec2 _oldMousePos;
    glm::vec2 _oldOrbitParameter;
    glm::vec2 _curOrbitParameter;

    std::shared_ptr<Renderer> _renderer;
    std::shared_ptr<VertexShader> _vertexShader;
    std::shared_ptr<FragmentShader> _fragShader;

    std::shared_ptr<Camera> _camera;

    std::vector<Vertex> _vertices;
};
