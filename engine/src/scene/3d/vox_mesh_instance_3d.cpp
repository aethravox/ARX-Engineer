// ==============================================================================
// src/scene/3d/vox_mesh_instance_3d.cpp — load_glb() con múltiples meshes + texturas
// ==============================================================================
#include "vox_mesh_instance_3d.hpp"
#include "assets/glb_loader.hpp"
#include "assets/texture_loader.hpp"
#include "core/logging.hpp"
#include <glad/glad.h>

#include <algorithm>
#include <cmath>

namespace arx {

bool VoxMeshInstance3D::load_glb(const std::string& path) {
    auto result = arx::load_glb(path);
    if (!result.error.empty()) {
        ARX_LOG_ERROR("VoxMeshInstance3D: error cargando GLB '{}': {}", path, result.error);
        has_loaded_mesh_ = false;
        return false;
    }

    if (result.meshes.empty()) {
        ARX_LOG_WARN("VoxMeshInstance3D: GLB no tiene meshes: {}", path);
        has_loaded_mesh_ = false;
        return false;
    }

    // Cargar TODOS los meshes
    loaded_meshes_.clear();
    loaded_meshes_.reserve(result.meshes.size());

    // Calcular bounding box global (para auto-escalar)
    float minX = 1e30f, minY = 1e30f, minZ = 1e30f;
    float maxX = -1e30f, maxY = -1e30f, maxZ = -1e30f;

    for (const auto& srcMesh : result.meshes) {
        LoadedMesh lm;
        lm.vertices = srcMesh.vertices;
        lm.indices = srcMesh.indices;
        std::memcpy(lm.albedo, srcMesh.albedo, 4 * sizeof(float));
        lm.has_uvs = srcMesh.has_uvs;

        // Actualizar bounding box
        for (size_t i = 0; i < lm.vertices.size(); i += 8) {
            float x = lm.vertices[i];
            float y = lm.vertices[i + 1];
            float z = lm.vertices[i + 2];
            minX = std::min(minX, x); maxX = std::max(maxX, x);
            minY = std::min(minY, y); maxY = std::max(maxY, y);
            minZ = std::min(minZ, z); maxZ = std::max(maxZ, z);
        }

        // FASE 14B: Cargar textura si el mesh tiene texture_index
        if (srcMesh.texture_index >= 0 && srcMesh.texture_index < (int)result.textures.size()) {
            const auto& tex = result.textures[srcMesh.texture_index];
            if (tex.image_index >= 0 && tex.image_index < (int)result.images.size()) {
                const auto& img = result.images[tex.image_index];
                if (!img.pixels.empty()) {
                    // Subir textura a OpenGL
                    lm.texture_id = load_texture_from_pixels(img.pixels.data(), img.width, img.height);
                    lm.has_texture = (lm.texture_id != 0);
                    if (lm.has_texture) {
                        ARX_LOG_INFO("VoxMeshInstance3D: textura cargada para mesh '{}' (tex {})",
                                     srcMesh.name, lm.texture_id);
                    }
                }
            }
        }

        loaded_meshes_.push_back(std::move(lm));
    }

    // Calcular centro y escala
    loaded_center_ = glm::vec3((minX + maxX) * 0.5f, (minY + maxY) * 0.5f, (minZ + maxZ) * 0.5f);

    if (auto_scale_) {
        float sizeX = maxX - minX;
        float sizeY = maxY - minY;
        float sizeZ = maxZ - minZ;
        float maxSize = std::max({sizeX, sizeY, sizeZ});
        if (maxSize > 0.0001f) {
            loaded_scale_ = 5.0f / maxSize;
        } else {
            loaded_scale_ = 1.0f;
        }
    } else {
        loaded_scale_ = 1.0f;
    }

    // Set albedo del primer material (para el Inspector)
    if (!loaded_meshes_.empty()) {
        albedo_ = Color(loaded_meshes_[0].albedo[0], loaded_meshes_[0].albedo[1],
                        loaded_meshes_[0].albedo[2], loaded_meshes_[0].albedo[3]);
    }

    has_loaded_mesh_ = true;
    int totalVerts = 0, totalIndices = 0;
    int texCount = 0;
    for (const auto& m : loaded_meshes_) {
        totalVerts += (int)m.vertices.size() / 8;
        totalIndices += (int)m.indices.size();
        if (m.has_texture) texCount++;
    }
    ARX_LOG_INFO("VoxMeshInstance3D: GLB cargado '{}' - {} meshes, {} verts, {} indices, {} texturas, scale={:.4f}",
                 path, (int)loaded_meshes_.size(), totalVerts, totalIndices, texCount, loaded_scale_);
    return true;
}

void VoxMeshInstance3D::ensure_vbos() {
    for (auto& lm : loaded_meshes_) {
        if (lm.vbo_created || lm.vertices.empty() || lm.indices.empty()) continue;
        
        glGenBuffers(1, &lm.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, lm.vbo);
        glBufferData(GL_ARRAY_BUFFER,
            lm.vertices.size() * sizeof(float),
            lm.vertices.data(), GL_STATIC_DRAW);
        
        glGenBuffers(1, &lm.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lm.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            lm.indices.size() * sizeof(uint32_t),
            lm.indices.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        lm.vbo_created = true;
    }
}

} // namespace arx
