// ==============================================================================
// src/resources/asset_manager.hpp — Cache de recursos cargados.
// Carga imágenes, fonts, audio, scenes con deduplicación por path.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <functional>

namespace arx {

class Texture;
class Mesh;
class Shader;
class AudioStream;
class Font;
class PackedScene;

// Tipos de recurso soportados.
enum class ResourceType : uint8_t {
    Texture, Mesh, Shader, AudioStream, Font, PackedScene, Text, Json, Custom
};

// Resource: base de todos los assets cargables.
class Resource : public Object {
public:
    friend class AssetManager;
    ARX_CLASS(Resource, Object);
public:
    friend class AssetManager;

    virtual ~Resource() = default;
    virtual ResourceType get_resource_type() const = 0;

    void set_path(const std::string& p) { path_ = p; }
    const std::string& get_path() const { return path_; }

    bool is_loaded() const { return loaded_; }

protected:
    std::string path_;
    bool        loaded_ = false;
};

// AssetManager: cache global.
class AssetManager {
public:
    static AssetManager& instance();

    // Carga asíncrona (no bloquea). Devuelve un handle.
    template<typename T>
    std::shared_ptr<T> load(const std::string& path) {
        static_assert(std::is_base_of_v<Resource, T>,
                      "T debe heredar de arx::Resource");
        auto base = load_raw(path, T::static_resource_type());
        return std::dynamic_pointer_cast<T>(base);
    }

    // Carga síncrona.
    std::shared_ptr<Resource> load_raw(const std::string& path, ResourceType type);

    // Recursos ya cargados.
    bool has(const std::string& path) const;
    std::shared_ptr<Resource> get(const std::string& path) const;
    void unload(const std::string& path);
    void unload_all();
    void unload_unused();

    // Para limpiar periódicamente.
    size_t get_loaded_count() const;
    std::vector<std::string> get_loaded_paths() const;

    // Helpers de paths.
    static std::string normalize_path(const std::string& p);
    static bool file_exists(const std::string& p);
    static std::string read_text_file(const std::string& p);
    static std::vector<uint8_t> read_binary_file(const std::string& p);

    // Registrar loaders custom.
    using LoaderFn = std::function<std::shared_ptr<Resource>(const std::string&)>;
    void register_loader(ResourceType type, LoaderFn fn);

private:
    AssetManager() = default;
    mutable std::mutex mtx_;
    std::unordered_map<std::string, std::shared_ptr<Resource>> cache_;
    std::unordered_map<ResourceType, LoaderFn> custom_loaders_;

    std::shared_ptr<Resource> load_texture_(const std::string& path);
    std::shared_ptr<Resource> load_audio_(const std::string& path);
    std::shared_ptr<Resource> load_font_(const std::string& path);
    std::shared_ptr<Resource> load_shader_(const std::string& path);
    std::shared_ptr<Resource> load_scene_(const std::string& path);
};

} // namespace arx
