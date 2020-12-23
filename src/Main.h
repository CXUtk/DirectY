#pragma once
#include <glm/gtx/transform.hpp>
#include "render/Renderer.h"
#include "framework/system/DYWindow.h"
#include "render/Camera.h"
#include "resource/TextureManager.h"
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


    MouseInfo _oldMouseInfo;
    MouseInfo _curMouseInfo;

    KeyState _oldKeyState;
    KeyState _curKeyState;

    glm::ivec2 _oldMousePos;
    glm::vec2 _oldOrbitParameter;
    glm::vec2 _curOrbitParameter;
    float _orbitDistance;

    int _oldWheelPos;
    int _curWheelPos;

    TextureManager _textureManager;

    std::shared_ptr<Renderer> _renderer;
    std::shared_ptr<VertexShader> _vertexShader;
    std::shared_ptr<FragmentShader> _fragShader;

    std::shared_ptr<Camera> _camera;

    std::vector<Vertex> _vertices;


    void resetInputState();
};
