#pragma once
#include<glm/glm.hpp>
class Camera {
public:
    Camera();
    ~Camera();
    glm::mat4 getViewMatrix() const;
    void Rotate(float r, const glm::vec3& axis);
    void SetPosition(const glm::vec3& pos) { _position = pos; }
    glm::vec3 GetPosition() const { return _position; }
private:
    glm::vec3 _position;
    glm::vec3 _lookat;
    glm::vec3 _up;
};
