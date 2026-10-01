// ==============================================================================
// src/main.cpp — Entry point del ARX Engine.
//
// Modos de ejecución:
//   arx-editor                          → abre el Project Manager (pantalla completa)
//   arx-editor /path/to/project         → abre el Editor con el proyecto cargado
//   arx-editor --project /path/to/proj  → idem
// ==============================================================================
#include "core/logging.hpp"
#include "core/object.hpp"
#include "os/os.hpp"
#include "render/renderer.hpp"
#include "editor/editor_main.hpp"
#include "editor/gui/project_manager.hpp"
#include "editor/gui/project_manager_app.hpp"
#include "modules/register_module_types.hpp"

#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

// Variable global para comunicación PM → main loop (definida acá, usada en project_manager_app.cpp)
namespace arx { extern std::string g_selected_project; }
namespace arx { std::string g_selected_project; }

// Modo Project Manager: pantalla completa, sin editor docks
static int run_project_manager(arx::OS* os) {
    using namespace arx;

    WindowCreateInfo wci;
    wci.title    = "ARX Engine — Project Manager";
    // Detectar resolución del monitor
    {
        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        if (mon) {
            const GLFWvidmode* mode = glfwGetVideoMode(mon);
            wci.width  = std::min(mode->width - 40, 1024);
            wci.height = std::min(mode->height - 80, 680);
        } else {
            wci.width  = 1024;
            wci.height = 680;
        }
    }
    wci.gl_major = 2;
    wci.gl_minor = 1;
    wci.gl_compat = true;
    auto window = os->create_window(wci);
    if (!window) {
        ARX_LOG_FATAL("No se pudo crear la ventana del Project Manager");
        return 1;
    }
    window->set_vsync(true);

    auto renderer = Renderer::create(Renderer::Backend::OpenGL);
    RendererConfig rc;
    rc.viewport_w = wci.width;
    rc.viewport_h = wci.height;
    if (!renderer->init(rc)) {
        ARX_LOG_FATAL("No se pudo inicializar el renderer");
        return 1;
    }

    // El ProjectManager es un MainLoop standalone
    ProjectManagerApp pm;
    if (!pm.init(window.get(), renderer.get())) {
        ARX_LOG_FATAL("No se pudo inicializar el Project Manager");
        return 1;
    }

    os->set_main_loop(&pm);
    os->run();

    pm.finish();
    renderer->shutdown();
    return 0;
}

// Modo Editor: abre el editor con un proyecto cargado
// Retorna true si el usuario quiere volver al PM, false si quiere salir.
static bool run_editor(arx::OS* os, const std::string& project_path) {
    using namespace arx;

    WindowCreateInfo wci;
    wci.title    = "ARX Engine — Editor";
    // Detectar resolución del monitor y ajustar (no más grande que la pantalla)
    {
        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        if (mon) {
            const GLFWvidmode* mode = glfwGetVideoMode(mon);
            wci.width  = std::min(mode->width - 40, 1600);
            wci.height = std::min(mode->height - 80, 900);
            ARX_LOG_INFO("Monitor: {}x{}, ventana: {}x{}", mode->width, mode->height, wci.width, wci.height);
        } else {
            wci.width  = 1280;
            wci.height = 720;
        }
    }
    wci.gl_major = 2;
    wci.gl_minor = 1;
    wci.gl_compat = true;
    auto window = os->create_window(wci);
    if (!window) {
        ARX_LOG_FATAL("No se pudo crear la ventana del editor");
        return false;
    }
    window->set_vsync(true);

    auto renderer = Renderer::create(Renderer::Backend::OpenGL);
    RendererConfig rc;
    rc.viewport_w = wci.width;
    rc.viewport_h = wci.height;
    if (!renderer->init(rc)) {
        ARX_LOG_FATAL("No se pudo inicializar el renderer");
        return false;
    }

    register_module_types();

    EditorMain editor;
    if (!editor.init(window.get(), renderer.get())) {
        ARX_LOG_FATAL("No se pudo inicializar el editor");
        return false;
    }
    editor.set_window(window.get());
    
    // Cargar proyecto si se especificó
    if (!project_path.empty()) {
        editor.load_project(project_path);
    }

    os->set_main_loop(&editor);
    os->run();

    bool wants_pm = editor.wants_return_to_pm();
    editor.finish();
    renderer->shutdown();
    return wants_pm;
}

int main(int argc, char** argv) {
    using namespace arx;

    Logger::instance().set_min_level(LogLevel::Debug);
    Logger::instance().set_log_file_path("/tmp/arx_editor.log");
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    ARX_LOG_INFO("=== ARX Engine {} ===", ARX_VERSION);

    // Parse args: buscar --project o path como primer arg
    std::string project_path;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--project" && i + 1 < argc) {
            project_path = argv[++i];
        } else if (arg[0] != '-') {
            // Primer arg sin --, asumir que es path a proyecto
            project_path = arg;
        }
    }

    // Si el path es un archivo .arx (ej: project.arx), usar la carpeta que lo contiene
    if (!project_path.empty()) {
        if (fs::is_regular_file(project_path)) {
            project_path = fs::path(project_path).parent_path().string();
            ARX_LOG_INFO("Path era archivo, usando carpeta: {}", project_path);
        }
        // Verificar que exista project.arx en la carpeta
        if (!fs::exists(fs::path(project_path) / "project.arx")) {
            ARX_LOG_ERROR("No se encontró project.arx en: {}", project_path);
            project_path.clear();  // abrir PM en vez de crashear
        }
    }

    // Loop principal: PM → Editor → (si editor cierra) terminar
    // Si el editor pide volver al PM (Close Project), reabre el PM.
    while (true) {
        auto os = std::unique_ptr<OS>(OS::create());
        if (!os || !os->init()) {
            std::cerr << "No se pudo inicializar el OS.\n";
            return 1;
        }

        if (project_path.empty()) {
            ARX_LOG_INFO("Modo: Project Manager (sin proyecto)");
            run_project_manager(os.get());
            // Si el PM seleccionó un proyecto, cambiar a modo editor
            if (!g_selected_project.empty()) {
                project_path = g_selected_project;
                g_selected_project.clear();
                os->shutdown();
                continue;  // volver a loop, ahora con proyecto
            }
            break;  // PM cerró sin seleccionar
        } else {
            ARX_LOG_INFO("Modo: Editor con proyecto '{}'", project_path);
            bool wants_pm = run_editor(os.get(), project_path);
            os->shutdown();
            if (wants_pm) {
                // El editor pidió volver al PM
                project_path.clear();
                continue;  // volver a loop, ahora en modo PM
            }
            break;  // editor cerró sin pedir PM, terminar
        }
    }

    return 0;
}
