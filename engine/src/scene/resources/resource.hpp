// ==============================================================================
// src/scene/resources/resource.hpp — Base de recursos cargables.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace arx {

// Resource — cualquier asset cargable: texture, mesh, audio stream, scene, etc.
class Resource : public Object {
public:
    friend class AssetManager;
    ARX_CLASS(Resource, Object);
public:
    friend class AssetManager;

    void set_path(const std::string& p) { path_ = p; }
    const std::string& get_path() const { return path_; }

    // ¿Es válido? Overridear en subclases.
    virtual bool is_valid() const { return true; }

    // Serialización básica.
    virtual void to_dict(std::unordered_map<std::string, Variant>& /*out*/) const {}
    virtual void from_dict(const std::unordered_map<std::string, Variant>& /*in*/) {}

protected:
    std::string path_;   // "res://textures/player.png"
};

using ResourcePtr = std::shared_ptr<Resource>;

// ResourceLoader — cache + cargadores por extensión.
class ResourceLoader {
public:
    friend class AssetManager;
    using LoaderFn = std::function<ResourcePtr(const std::string&)>;

    static ResourceLoader& instance();

    void register_loader(const std::string& extension, LoaderFn fn);
    ResourcePtr load(const std::string& path);
    template<typename T>
    std::shared_ptr<T> load_as(const std::string& path) {
        return std::dynamic_pointer_cast<T>(load(path));
    }

    bool has_cached(const std::string& path) const;
    void clear_cache();
    void clear_cache(const std::string& path);

    // Conversión de paths "res://" → rutas reales del filesystem.
    static std::string resolve_path(const std::string& arx_path);
    static void set_project_root(const std::string& root);

private:
    ResourceLoader() = default;
    std::unordered_map<std::string, LoaderFn> loaders_;
    std::unordered_map<std::string, ResourcePtr> cache_;
    static std::string project_root_;
};

// ResourceSaver — guarda recursos a disco.
class ResourceSaver {
public:
    friend class AssetManager;
    using SaverFn = std::function<bool(const ResourcePtr&, const std::string&)>;

    static ResourceSaver& instance();
    void register_saver(const std::string& extension, SaverFn fn);
    bool save(const ResourcePtr& res, const std::string& path);

private:
    ResourceSaver() = default;
    std::unordered_map<std::string, SaverFn> savers_;
};

} // namespace arx
