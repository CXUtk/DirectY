﻿#include "Main.h"
#include <glm/gtx/transform.hpp>
#include <vector>

#include "resource/ObjLoader.h"
#include "framework/geometry.h"
#include <algorithm>


Main::Main(std::shared_ptr<DYWindow> window) : _window(window) {
    _curOrbitParameter = _oldOrbitParameter = glm::vec2(0);
    _oldMousePos = glm::vec2(0, _height - 2);

    _width = window->getWidth();
    _height = window->getHeight();

    _renderer = std::make_shared<Renderer>(_width, _height, window->getGraphicDevice());

    _vertexShader = std::make_shared<VertexShader>();
    _renderer->SetVertexShader(_vertexShader);
    _fragShader = std::make_shared<FragmentShader>();
    _renderer->SetFragmentShader(_fragShader);


    _camera = std::make_shared<Camera>(glm::vec3(0, 0, -4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::pi<float>() / 4, 800.f / 600.f, 1.0f, 100.f);
    _vertexShader->SetProjTransform(_camera->getProjectTransform());
    //Vertex triangle[6] = {
    //    Vertex(glm::vec4(-0.5, 0.5, 0, 1), glm::vec3(1, 0, 0), glm::vec2(0, 0)),
    //    Vertex(glm::vec4(-0.5, -0.5, 0, 1), glm::vec3(0, 1, 0), glm::vec2(0, 1)),
    //    Vertex(glm::vec4(0.5, -0.5, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 1)),

    //    Vertex(glm::vec4(-0.5, 0.5, 0, 1), glm::vec3(1, 0, 0), glm::vec2(0, 0)),
    //    Vertex(glm::vec4(0.5, -0.5, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 1)),
    //    Vertex(glm::vec4(0.5, 0.5, 0, 1), glm::vec3(1, 1, 0), glm::vec2(1, 0))
    //};

    //Vertex triangle2[3] = {
    //    Vertex(glm::vec4(-0.5, 0.5, -1, 1), glm::vec3(0, 1, 0), glm::vec2(0, 0)),
    //    Vertex(glm::vec4(-0.5, -0.5, -1, 1), glm::vec3(1, 1, 0), glm::vec2(0, 1)),
    //    Vertex(glm::vec4(0.5, -0.5, 1, 1), glm::vec3(0, 1, 1), glm::vec2(1, 1)),
    //};

    //Vertex lines[2] = {
    //    Vertex(glm::vec4(1, 3, 0, 1), glm::vec3(0, 0, 1), glm::vec2(0, 1)),
    //    Vertex(glm::vec4(-1, -1, 0, 1), glm::vec3(1, 0, 0), glm::vec2(0, 0)),
    //};

    ObjLoader loader;
    loader.load("models/gd5k.obj");
    _vertices = loader.getVertices();
    _numVertices = _vertices.size();
    _modelBuff = _renderer->CreateVertexBuffer(sizeof(Vertex) * _numVertices, sizeof(Vertex), _vertices.data());

    std::vector<Vertex> normalV;
    for (auto& v : _vertices) {
        Vertex v1, v2;
        v1.pos = v.pos;
        v1.color = glm::vec3(1, 0, 0);
        v2.pos = v.pos + glm::vec4(v.normal, 0.0f) * 0.25f;
        v2.color = glm::vec3(1, 0, 0);
        normalV.push_back(v1);
        normalV.push_back(v2);
    }
    _vbuff = _renderer->CreateVertexBuffer(sizeof(Vertex) * _numVertices * 2, sizeof(Vertex), normalV.data());
    //_vbuff2 = _renderer->CreateVertexBuffer(sizeof(triangle2), sizeof(Vertex), &triangle2);
    //_vbuff3 = _renderer->CreateVertexBuffer(sizeof(lines), sizeof(Vertex), &lines);
}

void Main::Update() {
    auto mouseInfo = _window->getMouseInfo();
    if (mouseInfo.isMouseLeftDown && !_wasMouseLeftDown) {
        // Mouse Down
        _oldMousePos = mouseInfo.mousePos;
    }
    if (mouseInfo.isMouseLeftDown) {
        // Mouse Downed
        auto moved = mouseInfo.mousePos - _oldMousePos;
        _curOrbitParameter = _oldOrbitParameter + glm::vec2(moved) * 0.005f;

        constexpr float pi = glm::pi<float>();
        _curOrbitParameter.x = std::max(-pi, std::min(pi, _curOrbitParameter.x));
        _curOrbitParameter.y = std::max(-pi / 2 + 0.001f, std::min(pi / 2 - 0.001f, _curOrbitParameter.y));
    }
    if (!mouseInfo.isMouseLeftDown && _wasMouseLeftDown) {
        // Mouse Up
        _oldOrbitParameter = _curOrbitParameter;
    }

    float r = 4.0f;
    float r2 = std::cos(_curOrbitParameter.y);
    _camera->SetEyePos(r * glm::vec3(r2 * std::sin(_curOrbitParameter.x),
        -std::sin(_curOrbitParameter.y), -r2 * std::cos(_curOrbitParameter.x)));
    _fragShader->_camera = _camera;

    _vertexShader->SetViewTransform(_camera->getViewTransform());
    _wasMouseLeftDown = mouseInfo.isMouseLeftDown;
}

void Main::Draw() {
    _renderer->ClearStats();
    _renderer->ClearFrameBuffer();
    //_renderer->SetDrawMode(DrawMode::WireFrame);
    _renderer->SetCullMode(CullMode::CullClockwise);
    //_renderer->DrawElements(_vbuff, 0, 6, Primitives::Triangles);
    //_renderer->DrawElements(_vbuff2, 0, 3, Primitives::Triangles);
   //_renderer->DrawElements(_vbuff, 0, _vertices.size() * 2, Primitives::Lines);
    _renderer->DrawElements(_modelBuff, 0, _numVertices, Primitives::Triangles);
    _renderer->Present();
}
