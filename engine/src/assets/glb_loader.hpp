// ==============================================================================
// src/assets/glb_loader.hpp — Cargador de GLB (glTF Binary) con texturas.
//
// FASE 14B: Soporta texturas embebidas (PNG/JPEG) en el material baseColorTexture.
// ==============================================================================
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace arx {

struct GLBMesh {
    std::string name;
    std::vector<float> vertices;   // pos(3) + normal(3) + uv(2) = 8 floats per vertex
    std::vector<uint32_t> indices;
    float albedo[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    int material_index = -1;
    int texture_index = -1;  // índice de la textura (-1 si no tiene)
    bool has_uvs = false;     // ¿el mesh tiene UVs?
};

struct GLBTexture {
    int image_index = -1;     // índice en images[]
    int sampler_index = -1;
};

struct GLBImage {
    std::string name;
    std::string mime_type;    // "image/png", "image/jpeg"
    int buffer_view = -1;     // si está embebida
    std::string uri;          // si es externa (data URI o path)
    // Datos decodificados (raw pixels)
    std::vector<uint8_t> pixels;
    int width = 0;
    int height = 0;
    int channels = 0;
};

struct GLBResult {
    std::vector<GLBMesh> meshes;
    std::vector<GLBTexture> textures;
    std::vector<GLBImage> images;
    std::string error;
};

GLBResult load_glb(const std::string& path);

} // namespace arx
