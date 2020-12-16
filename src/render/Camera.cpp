#include "Camera.h"

Camera::Camera(glm::vec3 eyePos, glm::vec3 lookat, glm::vec3 up, float fov, float aspect, float zNear, float zFar)
    :eyePos(eyePos), lookAt(lookat), up(up), fov(fov), aspect(aspect), zNear(zNear), zFar(zFar) {
}

glm::mat4 Camera::getViewTransform() const {
    return glm::lookAt(eyePos, lookAt, up);
}

glm::mat4 Camera::getProjectTransform() const {
    return glm::perspective(fov, aspect, zNear, zFar);
}
