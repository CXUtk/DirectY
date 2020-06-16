#include "Camera.h"
#include <glm/gtx/transform.hpp>
Camera::Camera() {
    _position = glm::vec3(0, 0, 0);
    _lookat = glm::vec3(0, 0, 0);
    _up = glm::vec3(0, 1, 0);
}

Camera::~Camera() {
}

static glm::mat4 view_matrix(glm::vec3 eyePos, glm::vec3 lookat, glm::vec3 up) {
    return glm::lookAtRH(eyePos, lookat, up);
}

glm::mat4 Camera::getViewMatrix() const {
    return view_matrix(_position, _lookat, _up);
}

void Camera::Rotate(float r, const glm::vec3& axis) {
}
