#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Texture.h"

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    int CreateTexture(const std::string& fileName);
    std::shared_ptr<Texture> GetTexture(int id) const;
private:
    std::vector<std::shared_ptr<Texture>> _textures;
};
