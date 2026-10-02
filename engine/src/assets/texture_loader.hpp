// ==============================================================================
// src/assets/texture_loader.hpp — Carga de texturas.
// ==============================================================================
#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace arx {

// Sube pixels RGBA ya decodificados a OpenGL (para texturas de GLB)
unsigned int load_texture_from_pixels(const uint8_t* pixels, int w, int h);

// Carga textura desde datos comprimidos en memoria (PNG/JPEG)
unsigned int load_texture_from_memory(const uint8_t* data, int data_size);

// Carga textura desde archivo
unsigned int load_texture_from_file(const std::string& path);

} // namespace arx
