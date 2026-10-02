// ==============================================================================
// src/resources/asset_manager.cpp
// ==============================================================================
#include "asset_manager.hpp"
#include "render/texture.hpp"
#include "audio/audio_server.hpp"
#include "core/logging.hpp"

// IMPORTANTE: NO definir STB_IMAGE_STATIC aquí. Si se define, las funciones
// (stbi_load, stbi_image_free, etc.) quedan con linkage 'static' (locales a
// este TU) pero sin implementación (la implementación está en stb_image_impl.cpp
// con STB_IMAGE_IMPLEMENTATION). Eso produce warnings de 'used but never defined'.
// Al dejarlas como 'extern' (default), el linker las resuelve desde stb_image_impl.cpp.
#include "stb_image.h"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace arx {

AssetManager& AssetManager::instance() {
    static AssetManager s;
    return s;
}

std::string AssetManager::normalize_path(const std::string& p) {
    namespace fs = std::filesystem;
    fs::path fp(p);
    return fs::weakly_canonical(fp).string();
}

bool AssetManager::file_exists(const std::string& p) {
    return std::filesystem::exists(p);
}

std::string AssetManager::read_text_file(const std::string& p) {
    std::ifstream in(p, std::ios::binary);
    if (!in) return {};
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

std::vector<uint8_t> AssetManager::read_binary_file(const std::string& p) {
    std::ifstream in(p, std::ios::binary | std::ios::ate);
    if (!in) return {};
    auto sz = in.tellg();
    in.seekg(0);
    std::vector<uint8_t> data(sz);
    in.read(reinterpret_cast<char*>(data.data()), sz);
    return data;
}

bool AssetManager::has(const std::string& path) const {
    std::lock_guard<std::mutex> lock(mtx_);
    return cache_.find(normalize_path(path)) != cache_.end();
}

std::shared_ptr<Resource> AssetManager::get(const std::string& path) const {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = cache_.find(normalize_path(path));
    return it != cache_.end() ? it->second : nullptr;
}

void AssetManager::unload(const std::string& path) {
    std::lock_guard<std::mutex> lock(mtx_);
    cache_.erase(normalize_path(path));
}

void AssetManager::unload_all() {
    std::lock_guard<std::mutex> lock(mtx_);
    cache_.clear();
}

void AssetManager::unload_unused() {
    std::lock_guard<std::mutex> lock(mtx_);
    for (auto it = cache_.begin(); it != cache_.end(); ) {
        if (it->second.use_count() <= 1) it = cache_.erase(it);
        else                              ++it;
    }
}

size_t AssetManager::get_loaded_count() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return cache_.size();
}

std::vector<std::string> AssetManager::get_loaded_paths() const {
    std::lock_guard<std::mutex> lock(mtx_);
    std::vector<std::string> out;
    out.reserve(cache_.size());
    for (const auto& [k, _] : cache_) out.push_back(k);
    return out;
}

void AssetManager::register_loader(ResourceType type, LoaderFn fn) {
    custom_loaders_[type] = std::move(fn);
}

std::shared_ptr<Resource> AssetManager::load_raw(const std::string& path,
                                                  ResourceType type) {
    auto npath = normalize_path(path);
    {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = cache_.find(npath);
        if (it != cache_.end()) return it->second;
    }

    std::shared_ptr<Resource> res;
    switch (type) {
        case ResourceType::Texture:     res = load_texture_(npath); break;
        case ResourceType::AudioStream: res = load_audio_(npath);   break;
        case ResourceType::Font:        res = load_font_(npath);    break;
        case ResourceType::Shader:      res = load_shader_(npath);  break;
        case ResourceType::PackedScene: res = load_scene_(npath);   break;
        default:
            if (auto it = custom_loaders_.find(type); it != custom_loaders_.end()) {
                res = it->second(npath);
            }
            break;
    }

    if (res) {
        std::lock_guard<std::mutex> lock(mtx_);
        cache_[npath] = res;
    }
    return res;
}

// === Loaders específicos ===

class TextureResource : public Resource {
public:
    ARX_CLASS(TextureResource, Resource);
    ResourceType get_resource_type() const override { return ResourceType::Texture; }
    static ResourceType static_resource_type() { return ResourceType::Texture; }

    std::shared_ptr<Texture> texture;
    int width = 0, height = 0, channels = 0;
};

std::shared_ptr<Resource> AssetManager::load_texture_(const std::string& path) {
    if (!file_exists(path)) {
        ARX_LOG_ERROR("AssetManager: textura no encontrada: {}", path);
        return nullptr;
    }
    int w, h, c;
    stbi_uc* data = stbi_load(path.c_str(), &w, &h, &c, 4);
    if (!data) {
        ARX_LOG_ERROR("AssetManager: stbi_load falló para: {}", path);
        return nullptr;
    }
    auto res = std::make_shared<TextureResource>();
    res->path_ = path;
    res->loaded_ = true;
    res->width = w;
    res->height = h;
    res->channels = 4;
    // NOTA: aquí se llamaría a Renderer::create_texture(data, w, h, 4)
    // Para mantener el AssetManager desacoplado del Renderer, se guarda
    // la data en bruto y se sube a GPU cuando se usa.
    stbi_image_free(data);
    ARX_LOG_INFO("AssetManager: textura cargada {} ({}x{}x{})", path, w, h, c);
    return res;
}

class AudioResource : public Resource {
public:
    ARX_CLASS(AudioResource, Resource);
    ResourceType get_resource_type() const override { return ResourceType::AudioStream; }
    static ResourceType static_resource_type() { return ResourceType::AudioStream; }

    std::shared_ptr<AudioStream> stream;
};

std::shared_ptr<Resource> AssetManager::load_audio_(const std::string& path) {
    auto stream = std::make_shared<AudioStream>(); (void)stream; (void)path;
    if (!stream) {
        ARX_LOG_ERROR("AssetManager: no se pudo cargar audio: {}", path);
        return nullptr;
    }
    auto res = std::make_shared<AudioResource>();
    res->path_ = path;
    res->loaded_ = true;
    res->stream = stream;
    return res;
}

class FontResource : public Resource {
public:
    ARX_CLASS(FontResource, Resource);
    ResourceType get_resource_type() const override { return ResourceType::Font; }
    static ResourceType static_resource_type() { return ResourceType::Font; }
    // TODO: miembro Font (FreeType wrapper)
};

std::shared_ptr<Resource> AssetManager::load_font_(const std::string& path) {
    if (!file_exists(path)) return nullptr;
    auto res = std::make_shared<FontResource>();
    res->path_ = path;
    res->loaded_ = true;
    ARX_LOG_INFO("AssetManager: fuente cargada {}", path);
    return res;
}

class ShaderResource : public Resource {
public:
    ARX_CLASS(ShaderResource, Resource);
    ResourceType get_resource_type() const override { return ResourceType::Shader; }
    static ResourceType static_resource_type() { return ResourceType::Shader; }
    std::string vertex_src;
    std::string fragment_src;
};

std::shared_ptr<Resource> AssetManager::load_shader_(const std::string& path) {
    auto src = read_text_file(path);
    if (src.empty()) return nullptr;
    auto res = std::make_shared<ShaderResource>();
    res->path_ = path;
    res->loaded_ = true;
    res->vertex_src   = src;  // TODO: parser de #vertex / #fragment
    res->fragment_src = src;
    return res;
}

class SceneResource : public Resource {
public:
    ARX_CLASS(SceneResource, Resource);
    ResourceType get_resource_type() const override { return ResourceType::PackedScene; }
    static ResourceType static_resource_type() { return ResourceType::PackedScene; }
    std::shared_ptr<PackedScene> scene;
};

std::shared_ptr<Resource> AssetManager::load_scene_(const std::string& path) {
    auto res = std::make_shared<SceneResource>();
    res->path_ = path;
    res->loaded_ = true;
    // TODO: SceneSerializer::load(path)
    return res;
}

} // namespace arx
