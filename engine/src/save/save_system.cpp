// ==============================================================================
#include "core/variant.hpp"
// src/save/save_system.cpp
// ==============================================================================
#include "save_system.hpp"
#include "json/json.hpp"
#include "core/logging.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace arx {

namespace fs = std::filesystem;

// ===================== SaveSystem ============================================
SaveSystem& SaveSystem::instance() {
    static SaveSystem s;
    return s;
}

bool SaveSystem::save(const std::string& slot,
                       const std::unordered_map<std::string, Variant>& data) {
    fs::create_directories(save_dir_);
    json::Object obj;
    for (const auto& [k, v] : data) obj[k] = json::from_variant(v);
    std::string s = json::serialize(json::Value(std::move(obj)), true);
    std::ofstream f(save_dir_ + "/" + slot + ".json");
    if (!f) return false;
    f << s;
    return true;
}

bool SaveSystem::load(const std::string& slot,
                       std::unordered_map<std::string, Variant>& out) {
    std::ifstream f(save_dir_ + "/" + slot + ".json");
    if (!f) return false;
    std::stringstream ss; ss << f.rdbuf();
    std::string err;
    json::Value v = json::parse(ss.str(), &err);
    if (!err.empty()) { ARX_LOG_ERROR("SaveSystem load: {}", err); return false; }
    if (!v.is_object()) return false;
    for (const auto& [k, val] : v.as_object())
        out[k] = json::to_variant(val);
    return true;
}

bool SaveSystem::has_slot(const std::string& slot) const {
    return fs::exists(save_dir_ + "/" + slot + ".json");
}

bool SaveSystem::delete_slot(const std::string& slot) {
    return fs::remove(save_dir_ + "/" + slot + ".json");
}

std::vector<std::string> SaveSystem::list_slots() const {
    std::vector<std::string> out;
    if (!fs::exists(save_dir_)) return out;
    for (auto& e : fs::directory_iterator(save_dir_)) {
        if (e.path().extension() == ".json")
            out.push_back(e.path().stem().string());
    }
    return out;
}

// ===================== ConfigFile ============================================
bool ConfigFile::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    std::stringstream ss; ss << f.rdbuf();
    std::string err;
    json::Value v = json::parse(ss.str(), &err);
    if (!err.empty() || !v.is_object()) return false;
    for (const auto& [section, val] : v.as_object()) {
        if (!val.is_object()) continue;
        for (const auto& [k, inner] : val.as_object()) {
            data_[section][k] = json::to_variant(inner);
        }
    }
    return true;
}

bool ConfigFile::save(const std::string& path) const {
    json::Object root;
    for (const auto& [section, kvs] : data_) {
        json::Object sec;
        for (const auto& [k, v] : kvs) sec[k] = json::from_variant(v);
        root[section] = json::Value(std::move(sec));
    }
    std::ofstream f(path);
    if (!f) return false;
    f << json::serialize(json::Value(std::move(root)), true);
    return true;
}

void ConfigFile::set_value(const std::string& section, const std::string& key, Variant v) {
    data_[section][key] = std::move(v);
}

Variant ConfigFile::get_value(const std::string& section, const std::string& key,
                                Variant fallback) const {
    auto sit = data_.find(section);
    if (sit == data_.end()) return fallback;
    auto kit = sit->second.find(key);
    return kit != sit->second.end() ? kit->second : fallback;
}

bool ConfigFile::has_section(const std::string& s) const {
    return data_.find(s) != data_.end();
}

bool ConfigFile::has_key(const std::string& section, const std::string& key) const {
    auto sit = data_.find(section);
    if (sit == data_.end()) return false;
    return sit->second.find(key) != sit->second.end();
}

std::vector<std::string> ConfigFile::get_sections() const {
    std::vector<std::string> out;
    for (const auto& [s, _] : data_) out.push_back(s);
    return out;
}

std::vector<std::string> ConfigFile::get_keys(const std::string& section) const {
    std::vector<std::string> out;
    auto it = data_.find(section);
    if (it == data_.end()) return out;
    for (const auto& [k, _] : it->second) out.push_back(k);
    return out;
}

// ===================== ProjectSettings =======================================
ProjectSettings& ProjectSettings::instance() {
    static ProjectSettings s;
    return s;
}

bool ProjectSettings::load_from_file(const std::string& path) {
    ConfigFile cf;
    if (!cf.load(path)) return false;
    for (const auto& sec : cf.get_sections()) {
        for (const auto& key : cf.get_keys(sec)) {
            settings_[sec + "/" + key] = cf.get_value(sec, key);
        }
    }
    return true;
}

bool ProjectSettings::save_to_file(const std::string& path) const {
    ConfigFile cf;
    for (const auto& [full_key, v] : settings_) {
        auto slash = full_key.find('/');
        std::string section = (slash == std::string::npos) ? "global" : full_key.substr(0, slash);
        std::string key     = (slash == std::string::npos) ? full_key : full_key.substr(slash + 1);
        cf.set_value(section, key, v);
    }
    return cf.save(path);
}

void ProjectSettings::set_setting(const std::string& key, Variant v) {
    settings_[key] = std::move(v);
}

Variant ProjectSettings::get_setting(const std::string& key, Variant fallback) const {
    auto it = settings_.find(key);
    return it != settings_.end() ? it->second : fallback;
}

bool ProjectSettings::has_setting(const std::string& key) const {
    return settings_.find(key) != settings_.end();
}

std::string ProjectSettings::get_project_name() const {
    return get_setting("application/name", Variant(std::string("ARX App"))).to_string();
}
std::string ProjectSettings::get_main_scene() const {
    return get_setting("application/main_scene", Variant(std::string("res://scenes/main.scene"))).to_string();
}
Vector2i ProjectSettings::get_window_size() const {
    auto def = Variant(Vector2(1280, 720));
    Vector2 v = get_setting("display/window/size", def).to_vector2();
    return Vector2i((int)v.x, (int)v.y);
}
bool ProjectSettings::is_fullscreen() const {
    return get_setting("display/window/fullscreen", Variant(false)).to_bool();
}
int ProjectSettings::get_target_fps() const {
    return (int)get_setting("application/fps", Variant((int64_t)60)).to_int();
}

} // namespace arx
