// ==============================================================================
// src/editor/gui/project_manager_app.hpp
//
// ProjectManagerApp — aplicación standalone del Project Manager.
// Es un MainLoop (como EditorMain) que se ejecuta cuando se abre
// arx-editor sin argumentos.
//
// Muestra una pantalla completa con:
//   - Lista de proyectos recientes (grid con iconos)
//   - Botón "New Project" → wizard
//   - Botón "Open Folder..." → scan de carpeta
//   - Filter input
//
// Cuando el usuario selecciona un proyecto, cierra el PM y el editor
// se abre con ese proyecto (vía main.cpp).
// ==============================================================================
#pragma once

#include "scene/main_loop.hpp"
#include "core/types.hpp"
#include "os/os.hpp"
#include "render/renderer.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace arx {

class ProjectManagerApp : public MainLoop {
public:
    ProjectManagerApp() = default;
    ~ProjectManagerApp() override;

    bool init(Window* win, Renderer* r);
    void start() override;
    bool process(float delta) override;
    void finish() override;

    // Proyecto seleccionado por el usuario (vacío si canceló)
    const std::string& selected_project() const { return selected_project_; }

private:
    void render_main_ui_();
    void render_new_project_wizard_();
    void scan_default_folder_();
    void scan_folder_(const std::filesystem::path& folder);

    struct RecentProject {
        std::string name;
        std::string path;
        std::string last_modified;
        std::string kind;       // "game", "app", "tool"
        std::string icon_color; // para el cuadrito de color
    };

    void setup_imgui_();

    Window*                     window_   = nullptr;
    Renderer*                   renderer_ = nullptr;
    bool                        imgui_ready_ = false;
    std::vector<RecentProject>  recent_;
    std::string                 selected_project_;

    // Filter
    char filter_buf_[256] = "";

    // Wizard state
    bool show_wizard_ = false;
    bool show_about_ = false;
    char name_buf_[256]  = "MyProject";
    char path_buf_[1024] = "";
    int  template_idx_   = 0;
};

} // namespace arx
