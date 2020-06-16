#include "Model.h"
#include<glm\gtx\transform.hpp>
Model::Model(const std::vector<Triangle>& triangles) {
    _transform = glm::identity<glm::mat4>();
    std::copy(triangles.begin(), triangles.end(), std::back_inserter(_triangles));
    _shader = nullptr;
}

Model::~Model() {
}
