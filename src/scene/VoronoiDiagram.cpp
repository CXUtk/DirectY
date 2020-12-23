#include "VoronoiDiagram.h"
#include "Shaders.h"
VoronoiDiagram::VoronoiDiagram(std::shared_ptr<DYWindow> window) : _window(window) {
    _oldMousePos = glm::vec2(0, _height - 2);

    _width = window->getWidth();
    _height = window->getHeight();

    _renderer = std::make_shared<Renderer>(_width, _height, window->getGraphicDevice());

    _vertexShader = std::make_shared<SimpleVertexShader>();
    _vertexShader->SetViewTransform(glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));
    _vertexShader->SetProjTransform(glm::ortho(0.f, 800.f, 0.f, 600.f, 0.f, 100.f));

    _fragShader = std::make_shared<SimpleFragmentShader>();
    _fragShaderC = std::make_shared<CircleFragmentShader>();



    _renderer->SetVertexShader(_vertexShader);
    _renderer->SetFragmentShader(_fragShader);


    Vertex triangle[6] = {
        Vertex(glm::vec4(-1, 1, 0, 1), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 1)),
        Vertex(glm::vec4(-1, -1, 0, 1), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 1)),
        Vertex(glm::vec4(1, -1, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0, 0, 1)),

        Vertex(glm::vec4(-1, 1, 0, 1), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 1)),
        Vertex(glm::vec4(1, -1, 0, 1), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0, 0, 1)),
        Vertex(glm::vec4(1, 1, 0, 1), glm::vec3(1, 1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 1))
    };
    Vertex v2[2];

    _vbuff = _renderer->CreateVertexBuffer(sizeof(triangle), sizeof(Vertex), &triangle);
    _vbuff2 = _renderer->CreateVertexBuffer(sizeof(v2), sizeof(Vertex), &v2);
}

void VoronoiDiagram::Update() {
    _curMouseInfo = _window->getMouseInfo();
    _curPoint = _curMouseInfo.mousePos;

    if (_curMouseInfo.isMouseLeftDown && !_oldMouseInfo.isMouseLeftDown) {
        _points.push_back(_curPoint);
    }

    for (auto p : _points) {
        auto dir = glm::normalize(p - _curPoint);
        float f = -dir.y;
        dir.y = dir.x;
        dir.x = f;
        _lines.push_back(Line((_curPoint + p) / 2.0f, dir));
    }

    resetInputState();
}

void VoronoiDiagram::Draw() {
    _renderer->ClearFrameBuffer();
    _renderer->SetCullMode(CullMode::CullClockwise);


    constexpr int size = 4;
    _renderer->SetFragmentShader(_fragShader);
    _vertexShader->SetModelTransform(glm::identity<glm::mat4>());
    _vertexShader->apply();
    for (auto l : _lines) {
        Vertex v2[2];
        v2[0].color = v2[1].color = glm::vec3(1);
        v2[0].pos = glm::vec4(l.start + l.dir * 100000.f, 0.f, 1.0f);
        v2[1].pos = glm::vec4(l.start - l.dir * 100000.f, 0.f, 1.0f);
        _renderer->ModifyVertexBuffer(_vbuff2, 0, 0, &v2);
        _renderer->DrawElements(_vbuff2, 0, 2, Primitives::Lines);
    }

    _renderer->SetFragmentShader(_fragShaderC);
    for (auto p : _points) {
        _vertexShader->SetModelTransform(glm::translate(glm::vec3(p.x, p.y, 0)) * glm::scale(glm::vec3(size, size, 0)));
        _vertexShader->apply();
        _renderer->DrawElements(_vbuff, 0, 6, Primitives::Triangles);
    }

    _vertexShader->SetModelTransform(glm::translate(glm::vec3(_curPoint.x, _curPoint.y, 0)) * glm::scale(glm::vec3(size, size, 0)));
    _vertexShader->apply();
    _renderer->DrawElements(_vbuff, 0, 6, Primitives::Triangles);


    _renderer->Present();

    _lines.clear();
}

void VoronoiDiagram::resetInputState() {
    memcpy(&_oldMouseInfo, &_curMouseInfo, sizeof(MouseInfo));
}
