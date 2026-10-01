// ==============================================================================
// src/asset_pipeline/asset_pipeline.cpp
// ==============================================================================
#include "asset_pipeline.hpp"
#include "core/logging.hpp"

#include "stb_image.h"
#include <fstream>
#include <thread>
#include <chrono>
#include <algorithm>

namespace arx {

// ===================== TextureImporter ======================================
bool TextureImporter::import(const fs::path& source, const fs::path& target,
                                const std::unordered_map<std::string, std::string>& opts) {
    int w, h, ch;
    unsigned char* data = stbi_load(source.string().c_str(), &w, &h, &ch, 4);
    if (!data) {
        ARX_LOG_ERROR("TextureImporter: fallo cargar {}", source.string());
        return false;
    }

    // Premultiply alpha si necesario.
    if (premultiply_alpha_) {
        for (int i = 0; i < w * h; ++i) {
            float a = data[i*4 + 3] / 255.0f;
            data[i*4 + 0] = (unsigned char)(data[i*4 + 0] * a);
            data[i*4 + 1] = (unsigned char)(data[i*4 + 1] * a);
            data[i*4 + 2] = (unsigned char)(data[i*4 + 2] * a);
        }
    }

    // Resize si supera max_size_.
    if (w > max_size_ || h > max_size_) {
        // Simplificación: en una impl completa usar stb_image_resize.
        ARX_LOG_WARN("TextureImporter: imagen {} más grande que max_size {} — skipping resize",
                     source.string(), max_size_);
    }

    // Mipmaps (simplificado: en producción se generaría con cvtt/squish).
    int mipmap_count = 1;
    if (generate_mipmaps_) {
        mipmap_count = static_cast<int>(std::log2(std::max(w, h))) + 1;
    }

    // Compresión (stub): en una impl real se usaría cvtt (BC7), etc2comp (ETC2),
    // o pvrtccompressor (PVRTC). Aquí solo marcamos el formato.
    std::string compression_str = "none";
    switch (compression_) {
        case Compression::None:   compression_str = "none"; break;
        case Compression::BC1:    compression_str = "bc1";  break;
        case Compression::BC3:    compression_str = "bc3";  break;
        case Compression::BC5:    compression_str = "bc5";  break;
        case Compression::BC7:    compression_str = "bc7";  break;
        case Compression::ETC2:   compression_str = "etc2"; break;
        case Compression::PVRTC:  compression_str = "pVRTC";break;
        case Compression::ASTC:   compression_str = "astc"; break;
    }

    // Escribir .texture (formato interno).
    std::ofstream f(target, std::ios::binary);
    if (!f) { stbi_image_free(data); return false; }

    // Header
    struct TextureHeader {
        uint32_t magic = 0xARXT('A','R','X','T'); // ARX Texture
        uint32_t width;
        uint32_t height;
        uint32_t channels = 4;
        uint32_t compression;       // enum value
        uint32_t mipmap_count;
        uint32_t data_size;
    } header;
    header.width         = w;
    header.height        = h;
    header.compression   = (uint32_t)compression_;
    header.mipmap_count  = mipmap_count;
    header.data_size     = w * h * 4;
    f.write(reinterpret_cast<char*>(&header), sizeof(header));
    f.write(reinterpret_cast<char*>(data), header.data_size);
    stbi_image_free(data);

    (void)opts;
    ARX_LOG_INFO("TextureImporter: {} → {} ({}x{}, {}, {} mips)",
                 source.filename().string(), target.filename().string(),
                 w, h, compression_str, mipmap_count);
    return true;
}

// ===================== MeshImporter ==========================================
bool MeshImporter::import(const fs::path& source, const fs::path& target,
                            const std::unordered_map<std::string, std::string>& opts) {
    // Stub: en una impl completa se parsearía glTF/GLB con una librería
    // como cgltf, se optimizaría con meshopt, y se escribiría un .mesh binario.
    ARX_LOG_INFO("MeshImporter: {} → {} (opts: optimize={}, tangents={})",
                 source.filename().string(), target.filename().string(),
                 optimize_, generate_tangents_);
    (void)opts;

    // Escribir header mínimo.
    std::ofstream f(target, std::ios::binary);
    if (!f) return false;
    struct { uint32_t magic = 0x41524D58; uint32_t version = 1; uint32_t vertex_count = 0; uint32_t index_count = 0; } h;
    f.write(reinterpret_cast<char*>(&h), sizeof(h));
    return true;
}

// ===================== AudioImporter =========================================
bool AudioImporter::import(const fs::path& source, const fs::path& target,
                              const std::unordered_map<std::string, std::string>& opts) {
    ARX_LOG_INFO("AudioImporter: {} → {} (format: {}, {}Hz)",
                 source.filename().string(), target.filename().string(),
                 (int)format_, sample_rate_);
    (void)opts;
    std::ofstream f(target, std::ios::binary);
    if (!f) return false;
    struct { uint32_t magic = 0x41524158; uint32_t format; uint32_t sample_rate; uint32_t channels; uint32_t data_size = 0; } h;
    h.format = (uint32_t)format_;
    h.sample_rate = sample_rate_;
    h.channels = 2;
    f.write(reinterpret_cast<char*>(&h), sizeof(h));
    return true;
}

// ===================== FontImporter ==========================================
bool FontImporter::import(const fs::path& source, const fs::path& target,
                            const std::unordered_map<std::string, std::string>& opts) {
    ARX_LOG_INFO("FontImporter: {} → {} (sizes: {})", source.filename().string(),
                 target.filename().string(), sizes_.size());
    (void)opts;
    std::ofstream f(target, std::ios::binary);
    if (!f) return false;
    struct { uint32_t magic = 0x41524658; uint32_t version = 1; uint32_t size_count; } h;
    h.size_count = (uint32_t)sizes_.size();
    f.write(reinterpret_cast<char*>(&h), sizeof(h));
    for (int s : sizes_) f.write(reinterpret_cast<char*>(&s), sizeof(s));
    return true;
}

// ===================== ShaderImporter ========================================
bool ShaderImporter::import(const fs::path& source, const fs::path& target,
                              const std::unordered_map<std::string, std::string>& opts) {
    ARX_LOG_INFO("ShaderImporter: {} → {}", source.filename().string(), target.filename().string());
    (void)opts;
    std::ifstream src(source);
    std::ofstream dst(target, std::ios::binary);
    if (!src || !dst) return false;
    dst << src.rdbuf();
    return true;
}

// ===================== AssetPipeline =========================================
AssetPipeline& AssetPipeline::instance() {
    static AssetPipeline p;
    return p;
}

void AssetPipeline::register_importer(std::shared_ptr<AssetImporter> importer) {
    if (importer) importers_.push_back(std::move(importer));
}

bool AssetPipeline::import(const fs::path& source, const fs::path& target,
                              const std::unordered_map<std::string, std::string>& options) {
    std::string ext = source.extension().string().substr(1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    for (auto& imp : importers_) {
        if (imp->source_extension() == ext) {
            if (imp->import(source, target, options)) {
                ++imported_count_;
                return true;
            }
            ++failed_count_;
            return false;
        }
    }
    ARX_LOG_WARN("AssetPipeline: no importer for extension '.{}'", ext);
    ++failed_count_;
    return false;
}

int AssetPipeline::import_directory(const fs::path& source_dir, const fs::path& target_dir,
                                       bool force) {
    if (!fs::exists(source_dir)) return 0;
    fs::create_directories(target_dir);
    int count = 0;
    for (auto& e : fs::recursive_directory_iterator(source_dir)) {
        if (!e.is_regular_file()) continue;
        fs::path rel = fs::relative(e.path(), source_dir);
        fs::path target = target_dir / rel;
        target.replace_extension();  // Sin extensión, se la da el importer

        if (!force && fs::exists(target) &&
            fs::last_write_time(target) > fs::last_write_time(e.path()))
            continue;

        fs::create_directories(target.parent_path());
        if (import(e.path(), target)) ++count;
    }
    ARX_LOG_INFO("AssetPipeline: {} archivos importados de {}", count, source_dir.string());
    return count;
}

void AssetPipeline::start_watcher(const fs::path& source_dir) {
    watching_ = true;
    std::thread([this, source_dir]() {
        while (watching_) {
            for (auto& e : fs::recursive_directory_iterator(source_dir)) {
                if (!e.is_regular_file()) continue;
                auto path = e.path().string();
                auto mtime = fs::last_write_time(e.path());
                auto it = last_modified_.find(path);
                if (it == last_modified_.end() || it->second != mtime) {
                    last_modified_[path] = mtime;
                    ARX_LOG_INFO("Watcher: cambio detectado en {}", path);
                    // Re-importar.
                    fs::path target = e.path();
                    target.replace_extension();
                    import(e.path(), target);
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }).detach();
}

void AssetPipeline::stop_watcher() {
    watching_ = false;
}

} // namespace arx
