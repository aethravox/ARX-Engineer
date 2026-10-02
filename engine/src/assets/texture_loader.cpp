// ==============================================================================
// src/assets/texture_loader.cpp — Carga de texturas.
//
// FASE 14B: Dos funciones:
// 1. load_texture_from_pixels() - sube pixels RGBA ya decodificados a OpenGL
// 2. load_texture_from_file() - carga PNG/JPEG desde archivo con stb_image
// ==============================================================================
#include "texture_loader.hpp"
#include "core/logging.hpp"

#include <glad/glad.h>
// NO definir STB_IMAGE_IMPLEMENTATION aquí (ya está en stb_image_impl.cpp)
#include "stb_image.h"

#include <fstream>
#include <vector>

namespace arx {

// Sube pixels RGBA ya decodificados (de un GLB) a OpenGL
unsigned int load_texture_from_pixels(const uint8_t* pixels, int w, int h) {
    if (!pixels || w <= 0 || h <= 0) return 0;

    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);  // GL 2.1

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    ARX_LOG_INFO("texture_loader: textura subida a OpenGL {}x{} → tex {}", w, h, tex_id);
    return tex_id;
}

// Carga una textura desde datos comprimidos en memoria (PNG/JPEG)
unsigned int load_texture_from_memory(const uint8_t* data, int data_size) {
    if (!data || data_size <= 0) return 0;

    int w, h, channels;
    stbi_uc* pixels = stbi_load_from_memory(data, data_size, &w, &h, &channels, 4);
    if (!pixels) {
        ARX_LOG_ERROR("texture_loader: stbi_load_from_memory falló (size={})", data_size);
        return 0;
    }

    unsigned int tex_id = load_texture_from_pixels(pixels, w, h);
    stbi_image_free(pixels);
    return tex_id;
}

unsigned int load_texture_from_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        ARX_LOG_ERROR("texture_loader: no se pudo abrir '{}'", path);
        return 0;
    }
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    return load_texture_from_memory(data.data(), (int)data.size());
}

} // namespace arx
