// ==============================================================================
// src/editor/icon_manager.cpp — Implementación del gestor de iconos SVG.
//
// Compila nanosvg dentro de este archivo (header-only) para no tener que
// tocar el CMakeLists de arx_core. Solo necesita el include path a
// thirdparty/nanosvg/ en arx_editor_lib.
// ==============================================================================

// === Compilar nanosvg inline ===
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

#include "icon_manager.hpp"
#include "core/logging.hpp"

#include <glad/glad.h>
#include <imgui.h>

#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace arx {

IconManager& IconManager::get() {
    static IconManager instance;
    return instance;
}

IconManager::~IconManager() {
    shutdown();
}

void IconManager::init(const std::string& icons_dir, int icon_size) {
    icons_dir_ = icons_dir;
    icon_size_ = icon_size;
    ARX_LOG_INFO("IconManager: init dir='{}' size={}", icons_dir_, icon_size_);
}

void IconManager::shutdown() {
    for (auto& [name, ci] : cache_) {
        unload_(ci);
    }
    cache_.clear();
}

uintptr_t IconManager::texture(const std::string& name) {
    auto it = cache_.find(name);
    if (it == cache_.end()) {
        // Crear entrada nueva
        CachedIcon ci;
        ci.path = icons_dir_ + "/" + name + ".svg";
        load_or_reload_(ci);
        auto [inserted_it, _] = cache_.emplace(name, std::move(ci));
        return inserted_it->second.texture_id;
    }
    // Verificar hot reload
    load_or_reload_(it->second);
    return it->second.texture_id;
}

ImTextureID IconManager::imgui_texture(const std::string& name) {
    return static_cast<ImTextureID>(texture(name));
}

void IconManager::load_or_reload_(CachedIcon& ci) {
    // Verificar mtime
    struct stat st;
    if (stat(ci.path.c_str(), &st) != 0) {
        // No existe
        if (ci.texture_id != 0) {
            unload_(ci);
        }
        return;
    }
    int64_t current_mtime = (int64_t)st.st_mtime;
    if (ci.texture_id != 0 && current_mtime == ci.mtime) {
        // Sin cambios
        return;
    }
    // Necesita cargar/recargar
    if (ci.texture_id != 0) {
        ARX_LOG_INFO("IconManager: hot reload '{}'", ci.path);
        unload_(ci);
    }

    // Cargar SVG con nanosvg
    NSVGimage* image = nsvgParseFromFile(ci.path.c_str(), "px", 96.0f);
    if (!image) {
        ARX_LOG_WARN("IconManager: no se pudo parsear '{}'", ci.path);
        return;
    }

    // Crear rasterizer
    NSVGrasterizer* rast = nsvgCreateRasterizer();
    if (!rast) {
        ARX_LOG_WARN("IconManager: no se pudo crear rasterizer");
        nsvgDelete(image);
        return;
    }

    // Buffer RGBA
    int w = icon_size_;
    int h = icon_size_;
    unsigned char* pixels = (unsigned char*)malloc(w * h * 4);
    if (!pixels) {
        nsvgDeleteRasterizer(rast);
        nsvgDelete(image);
        return;
    }

    // Rasterizar. La escala se calcula para que el SVG quepa en w×h.
    float scale = (float)w / image->width;
    if (image->height > image->width) {
        scale = (float)h / image->height;
    }
    // Centrar
    float tx = ((float)w - image->width * scale) * 0.5f;
    float ty = ((float)h - image->height * scale) * 0.5f;
    // stride = w * 4 (RGBA, tightly packed)
    nsvgRasterize(rast, image, tx, ty, scale, pixels, w, h, w * 4);

    // Subir a OpenGL
    GLuint tex_id;
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    // OpenGL 2.1: GL_GENERATE_MIPMAP en vez de glGenerateMipmap
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Limpieza nanosvg
    free(pixels);
    nsvgDeleteRasterizer(rast);
    nsvgDelete(image);

    ci.texture_id = (uintptr_t)tex_id;
    ci.mtime = current_mtime;
    ARX_LOG_INFO("IconManager: cargado '{}' → tex {}", ci.path, tex_id);
}

void IconManager::unload_(CachedIcon& ci) {
    if (ci.texture_id != 0) {
        GLuint tex = (GLuint)ci.texture_id;
        glDeleteTextures(1, &tex);
        ci.texture_id = 0;
    }
}

} // namespace arx
