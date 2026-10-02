// ==============================================================================
// src/editor/gui/filesystem_dock.hpp / .cpp — Explorador de archivos del proyecto.
// ==============================================================================
#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <functional>

namespace arx {

class FilesystemDock {
public:
    void render();
    void set_project_dir(const std::string& d) { project_dir_ = d; }

    // Callback: cuando se hace doble-click en un archivo .zen
    // El Code Editor se registra acá para recibir los archivos a abrir.
    std::function<void(const std::string&)> on_open_zen_file;

private:
    void render_directory(const std::filesystem::path& p);

    std::filesystem::path project_dir_;
    std::filesystem::path selected_;
};

} // namespace arx
