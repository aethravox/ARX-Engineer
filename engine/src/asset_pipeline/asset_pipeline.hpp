// ==============================================================================
// src/asset_pipeline/asset_pipeline.hpp — Importación de assets.
//
// Pipeline:
//   raw asset (PNG, glTF, WAV, OGG) → process → .arximport (cached)
//   → load at runtime via ResourceLoader
//
// Steps:
//   1. Import: leer el archivo crudo.
//   2. Process: comprimir texturas (BC7/ETC2/PVRTC), optimizar meshes,
//      transcodificar audio.
//   3. Cache: guardar resultado en .arximport (binary format).
//   4. Load: ResourceLoader sirve el .arximport al renderer.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "render/texture.hpp"

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <filesystem>

namespace arx {

namespace fs = std::filesystem;

// AssetImporter — interfaz para importar un tipo de asset.
class AssetImporter {
public:
    virtual ~AssetImporter() = default;
    virtual std::string source_extension() const = 0;
    virtual std::string target_extension() const = 0;
    virtual bool import(const fs::path& source, const fs::path& target,
                          const std::unordered_map<std::string, std::string>& options) = 0;
};

// TextureImporter — importa PNG/JPG/WebP/TGA/BMP a texturas comprimidas.
class TextureImporter : public AssetImporter {
public:
    enum class Compression { None, BC1, BC3, BC5, BC7, ETC2, PVRTC, ASTC };

    std::string source_extension() const override { return "png"; }
    std::string target_extension() const override { return "texture"; }

    bool import(const fs::path& source, const fs::path& target,
                const std::unordered_map<std::string, std::string>& options) override;

    void set_compression(Compression c) { compression_ = c; }
    void set_generate_mipmaps(bool g) { generate_mipmaps_ = g; }
    void set_premultiply_alpha(bool p) { premultiply_alpha_ = p; }
    void set_max_size(int s) { max_size_ = s; }

private:
    Compression compression_     = Compression::BC7;
    bool        generate_mipmaps_ = true;
    bool        premultiply_alpha_ = false;
    int         max_size_         = 4096;
};

// MeshImporter — importa glTF/GLB/OBJ/FBX a meshes optimizados.
class MeshImporter : public AssetImporter {
public:
    std::string source_extension() const override { return "gltf"; }
    std::string target_extension() const override { return "mesh"; }

    bool import(const fs::path& source, const fs::path& target,
                const std::unordered_map<std::string, std::string>& options) override;

    void set_optimize(bool o) { optimize_ = o; }
    void set_generate_tangents(bool g) { generate_tangents_ = g; }
    void set_merge_duplicates(bool m) { merge_duplicates_ = m; }
    void set_compress_vertices(bool c) { compress_vertices_ = c; }

private:
    bool optimize_           = true;
    bool generate_tangents_  = true;
    bool merge_duplicates_   = true;
    bool compress_vertices_  = false;
};

// AudioImporter — importa WAV/MP3/OGG a formato comprimido interno.
class AudioImporter : public AssetImporter {
public:
    enum class Format { PCM16, Vorbis, Opus };

    std::string source_extension() const override { return "wav"; }
    std::string target_extension() const override { return "audio"; }

    bool import(const fs::path& source, const fs::path& target,
                const std::unordered_map<std::string, std::string>& options) override;

    void set_format(Format f) { format_ = f; }
    void set_target_sample_rate(int r) { sample_rate_ = r; }
    void set_normalize(bool n) { normalize_ = n; }
    void set_loop(bool l) { loop_ = l; }

private:
    Format format_       = Format::Vorbis;
    int    sample_rate_  = 44100;
    bool   normalize_    = true;
    bool   loop_         = false;
};

// FontImporter — importa TTF/OTF a font atlas.
class FontImporter : public AssetImporter {
public:
    std::string source_extension() const override { return "ttf"; }
    std::string target_extension() const override { return "font"; }

    bool import(const fs::path& source, const fs::path& target,
                const std::unordered_map<std::string, std::string>& options) override;

    void set_sizes(const std::vector<int>& s) { sizes_ = s; }
    void set_chars(const std::string& c) { chars_ = c; }

private:
    std::vector<int> sizes_ = {12, 14, 16, 20, 24, 32, 48, 64};
    std::string chars_ = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
};

// ShaderImporter — importa GLSL a shaders compilados (SPIR-V o cache binario).
class ShaderImporter : public AssetImporter {
public:
    std::string source_extension() const override { return "glsl"; }
    std::string target_extension() const override { return "shader"; }

    bool import(const fs::path& source, const fs::path& target,
                const std::unordered_map<std::string, std::string>& options) override;
};

// AssetPipeline — orquestador del proceso de import.
class AssetPipeline {
public:
    static AssetPipeline& instance();

    void register_importer(std::shared_ptr<AssetImporter> importer);
    bool import(const fs::path& source, const fs::path& target,
                const std::unordered_map<std::string, std::string>& options = {});

    // Importar todo un directorio recursivamente.
    int  import_directory(const fs::path& source_dir, const fs::path& target_dir,
                            bool force = false);

    // Watcher: vigilar cambios en source_dir y re-importar.
    void start_watcher(const fs::path& source_dir);
    void stop_watcher();
    bool is_watching() const { return watching_; }

    // Stats.
    int imported_count() const { return imported_count_; }
    int failed_count() const { return failed_count_; }

private:
    AssetPipeline() = default;
    std::vector<std::shared_ptr<AssetImporter>> importers_;
    bool watching_ = false;
    int imported_count_ = 0;
    int failed_count_ = 0;
    std::unordered_map<std::string, fs::file_time_type> last_modified_;
};

} // namespace arx
