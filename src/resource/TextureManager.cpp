#include "TextureManager.h"
#include "../framework/lodepng.h"
#include <iostream>
TextureManager::TextureManager() {
    // 一个黑白条纹的贴图作为默认贴图，当贴图加载不了的时候就用这个

    std::vector<unsigned char> tData;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            tData.push_back(((i + j) & 1) ? 0x7F : 0xFF);
        }
    }
    _textures.push_back(std::shared_ptr<Texture>(new Texture(8, 8, TextureColorFormat::GRAYSCALE8, tData)));
}
TextureManager::~TextureManager() {
}
int TextureManager::CreateTexture(const std::string& fileName) {
    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;

    //decode
    unsigned error = lodepng::decode(image, width, height, fileName);

    //if there's an error, display it
    if (error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        _textures.push_back(_textures[0]);
        return _textures.size() - 1;
    }
    _textures.push_back(std::shared_ptr<Texture>(new Texture(width, height, TextureColorFormat::RGBA32, image)));
    return _textures.size() - 1;
}

std::shared_ptr<Texture> TextureManager::GetTexture(int id) const {
    return _textures[id];
}
