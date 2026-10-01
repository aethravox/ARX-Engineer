// ==============================================================================
// src/editor/gui/filesystem_dock.hpp / .cpp — Explorador de archivos del proyecto.
// ==============================================================================
#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace arx {

class FilesystemDock {
public:
    void render();
    void set_project_dir(const std::string& d) { project_dir_ = d; }

private:
    void render_directory(const std::filesystem::path& p);

    std::filesystem::path project_dir_;
    std::filesystem::path selected_;
};

} // namespace arx
