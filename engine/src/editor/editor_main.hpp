// ==============================================================================
// src/editor/editor_main.hpp / .cpp — Entry point del editor visual.
// Crea la ventana, inicializa ImGui, muestra splash, arranca dock manager.
// ==============================================================================
#pragma once

#include "scene/main_loop.hpp"
#include "editor/gui/export_aex_dialog.hpp"
#include "render/renderer.hpp"
#include "os/os.hpp"

#include <memory>
#include <string>
#include <vector>
#include <filesystem>

#ifdef ARX_ZEN_ENABLED
namespace zen { class VM; }
#endif

struct GLFWwindow;
struct ImGuiContext;

namespace arx {

class EditorDockManager;
class ProjectManager;
class SceneTree;

class EditorMain : public MainLoop {
public:
    ARX_CLASS(EditorMain, MainLoop);
public:

    EditorMain();
    ~EditorMain() override;

    bool init(Window* win, Renderer* renderer);
    void start() override;
    bool process(float delta) override;
    void finish() override;

    // Cargar un proyecto desde una ruta (llamado desde main.cpp cuando
    // el ProjectManager selecciona un proyecto).
    void load_project(const std::string& path);

    // Solicitar volver al Project Manager (cierra el editor, main.cpp reabre el PM).
    void request_return_to_pm() { return_to_pm_ = true; }
    bool wants_return_to_pm() const { return return_to_pm_; }

private:
    void render_splash_(float t);
    void render_main_ui_(float delta);
    void setup_imgui_();
    void shutdown_imgui_();
    void render_console_panel_();
    void run_zen_vm_();
    void stop_zen_vm_();
    void check_hot_reload_();

    Renderer*                  renderer_       = nullptr;
    std::unique_ptr<EditorDockManager> docks_;
    std::unique_ptr<ProjectManager>   project_mgr_;
    SceneTree*                 scene_tree_     = nullptr;
    bool                       splash_done_    = false;
    float                      splash_time_    = 0.0f;
    bool                       imgui_ready_    = false;
    bool                       return_to_pm_   = false;  // señal para volver al PM
    ExportAexDialog            export_aex_dialog_;

    // Popups
    bool                       show_about_         = false;
    bool                       show_documentation_ = false;
    bool                       show_theme_editor_  = false;

    // Zen VM (preview)
#ifdef ARX_ZEN_ENABLED
    std::unique_ptr<zen::VM>   vm_;
#endif
    bool                       vm_running_     = false;
    std::string                loaded_project_path_;
    std::filesystem::file_time_type main_zen_last_write_;
    bool                       main_zen_exists_ = false;
};

} // namespace arx
