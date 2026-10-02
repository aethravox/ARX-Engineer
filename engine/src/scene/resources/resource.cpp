// ==============================================================================
// src/scene/resources/resource.cpp
// ==============================================================================
#include "resource.hpp"
#include "core/logging.hpp"

#include <filesystem>
#include <algorithm>

namespace arx {

std::string ResourceLoader::project_root_ = ".";

ResourceLoader& ResourceLoader::instance() {
    static ResourceLoader r;
    return r;
}

void ResourceLoader::register_loader(const std::string& ext, LoaderFn fn) {
    std::string lower = ext;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    loaders_[lower] = std::move(fn);
    ARX_LOG_DEBUG("ResourceLoader: registrado loader para extension '{}'", lower);
}

ResourcePtr ResourceLoader::load(const std::string& path) {
    // Cache.
    auto cache_it = cache_.find(path);
    if (cache_it != cache_.end()) return cache_it->second;

    std::string real_path = resolve_path(path);
    if (real_path.empty()) {
        ARX_LOG_ERROR("ResourceLoader: no se pudo resolver '{}'", path);
        return nullptr;
    }

    // Detectar extensión.
    auto dot = real_path.find_last_of('.');
    if (dot == std::string::npos) {
        ARX_LOG_ERROR("ResourceLoader: sin extension: {}", real_path);
        return nullptr;
    }
    std::string ext = real_path.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    auto it = loaders_.find(ext);
    if (it == loaders_.end()) {
        ARX_LOG_ERROR("ResourceLoader: no hay loader para '.{}'", ext);
        return nullptr;
    }

    auto res = it->second(real_path);
    if (res) {
        res->set_path(path);
        cache_[path] = res;
    } else {
        ARX_LOG_ERROR("ResourceLoader: fallo cargando '{}'", path);
    }
    return res;
}

bool ResourceLoader::has_cached(const std::string& path) const {
    return cache_.find(path) != cache_.end();
}

void ResourceLoader::clear_cache() { cache_.clear(); }

void ResourceLoader::clear_cache(const std::string& path) {
    cache_.erase(path);
}

std::string ResourceLoader::resolve_path(const std::string& arx_path) {
    // res:// → project_root + path
    if (arx_path.rfind("res://", 0) == 0) {
        return project_root_ + "/" + arx_path.substr(6);
    }
    // user:// → user_data_dir + path
    if (arx_path.rfind("user://", 0) == 0) {
        // Simplificación: usar /tmp en Linux, %APPDATA% en Windows.
        return "/tmp/arx_user/" + arx_path.substr(7);
    }
    return arx_path;
}

void ResourceLoader::set_project_root(const std::string& root) {
    project_root_ = root;
}

// ===================== ResourceSaver =========================================
ResourceSaver& ResourceSaver::instance() {
    static ResourceSaver r;
    return r;
}

void ResourceSaver::register_saver(const std::string& ext, SaverFn fn) {
    std::string lower = ext;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    savers_[lower] = std::move(fn);
}

bool ResourceSaver::save(const ResourcePtr& res, const std::string& path) {
    auto dot = path.find_last_of('.');
    if (dot == std::string::npos) return false;
    std::string ext = path.substr(dot + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    auto it = savers_.find(ext);
    if (it == savers_.end()) return false;
    return it->second(res, path);
}

} // namespace arx
