#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

class Camera {
public:
    Camera() = default;
    Camera(glm::vec3 eyePos, glm::vec3 lookat, glm::vec3 up, float fov, float aspect, float zNear, float zFar);
    ~Camera() = default;

    glm::mat4 getViewProjectTransform() const;

    void SetEyePos(glm::vec3 pos) {
        eyePos = pos;
    }
private:
    glm::vec3 eyePos, lookAt, up;
    float fov, aspect, zNear, zFar;
};
