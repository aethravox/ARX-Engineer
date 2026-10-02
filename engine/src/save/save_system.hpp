// ==============================================================================
// src/save/save_system.hpp — Save/Load de partidas.
// ==============================================================================
#pragma once

#include "core/variant.hpp"

#include <string>
#include <unordered_map>

namespace arx {

// SaveSystem — persiste datos en JSON o binario en user_data_dir.
class SaveSystem {
public:
    static SaveSystem& instance();

    bool save(const std::string& slot, const std::unordered_map<std::string, Variant>& data);
    bool load(const std::string& slot, std::unordered_map<std::string, Variant>& out);
    bool has_slot(const std::string& slot) const;
    bool delete_slot(const std::string& slot);
    std::vector<std::string> list_slots() const;

    // Path base donde se guardan los slots.
    void set_save_dir(const std::string& d) { save_dir_ = d; }
    const std::string& get_save_dir() const { return save_dir_; }

private:
    SaveSystem() = default;
    std::string save_dir_ = "./saves";
};

// ConfigFile — para settings del proyecto o del usuario (key-value).
class ConfigFile {
public:
    bool load(const std::string& path);
    bool save(const std::string& path) const;

    void set_value(const std::string& section, const std::string& key, Variant v);
    Variant get_value(const std::string& section, const std::string& key,
                       Variant fallback = {}) const;
    bool has_section(const std::string& s) const;
    bool has_key(const std::string& section, const std::string& key) const;
    std::vector<std::string> get_sections() const;
    std::vector<std::string> get_keys(const std::string& section) const;

private:
    std::unordered_map<std::string, std::unordered_map<std::string, Variant>> data_;
};

// ProjectSettings — singleton con settings globales del proyecto.
class ProjectSettings {
public:
    static ProjectSettings& instance();

    bool load_from_file(const std::string& path);
    bool save_to_file(const std::string& path) const;

    void set_setting(const std::string& key, Variant v);
    Variant get_setting(const std::string& key, Variant fallback = {}) const;
    bool has_setting(const std::string& key) const;

    // Getters comunes.
    std::string get_project_name() const;
    std::string get_main_scene() const;
    Vector2i get_window_size() const;
    bool is_fullscreen() const;
    int get_target_fps() const;

private:
    ProjectSettings() = default;
    std::unordered_map<std::string, Variant> settings_;
};

} // namespace arx
