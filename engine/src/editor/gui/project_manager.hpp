// ==============================================================================
// src/editor/gui/project_manager.hpp / .cpp — Gestor de proyectos.
// Permite crear proyectos nuevos (juego o app) y abrir existentes.
// ==============================================================================
#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace arx {

class ProjectManager {
public:
    void render();

    void show_open_dialog() { open_ = true; }
    void show_new_dialog()  { mode_ = Mode::New;  open_ = true; }

    bool has_open_project() const { return !current_project_.empty(); }
    const std::string& get_current_project() const { return current_project_; }
    void set_current_project(const std::string& path) { current_project_ = path; }

private:
    enum class Mode { Browse, New } mode_ = Mode::Browse;

    void render_browse();
    void render_new_project_wizard();

    void scan_recent_projects();

    struct RecentProject {
        std::string name;
        std::string path;
        std::string last_modified;
        std::string kind;   // "game" o "app"
    };

    bool                     open_ = false;
    std::vector<RecentProject> recent_;
    std::string              current_project_;

    // Wizard state.
    char name_buf_[256]  = "MyProject";
    char path_buf_[1024] = "";
    int  template_idx_   = 0;
    bool template_2d_    = true;
    bool template_3d_    = false;
};

} // namespace arx
