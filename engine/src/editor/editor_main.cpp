// ==============================================================================
// src/editor/editor_main.cpp
// ==============================================================================
#include "editor_main.hpp"
#include "gui/editor_dock_manager.hpp"
#include "gui/scene_tree_dock.hpp"
#include "gui/inspector_dock.hpp"
#include "gui/viewport.hpp"
#include "gui/filesystem_dock.hpp"
#include "gui/code_editor_dock.hpp"
#include "gui/theme_editor.hpp"
#include "gui/project_settings.hpp"
#include "gui/icon_manager.hpp"  // SVG icon system (nanosvg)
#include "gui/project_manager.hpp"
#include "scene/scene_tree.hpp"
#include "scene/vox.hpp"
#include "scene/3d/vox3d.hpp"
#include "scene/3d/vox_mesh_instance_3d.hpp"
#include "core/logging.hpp"

// IMPORTANTE: ImGui v1.91+ requiere estos defines ANTES de incluir imgui.h:
//   - IMGUI_DEFINE_MATH_OPERATORS: habilita operator+ etc. en ImVec2 (necesario
//     para que imgui_internal.h compile correctamente)
//   - IMGUI_ENABLE_DOCKING: habilita TODO el módulo de Docking. Sin esto,
//     las funciones DockSpace, DockBuilder*, ImGuiConfigFlags_DockingEnable,
//     ImGuiCol_DockingPreview, ImGuiCol_DockingEmptyBg, ImGuiDockNodeFlags_*,
//     ImGuiWindowFlags_NoDocking, etc. NO se declaran en imgui.h y da error
//     de compilación "was not declared in this scope".
// DockBuilder* está en imgui_internal.h (API interna pero estándar para editores).
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_ENABLE_DOCKING
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>
#include <glad/glad.h>   // debe ir ANTES de GLFW para que glad defina las funciones OpenGL
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <cstdio>
#include <filesystem>
#include <fstream>

#ifdef ARX_ZEN_ENABLED
#include "zen_vm.hpp"
#endif

namespace arx {

EditorMain::EditorMain()  = default;
EditorMain::~EditorMain() = default;

// Puente C para que el callback del viewport (que es C) pueda seleccionar
// un Vox en el SceneTreeDock sin capturar punteros C++.
static arx::SceneTreeDock* g_scene_tree_dock_for_cb = nullptr;
static arx::CodeEditorDock* g_code_editor_dock_for_cb = nullptr;
static bool show_close_confirm_dialog = false;
void arx_set_scene_tree_selected(arx::Vox* v) {
    if (g_scene_tree_dock_for_cb) g_scene_tree_dock_for_cb->set_selected(v);
}
void arx_open_code_file(const std::string& path) {
    if (g_code_editor_dock_for_cb) g_code_editor_dock_for_cb->load_file(path);
}

bool EditorMain::init(Window* win, Renderer* r) {
    window_  = win;
    renderer_ = r;
    setup_imgui_();

    docks_       = std::make_unique<EditorDockManager>();
    project_mgr_ = std::make_unique<ProjectManager>();

    docks_->init(renderer_);

    // Garantizar que el SceneTreeDock tenga un root desde el principio.
    if (docks_->scene_tree_dock()) {
        g_scene_tree_dock_for_cb = docks_->scene_tree_dock();
        if (docks_->code_editor_dock()) g_code_editor_dock_for_cb = docks_->code_editor_dock();
        docks_->scene_tree_dock()->ensure_root();
        // Inicializar el sistema de iconos SVG (carga bajo demanda + hot reload)
        IconManager::get().init("/home/aethravox/Escritorio/Projectos/Motores/ARX/engine/src/editor/icons/voxes", 24);
        ARX_LOG_INFO("EditorMain: IconManager inicializado");
        ARX_LOG_INFO("EditorMain: root por defecto creado en SceneTreeDock");
    }
    ARX_LOG_INFO("EditorMain inicializado");
    return true;
}

void EditorMain::start() {
    ARX_LOG_INFO("EditorMain::start — inicializando con splash");
    splash_done_ = false;
    splash_time_ = 0.0f;
    // Inicializar ImGui inmediatamente para poder dibujar el splash.
    if (!imgui_ready_) {
        setup_imgui_();
    }
}

bool EditorMain::process(float delta) {
    // 1. Poll events.
    window_->poll_events();

    // 2. Splash (2 segundos) o Main UI.
    if (!splash_done_) {
        splash_time_ += delta;
        render_splash_(splash_time_);
        window_->swap_buffers();
        if (splash_time_ >= 2.0f) {
            splash_done_ = true;
            ARX_LOG_INFO("Splash terminado, cargando editor");
        }
        return !window_->should_close();
    }

    // 3. Main UI (ImGui).
    render_main_ui_(delta);

    // Hot reload: verificar si main.zen cambió
    check_hot_reload_();

    // 3. Swap.
    window_->swap_buffers();

    stats_.delta_seconds = delta;
    stats_.fps = 1.0f / std::max(delta, 1e-6f);
    stats_.frame_usec = static_cast<uint64_t>(delta * 1e6);

    // Si se solicitó volver al PM, retornar false para cerrar el editor
    if (return_to_pm_) {
        return false;
    }
    return !window_->should_close();
}

void EditorMain::load_project(const std::string& path) {
    ARX_LOG_INFO("EditorMain::load_project — cargando '{}'", path);
    loaded_project_path_ = path;
    if (project_mgr_) {
        project_mgr_->set_current_project(path);
    }
    // Setear el directorio del proyecto en el FileSystem dock
    if (docks_ && docks_->filesystem_dock()) {
        docks_->filesystem_dock()->set_project_dir(path);
        // Conectar callback: doble-click en .zen abre el Code Editor
        docks_->filesystem_dock()->on_open_zen_file = [](const std::string& path) {
            arx_open_code_file(path);
        };
    if (docks_ && docks_->code_editor_dock()) {
        docks_->code_editor_dock()->set_project_dir(path);
    }
    }
#ifdef ARX_ZEN_ENABLED
    // Inicializar VM
    vm_ = std::make_unique<zen::VM>();
    // Verificar si existe main.zen
    auto main_zen = std::filesystem::path(path) / "main.zen";
    main_zen_exists_ = std::filesystem::exists(main_zen);
    if (main_zen_exists_) {
        main_zen_last_write_ = std::filesystem::last_write_time(main_zen);
    }
    ARX_LOG_INFO("EditorMain: main.zen {}", main_zen_exists_ ? "encontrado" : "no encontrado");
#endif

    // FASE 14: Auto-cargar archivos .glb del proyecto al inicio
    if (docks_ && docks_->scene_tree_dock()) {
        Vox* root = docks_->scene_tree_dock()->get_root();
        if (root) {
            namespace fs = std::filesystem;
            try {
                for (const auto& e : fs::directory_iterator(path)) {
                    if (!e.is_regular_file()) continue;
                    std::string ext = e.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    if (ext == ".glb") {
                        auto* mesh_vox = new VoxMeshInstance3D();
                        mesh_vox->set_name(e.path().stem().string());
                        mesh_vox->set_position(Vector3(0, 1, 0));
                        if (mesh_vox->load_glb(e.path().string())) {
                            root->add_child(mesh_vox);
                            ARX_LOG_INFO("EditorMain: auto-cargado GLB '{}'", e.path().filename().string());
                        } else {
                            delete mesh_vox;
                            ARX_LOG_WARN("EditorMain: no se pudo cargar GLB '{}'", e.path().filename().string());
                        }
                    }
                }
            } catch (...) {}
        }
    }
}

void EditorMain::finish() {
    IconManager::get().shutdown();
    stop_zen_vm_();
    shutdown_imgui_();
    ARX_LOG_INFO("EditorMain::finish");
}

void EditorMain::run_zen_vm_() {
#ifdef ARX_ZEN_ENABLED
    if (!vm_ || loaded_project_path_.empty()) return;

    auto main_zen = std::filesystem::path(loaded_project_path_) / "main.zen";
    if (!std::filesystem::exists(main_zen)) {
        ARX_LOG_ERROR("No se encontró main.zen en {}", loaded_project_path_);
        return;
    }

    ARX_LOG_INFO("EditorMain: ejecutando main.zen con VM...");
    vm_->clear_output();
    if (vm_->load_file(main_zen.string())) {
        vm_->run();
        vm_running_ = true;
        // Actualizar timestamp
        main_zen_last_write_ = std::filesystem::last_write_time(main_zen);
        ARX_LOG_INFO("EditorMain: VM ejecutada, {} líneas de output", vm_->output().size());
    } else {
        ARX_LOG_ERROR("EditorMain: error cargando main.zen");
        for (auto& e : vm_->errors()) ARX_LOG_ERROR("  {}", e);
    }
#else
    ARX_LOG_WARN("Zen VM no disponible (ARX_ZEN_ENABLED no definido)");
#endif
}

void EditorMain::stop_zen_vm_() {
    vm_running_ = false;
}

void EditorMain::check_hot_reload_() {
#ifdef ARX_ZEN_ENABLED
    if (!vm_ || loaded_project_path_.empty() || !main_zen_exists_) return;

    auto main_zen = std::filesystem::path(loaded_project_path_) / "main.zen";
    if (!std::filesystem::exists(main_zen)) return;

    auto current_write = std::filesystem::last_write_time(main_zen);
    if (current_write != main_zen_last_write_) {
        ARX_LOG_INFO("EditorMain: main.zen modificado — hot reload!");
        main_zen_last_write_ = current_write;
        // Re-ejecutar
        run_zen_vm_();
    }
#endif
}

void EditorMain::render_console_panel_() {
    if (!ImGui::Begin("Console")) {
        ImGui::End();
        return;
    }

    // Toolbar
    if (ImGui::Button(vm_running_ ? "Stop" : "Run", ImVec2(60, 24))) {
        if (vm_running_) {
            stop_zen_vm_();
        } else {
            run_zen_vm_();
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear", ImVec2(60, 24))) {
#ifdef ARX_ZEN_ENABLED
        if (vm_) vm_->clear_output();
#endif
    }
    ImGui::SameLine();
    ImGui::TextDisabled("F5=Run  F8=Stop");

    ImGui::Separator();

    // Output area
    ImGui::BeginChild("console_output", ImVec2(0, 0), true);
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 2 ? ImGui::GetIO().Fonts->Fonts[2] : nullptr);

#ifdef ARX_ZEN_ENABLED
    if (vm_) {
        if (vm_->has_errors()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            for (auto& e : vm_->errors()) {
                ImGui::TextUnformatted(e.c_str());
            }
            ImGui::PopStyleColor();
        }
        for (auto& line : vm_->output()) {
            ImGui::TextUnformatted(line.c_str());
        }
        if (vm_->output().empty() && !vm_->has_errors() && !vm_running_) {
            ImGui::TextDisabled("Press 'Run' (F5) to execute main.zen");
        }
    } else {
        ImGui::TextDisabled("No project loaded.");
    }
#else
    ImGui::TextDisabled("Zen VM not available (ARX_ZEN_ENABLED not defined).");
#endif

    // Auto-scroll al final
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::PopFont();
    ImGui::EndChild();
    ImGui::End();
}

// ===================== Splash ================================================
void EditorMain::render_splash_(float t) {
    // Cargar splash.png (una sola vez)
    static bool loaded = false;
    static unsigned int tex = 0;
    static int w = 0, h = 0;
    if (!loaded) {
        int c = 0;
        unsigned char* data = stbi_load(ARX_SPLASH_PATH, &w, &h, &c, 4);
        if (data) {
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            // OpenGL 2.1: GL_GENERATE_MIPMAP en vez de glGenerateMipmap
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            stbi_image_free(data);
            loaded = true;
            ARX_LOG_INFO("Splash cargado: {}x{}", w, h);
        } else {
            ARX_LOG_WARN("No se pudo cargar splash: {}", ARX_SPLASH_PATH);
            loaded = true;  // no reintentar
        }
    }

    // Frame de ImGui para el splash
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Limpiar pantalla con color de fondo oscuro
    auto* win = static_cast<GLFWwindow*>(window_->native_handle());
    int ww, wh;
    glfwGetWindowSize(win, &ww, &wh);

    // Ventana fullscreen sin decoración
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(ww, wh));
    ImGui::Begin("###Splash", nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground);

    // Calcular fade: fade-in 0-0.3s, mostrar 0.3-1.7s, fade-out 1.7-2.0s
    float alpha = 1.0f;
    if (t < 0.3f) alpha = t / 0.3f;
    else if (t > 1.7f) alpha = (2.0f - t) / 0.3f;
    alpha = std::max(0.0f, std::min(1.0f, alpha));

    // Centrar la imagen del splash
    if (tex && w > 0 && h > 0) {
        ImVec2 img_size(static_cast<float>(w), static_cast<float>(h));
        ImVec2 pos((ww - img_size.x) * 0.5f, (wh - img_size.y) * 0.5f);
        ImGui::SetCursorPos(pos);
        ImGui::Image(static_cast<ImTextureID>((intptr_t)tex), img_size,
                     ImVec2(0, 0), ImVec2(1, 1),
                     ImVec4(1, 1, 1, alpha), ImVec4(0, 0, 0, 0));
    } else {
        // Si no hay imagen, mostrar texto
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
        const char* text = "ARX Engine";
        ImVec2 text_size = ImGui::CalcTextSize(text);
        ImGui::SetCursorPos(ImVec2((ww - text_size.x) * 0.5f, (wh - text_size.y) * 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.92f, alpha));
        ImGui::TextUnformatted(text);
        ImGui::PopStyleColor();

        const char* sub = "Loading...";
        ImVec2 sub_size = ImGui::CalcTextSize(sub);
        ImGui::SetCursorPos(ImVec2((ww - sub_size.x) * 0.5f, (wh - sub_size.y) * 0.5f + 30));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.55f, alpha));
        ImGui::TextUnformatted(sub);
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    ImGui::End();

    // Render
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    int fbw, fbh;
    glfwGetFramebufferSize(win, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset GL state para ImGui (igual que en render_main_ui_)
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    for (int i = 0; i < 4; i++) {
        glDisableVertexAttribArray(i);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    ImGui_ImplOpenGL2_RenderDrawData(draw_data);
    glFlush();
}

// ===================== Main UI ===============================================
void EditorMain::render_main_ui_(float delta) {
    // ImGui frame.
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Hotkeys: F1=Docs, F5=Run, F8=Stop (usar GLFW directamente)
    auto* win_f5 = static_cast<GLFWwindow*>(window_->native_handle());
    static bool f1_prev = false, f5_prev = false, f8_prev = false;
    bool f1_now = glfwGetKey(win_f5, GLFW_KEY_F1) == GLFW_PRESS;
    bool f5_now = glfwGetKey(win_f5, GLFW_KEY_F5) == GLFW_PRESS;
    bool f8_now = glfwGetKey(win_f5, GLFW_KEY_F8) == GLFW_PRESS;
    bool f4_now = glfwGetKey(win_f5, GLFW_KEY_F4) == GLFW_PRESS;
    static bool f4_prev = false;
    if (f4_now && !f4_prev) {
        ImGui::SetWindowFocus("Code Editor");
    }
    f4_prev = f4_now;
    if (f1_now && !f1_prev) { show_documentation_ = !show_documentation_; }
    if (f5_now && !f5_prev) { run_zen_vm_(); }
    if (f8_now && !f8_prev) { stop_zen_vm_(); }
    f1_prev = f1_now;
    f5_prev = f5_now;
    f8_prev = f8_now;

    // Debug logging deshabilitado (ya verificado que funciona)

    // ===== Demo window deshabilitada (debug completado) =====
    // static bool show_demo = true;
    // if (show_demo) { ImGui::ShowDemoWindow(&show_demo); }

    // Welcome panel (siempre visible hasta que se abra un proyecto)
    if (!project_mgr_->has_open_project()) {
        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                                  ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("Welcome to ARX Engine", nullptr,
                          ImGuiWindowFlags_NoCollapse)) {
            ImGui::TextDisabled("ARX Engine %s", ARX_VERSION);
            ImGui::Separator();
            ImGui::TextWrapped("Bienvenido al editor de ARX Engine.");
            ImGui::TextWrapped("Para empezar, abre o crea un proyecto desde el menú File.");
            ImGui::Separator();
            if (ImGui::Button("New Project")) {
                project_mgr_->show_new_dialog();
            }
            ImGui::SameLine();
            if (ImGui::Button("Open Project...")) {
                // Usar zenity para seleccionar carpeta de proyecto
                std::string cmd = "zenity --file-selection --directory --title='Select ARX project folder' 2>/dev/null";
                FILE* pipe = popen(cmd.c_str(), "r");
                if (pipe) {
                    char buffer[1024];
                    std::string result;
                    while (fgets(buffer, sizeof(buffer), pipe)) result += buffer;
                    pclose(pipe);
                    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
                        result.pop_back();
                    }
                    if (!result.empty()) {
                        // Verificar que tenga project.arx
                        namespace fs = std::filesystem;
                        if (fs::exists(fs::path(result) / "project.arx")) {
                            load_project(result);
                        } else {
                            ARX_LOG_ERROR("No es un proyecto ARX válido (falta project.arx): {}", result);
                        }
                    }
                }
            }
            ImGui::Separator();
        }
        ImGui::End();
    }

    // Dockspace principal (estilo Godot).
    auto* win = static_cast<GLFWwindow*>(window_->native_handle());
    int ww, wh; glfwGetWindowSize(win, &ww, &wh);

    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGuiWindowFlags dock_flags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("###EditorDockSpace", nullptr, dock_flags);
    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("ARXEditorDockSpace");
    // Forzar layout solo la primera vez (flag estático)
    static bool layout_initialized = false;
    if (!layout_initialized) {
        layout_initialized = true;
        // Layout inicial estilo Godot:
        // ┌────────────┬──────────────────────────┐
        // │ FileSystem │     Viewport             │
        // │ SceneTree  │                          │  Inspector
        // │            │                          │
        // ├────────────┴──────────────────────────┤
        // │ Output / Console                       │
        // └────────────────────────────────────────┘
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id,
            ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, vp->WorkSize);

        ImGuiID main = dockspace_id;
        ImGuiID bottom, top;
        ImGui::DockBuilderSplitNode(main, ImGuiDir_Down, 0.25f, &bottom, &top);

        // Split top en left (25%), center (50%), right (25%)
        ImGuiID left, center, right;
        ImGui::DockBuilderSplitNode(top, ImGuiDir_Left,  0.20f, &left, &center);
        ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, &right, &center);

        // Split left en vox_tree (top, 50%) y filesystem (bottom, 50%)
        // Así NO se tabulan, sino que cada uno tiene su propia área
        ImGuiID vox_tree_node, filesystem_node;
        ImGui::DockBuilderSplitNode(left, ImGuiDir_Down, 0.5f, &filesystem_node, &vox_tree_node);

        ImGui::DockBuilderDockWindow("Vox Tree",    vox_tree_node);
        ImGui::DockBuilderDockWindow("FileSystem",  filesystem_node);
        ImGui::DockBuilderDockWindow("Viewport",    center);
        ImGui::DockBuilderDockWindow("Code Editor", center);  // Tabbed con Viewport
        ImGui::DockBuilderDockWindow("Inspector",   right);
        ImGui::DockBuilderDockWindow("Output",      bottom);
        ImGui::DockBuilderDockWindow("Console",     bottom);
        // Quitar NoTabBar de los docks izquierdo, derecho e inferior
        // para que se vean los tabs (Vox Tree | FileSystem, Output | Console, etc)
        if (ImGuiDockNode* left_node = ImGui::DockBuilderGetNode(left)) {
            left_node->LocalFlags &= ~ImGuiDockNodeFlags_NoTabBar;
        }
        if (ImGuiDockNode* right_node = ImGui::DockBuilderGetNode(right)) {
            right_node->LocalFlags &= ~ImGuiDockNodeFlags_NoTabBar;
        }
        if (ImGuiDockNode* bottom_node = ImGui::DockBuilderGetNode(bottom)) {
            bottom_node->LocalFlags &= ~ImGuiDockNodeFlags_NoTabBar;
        }
        ImGui::DockBuilderFinish(dockspace_id);
    }
    ImGui::DockSpace(dockspace_id, ImVec2(0,0),
                     ImGuiDockNodeFlags_PassthruCentralNode);

    // Forzar que el bottom dock muestre tabs (para ver Output | Console | Code Editor)
    // Esto se ejecuta cada frame para superar el .ini que podría tener NoTabBar guardado.
    if (ImGuiDockNode* root = ImGui::DockBuilderGetNode(dockspace_id)) {
        // Recorrer todos los nodos y quitar NoTabBar
        std::function<void(ImGuiDockNode*)> clear_notabbar = [&](ImGuiDockNode* node) {
            if (!node) return;
            node->LocalFlags &= ~ImGuiDockNodeFlags_NoTabBar;
            if (node->ChildNodes[0]) clear_notabbar(node->ChildNodes[0]);
            if (node->ChildNodes[1]) clear_notabbar(node->ChildNodes[1]);
        };
        clear_notabbar(root);
    }

    ImGui::End();

    // Menu bar superior.
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Project"))  {
                // Volver al PM en modo "new project wizard"
                return_to_pm_ = true;
            }
            if (ImGui::MenuItem("Open Project...")) {
                // Usar zenity para seleccionar carpeta de proyecto
                std::string cmd = "zenity --file-selection --directory --title='Select ARX project folder' 2>/dev/null";
                FILE* pipe = popen(cmd.c_str(), "r");
                if (pipe) {
                    char buffer[1024];
                    std::string result;
                    while (fgets(buffer, sizeof(buffer), pipe)) result += buffer;
                    pclose(pipe);
                    while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
                        result.pop_back();
                    }
                    if (!result.empty()) {
                        namespace fs = std::filesystem;
                        if (fs::exists(fs::path(result) / "project.arx")) {
                            load_project(result);
                        } else {
                            ARX_LOG_ERROR("No es un proyecto ARX válido (falta project.arx): {}", result);
                        }
                    }
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))   {}
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                show_close_confirm_dialog = true;
            }
            if (ImGui::MenuItem("Save As..."))   {}
            ImGui::Separator();
            if (ImGui::BeginMenu("Export")) {
                if (ImGui::MenuItem("ARX Package (.aex)...")) {
                    export_aex_dialog_.open("");
                }
                if (ImGui::MenuItem("Linux x64 (native)"))  {} // TODO: LinuxExporter
                if (ImGui::MenuItem("Windows (native)"))    {} // TODO: WindowsExporter
                if (ImGui::MenuItem("Web (HTML5)"))         {} // TODO: WebExporter
                if (ImGui::MenuItem("Android (APK)"))       {} // TODO: AndroidExporter
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close Project", "Ctrl+Shift+W")) {
                // Volver al Project Manager (sin proyecto)
                return_to_pm_ = true;
            }
            if (ImGui::MenuItem("Quit", "Alt+F4")) {
                // Cerrar la app completa (no volver al PM)
                if (auto* w = window_) glfwSetWindowShouldClose(
                    static_cast<GLFWwindow*>(w->native_handle()), GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Run", "F5"))    { run_zen_vm_(); }
            if (ImGui::MenuItem("Pause","F7"))   {}
            if (ImGui::MenuItem("Stop", "F8"))   { stop_zen_vm_(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Editor")) {
            if (ImGui::MenuItem("Project Settings...")) {
                // TODO: abrir dialog de Project Settings
            }
            if (ImGui::MenuItem("Editor Settings..."))  {
                // TODO: abrir dialog de Editor Settings
            }
            ImGui::Separator();
            if (ImGui::MenuItem("About ARX Engine"))    { show_about_ = true; }
                        ImGui::Separator();
            if (ImGui::MenuItem("Theme Editor...")) { show_theme_editor_ = true; }
ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Window")) {
            if (ImGui::MenuItem("Focus Code Editor", "F4")) {
                ImGui::SetWindowFocus("Code Editor");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("Documentation", "F1"))     { show_documentation_ = true; }
            if (ImGui::MenuItem("Zen Language Reference"))  { show_documentation_ = true; }
            ImGui::Separator();
            if (ImGui::MenuItem("Report a Bug"))      {
                system("xdg-open https://github.com/Aethravox/arx-engine/issues 2>/dev/null &");
            }
            if (ImGui::MenuItem("Website"))           {
                system("xdg-open https://aethravox.com 2>/dev/null &");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Export .aex dialog (modal).
    if (export_aex_dialog_.is_open()) {
        export_aex_dialog_.render();
    }

    // Conectar selección del Vox Tree al Inspector y Viewport
    if (docks_ && docks_->scene_tree_dock()) {
        Vox* selected = docks_->scene_tree_dock()->get_selected();
        // Validar que el Vox seleccionado sigue siendo parte del arbol.
        // Si fue borrado (puntero colgante), pasar nullptr al inspector.
        if (selected && !docks_->scene_tree_dock()->is_vox_in_tree(selected)) {
            docks_->scene_tree_dock()->set_selected(nullptr);
            selected = nullptr;
        }
        if (docks_->inspector_dock()) {
            docks_->inspector_dock()->inspect(selected);
        }
        if (docks_->viewport_dock()) {
            docks_->viewport_dock()->set_selected(selected);
            docks_->viewport_dock()->set_root(docks_->scene_tree_dock()->get_root());
        }
    }

    // Conectar validators: Inspector y Viewport validan que su target siga vivo
    static bool validators_init = false;
    if (!validators_init && docks_ && docks_->scene_tree_dock()) {
        auto* stdock = docks_->scene_tree_dock();
        if (docks_->inspector_dock()) {
            docks_->inspector_dock()->is_vox_valid_ = [stdock](Vox* v) {
                return stdock->is_vox_in_tree(v);
            };
        }
        if (docks_->viewport_dock()) {
            docks_->viewport_dock()->is_vox_valid_ = [stdock](Vox* v) {
                return stdock->is_vox_in_tree(v);
            };
        }
        validators_init = true;
    }

    // Conectar callback: cuando el viewport selecciona un Vox (drop/click),
    // propagar al SceneTreeDock para que el Inspector lo muestre.
    static bool cb_init = false;
    if (!cb_init && docks_ && docks_->viewport_dock() && docks_->scene_tree_dock()) {
        docks_->viewport_dock()->set_on_selected(
            [](Vox* v) {
                // Llamar a la funcion puente definida arriba.
                arx_set_scene_tree_selected(v);
            });
        cb_init = true;
    }

    // Docks (cada uno dibuja su ventana).
    docks_->render_all(delta);

    // Project Manager (modal si está abierto).
    project_mgr_->render();

    // Console panel (VM output)
    render_console_panel_();

    // ===== Close confirmation dialog =====
    if (show_close_confirm_dialog) {
        ImGui::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("Cerrar ARX Engine?", &show_close_confirm_dialog,
                         ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse)) {
            ImGui::TextWrapped("Hay cambios sin guardar. Seguro que queres salir?");
            ImGui::Separator();
            if (ImGui::Button("Salir sin guardar", ImVec2(150, 0))) {
                show_close_confirm_dialog = false;
                // Forzar cierre
                // Close confirmed - just return false from process() to end the loop
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancelar", ImVec2(120, 0))) {
                show_close_confirm_dialog = false;
            }
        }
        ImGui::End();
    }

    // ===== About dialog =====
    if (show_about_) {
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("About ARX Engine", &show_about_,
                         ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse)) {
            // Logo
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
            ImGui::Text("ARX Engine");
            ImGui::PopFont();
            ImGui::TextDisabled("Version %s", ARX_VERSION);
            ImGui::Separator();

            ImGui::TextWrapped("Motor de apps y juegos nativo C++ con Zen Lang.");
            ImGui::TextWrapped("Powered by Aethravox Studios.");
            ImGui::Separator();

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
            ImGui::Text("Tecnologias");
            ImGui::PopFont();
            ImGui::BulletText("Lenguaje: Zen (bilingue ES/EN, LLVM AOT + VM)");
            ImGui::BulletText("Renderer: OpenGL 2.1+ / GLES 3.0");
            ImGui::BulletText("Fisica: Bullet + Jolt (dual)");
            ImGui::BulletText("UI: Dear ImGui con docking");
            ImGui::BulletText("Cripto: Ed25519 + SHA-256");
            ImGui::BulletText("Audio: miniaudio");
            ImGui::BulletText("Build: CMake 3.20+");
            ImGui::Separator();

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
            ImGui::Text("Formato .aex");
            ImGui::PopFont();
            ImGui::BulletText("Paquete firmado criptograficamente");
            ImGui::BulletText("Sandbox con permisos explicitos");
            ImGui::BulletText("3 modos de export (.aex / standalone puro / motorizado)");
            ImGui::BulletText("Lazy loading (multiples .aex)");
            ImGui::Separator();

            ImGui::TextDisabled("Licencia: MIT");
            ImGui::TextDisabled("100%% codigo original - 0%% copy-paste");
            ImGui::Separator();

            if (ImGui::Button("Cerrar", ImVec2(100, 0))) show_about_ = false;
            ImGui::SameLine();
            if (ImGui::Button("Website")) {
                system("xdg-open https://aethravox.com 2>/dev/null &");
            }
        }
        ImGui::End();
    }

    // ===== Documentation dialog =====
    if (show_documentation_) {
        ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("Zen Language Reference", &show_documentation_,
                         ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse)) {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
            ImGui::Text("Zen Lang v1.2 — Referencia");
            ImGui::PopFont();
            ImGui::TextDisabled("Bilingue ES/EN | Sin tipos | Sin punto y coma");
            ImGui::Separator();

            if (ImGui::BeginTabBar("ZenDoc")) {
                // Tab: Basics
                if (ImGui::BeginTabItem("Basico")) {
                    ImGui::TextWrapped("Zen es un lenguaje bilingue (espanol/ingles) sin tipos, sin punto y coma, con indentacion.");
                    ImGui::Separator();
                    ImGui::TextDisabled("Imprimir:");
                    ImGui::Text("  muestra \"Hola mundo!\"");
                    ImGui::SameLine(); ImGui::TextDisabled("# o: show / print");
                    ImGui::TextDisabled("Variables:");
                    ImGui::Text("  x = 10");
                    ImGui::Text("  nombre = \"Ana\"");
                    ImGui::Text("  activo = verdad");
                    ImGui::SameLine(); ImGui::TextDisabled("# o: yes / true");
                    ImGui::TextDisabled("F-strings:");
                    ImGui::Text("  muestra f\"Hola {nombre}, tienes {edad} anos\"");
                    ImGui::TextDisabled("Comentarios:");
                    ImGui::Text("  # esto es un comentario");
                    ImGui::EndTabItem();
                }
                // Tab: Control flow
                if (ImGui::BeginTabItem("Control de flujo")) {
                    ImGui::TextDisabled("Condicional:");
                    ImGui::Text("  si x > 5");
                    ImGui::Text("      muestra \"grande\"");
                    ImGui::Text("  sino");
                    ImGui::Text("      muestra \"chiquito\"");
                    ImGui::SameLine(); ImGui::TextDisabled("# si/if, sino/else");
                    ImGui::Separator();
                    ImGui::TextDisabled("Bucle para (for range):");
                    ImGui::Text("  para i desde 1 hasta 10");
                    ImGui::Text("      muestra i");
                    ImGui::SameLine(); ImGui::TextDisabled("# para/for, desde/from, hasta/to");
                    ImGui::Separator();
                    ImGui::TextDisabled("Bucle para cada (for each):");
                    ImGui::Text("  para cada n en lista");
                    ImGui::Text("      muestra n");
                    ImGui::Separator();
                    ImGui::TextDisabled("Mientras (while):");
                    ImGui::Text("  mientras x < 100");
                    ImGui::Text("      x += 10");
                    ImGui::Separator();
                    ImGui::TextDisabled("Repetir (repeat):");
                    ImGui::Text("  repetir 5 veces");
                    ImGui::Text("      muestra \"Zen!\"");
                    ImGui::Separator();
                    ImGui::TextDisabled("Break / Continue:");
                    ImGui::Text("  romper     # o: break");
                    ImGui::Text("  continuar  # o: continue");
                    ImGui::EndTabItem();
                }
                // Tab: Funciones
                if (ImGui::BeginTabItem("Funciones")) {
                    ImGui::TextDisabled("Definir funcion:");
                    ImGui::Text("  funcion saludar(nombre)");
                    ImGui::Text("      muestra f\"Hola {nombre}!\"");
                    ImGui::SameLine(); ImGui::TextDisabled("# o: function");
                    ImGui::Separator();
                    ImGui::TextDisabled("Retornar valor:");
                    ImGui::Text("  funcion sumar(a, b)");
                    ImGui::Text("      retorna a + b");
                    ImGui::SameLine(); ImGui::TextDisabled("# o: return");
                    ImGui::Separator();
                    ImGui::TextDisabled("Llamar funcion:");
                    ImGui::Text("  resultado = sumar(10, 20)");
                    ImGui::Text("  muestra resultado  # 30");
                    ImGui::EndTabItem();
                }
                // Tab: Structs
                if (ImGui::BeginTabItem("Structs")) {
                    ImGui::TextDisabled("Declarar struct (sintaxis nueva):");
                    ImGui::Text("  estructura Persona:");
                    ImGui::Text("      nombre");
                    ImGui::Text("      edad");
                    ImGui::Text("      ciudad");
                    ImGui::SameLine(); ImGui::TextDisabled("# o: struct");
                    ImGui::Separator();
                    ImGui::TextDisabled("Crear instancia:");
                    ImGui::Text("  ana = Persona {");
                    ImGui::Text("      nombre: \"Ana\",");
                    ImGui::Text("      edad: 25,");
                    ImGui::Text("      ciudad: \"Madrid\"");
                    ImGui::Text("  }");
                    ImGui::Separator();
                    ImGui::TextDisabled("Acceder y modificar:");
                    ImGui::Text("  muestra ana.nombre");
                    ImGui::Text("  ana.edad = 26");
                    ImGui::Separator();
                    ImGui::TextDisabled("Tambien sintaxis vieja (una linea):");
                    ImGui::Text("  Punto {x, y}");
                    ImGui::EndTabItem();
                }
                // Tab: Listas
                if (ImGui::BeginTabItem("Listas")) {
                    ImGui::TextDisabled("Crear lista:");
                    ImGui::Text("  nums = [1, 2, 3, 4, 5]");
                    ImGui::Text("  nombres = [\"Ana\", \"Bob\", \"Carlos\"]");
                    ImGui::Separator();
                    ImGui::TextDisabled("Acceder:");
                    ImGui::Text("  muestra nums[0]  # 1");
                    ImGui::Text("  nums[0] = 99");
                    ImGui::Separator();
                    ImGui::TextDisabled("Builtins:");
                    ImGui::Text("  longitud(nums)     # 5");
                    ImGui::Text("  contiene(nums, 3)  # verdad");
                    ImGui::Text("  ultimo = quitar(nums)  # pop");
                    ImGui::Separator();
                    ImGui::TextDisabled("Iterar:");
                    ImGui::Text("  para cada n en nums");
                    ImGui::Text("      muestra n");
                    ImGui::EndTabItem();
                }
                // Tab: FFI
                if (ImGui::BeginTabItem("FFI")) {
                    ImGui::TextDisabled("Declarar funcion externa:");
                    ImGui::Text("  extern \"raylib\" InitWindow(ancho: int, alto: int, titulo: str) -> void");
                    ImGui::Separator();
                    ImGui::TextDisabled("Llamar:");
                    ImGui::Text("  InitWindow(800, 600, \"Mi App\")");
                    ImGui::Separator();
                    ImGui::TextWrapped("FFI permite llamar funciones de librerias C desde Zen. La libreria se linkea al exportar.");
                    ImGui::EndTabItem();
                }
                // Tab: Builtins
                if (ImGui::BeginTabItem("Builtins")) {
                    ImGui::TextDisabled("Print:");
                    ImGui::Text("  muestra(x)   / show(x)   / print(x)");
                    ImGui::Separator();
                    ImGui::TextDisabled("Conversion:");
                    ImGui::Text("  numero(\"42\")   # string -> numero");
                    ImGui::Text("  texto(42)      # numero -> string");
                    ImGui::Separator();
                    ImGui::TextDisabled("Listas:");
                    ImGui::Text("  longitud(x)    # tamano de lista o string");
                    ImGui::Text("  contiene(lst, item)  # busca elemento");
                    ImGui::Text("  quitar(lst)    # devuelve ultimo elemento");
                    ImGui::Separator();
                    ImGui::TextDisabled("Random:");
                    ImGui::Text("  azar(100)      # numero aleatorio 0-99");
                    ImGui::Separator();
                    ImGui::TextDisabled("Input:");
                    ImGui::Text("  leer_linea()   # lee string del teclado");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    // Theme editor (modal/panel si está activo)
    static ThemeEditor theme_editor_;
    static ProjectSettings project_settings_;
    project_settings_.set_visible(show_project_settings_);
    project_settings_.render();
    theme_editor_.set_visible(show_theme_editor_);
    theme_editor_.render();

// Render final.
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    int fbw, fbh;
    glfwGetFramebufferSize(win, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);
    glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Asegurar que el estado OpenGL esté correcto para ImGui
    // ImGui OpenGL2 backend usa pipeline fijo y NO puede resetear shaders/VBOs modernos.
    // Tenemos que resetear todo el estado "moderno" que el renderer del motor pudo haber dejado.
    glUseProgram(0);                          // desactivar shaders
    glBindBuffer(GL_ARRAY_BUFFER, 0);         // desatar VBOs
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // desatar IBOs
    for (int i = 0; i < 4; i++) {
        glDisableVertexAttribArray(i);        // desactivar attribs
        glClientActiveTexture(GL_TEXTURE0 + i);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    glClientActiveTexture(GL_TEXTURE0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    ImGui_ImplOpenGL2_RenderDrawData(draw_data);

    // Forzar flush de OpenGL
    glFlush();
}

// ===================== ImGui setup ===========================================
void EditorMain::setup_imgui_() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // ViewportsEnable deshabilitado en OpenGL 2.1 — causa problemas con contextos múltiples.
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = "arx_editor.ini";

    // ===== Cargar fuentes (igual que ProjectManagerApp) =====
    ImFontConfig config;
    config.OversampleH = 2;
    config.OversampleV = 2;

    // Font 0: Regular 16px (default)
    io.Fonts->AddFontFromFileTTF(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        16.0f, &config, io.Fonts->GetGlyphRangesDefault());

    // Font 1: Bold 18px (para títulos)
    io.Fonts->AddFontFromFileTTF(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        18.0f, &config, io.Fonts->GetGlyphRangesDefault());

    // Font 2: Mono 14px (para código/output)
    io.Fonts->AddFontFromFileTTF(
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        14.0f, &config, io.Fonts->GetGlyphRangesDefault());

    // ===== Tema oscuro estilo Godot/VS Code (igual que PM) =====
    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 6.0f;
    s.ChildRounding     = 6.0f;
    s.FrameRounding     = 4.0f;
    s.GrabRounding      = 4.0f;
    s.TabRounding       = 4.0f;
    s.ScrollbarRounding = 8.0f;
    s.ScrollbarSize     = 13.0f;
    s.WindowPadding     = ImVec2(12, 12);
    s.FramePadding      = ImVec2(8, 4);
    s.ItemSpacing       = ImVec2(8, 6);

    // Paleta ARX (púrpura + cyan como en Godot)
    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]         = ImVec4(0.10f, 0.10f, 0.13f, 1.0f);
    c[ImGuiCol_ChildBg]          = ImVec4(0.13f, 0.13f, 0.17f, 1.0f);
    c[ImGuiCol_PopupBg]          = ImVec4(0.13f, 0.13f, 0.17f, 0.98f);
    c[ImGuiCol_Border]           = ImVec4(0.22f, 0.22f, 0.28f, 1.0f);
    c[ImGuiCol_Text]             = ImVec4(0.91f, 0.91f, 0.94f, 1.0f);
    c[ImGuiCol_TextDisabled]     = ImVec4(0.50f, 0.50f, 0.55f, 1.0f);
    c[ImGuiCol_FrameBg]          = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    c[ImGuiCol_FrameBgHovered]   = ImVec4(0.26f, 0.26f, 0.32f, 1.0f);
    c[ImGuiCol_FrameBgActive]    = ImVec4(0.34f, 0.34f, 0.42f, 1.0f);
    c[ImGuiCol_TitleBg]          = ImVec4(0.10f, 0.10f, 0.13f, 1.0f);
    c[ImGuiCol_TitleBgActive]    = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    c[ImGuiCol_MenuBarBg]        = ImVec4(0.13f, 0.13f, 0.17f, 1.0f);
    c[ImGuiCol_ScrollbarBg]      = ImVec4(0.10f, 0.10f, 0.13f, 1.0f);
    c[ImGuiCol_ScrollbarGrab]    = ImVec4(0.30f, 0.30f, 0.34f, 1.0f);
    c[ImGuiCol_CheckMark]        = ImVec4(0.49f, 0.23f, 0.93f, 1.0f);  // púrpura
    c[ImGuiCol_SliderGrab]       = ImVec4(0.49f, 0.23f, 0.93f, 1.0f);
    c[ImGuiCol_Button]           = ImVec4(0.22f, 0.22f, 0.28f, 1.0f);
    c[ImGuiCol_ButtonHovered]    = ImVec4(0.49f, 0.23f, 0.93f, 1.0f);
    c[ImGuiCol_ButtonActive]     = ImVec4(0.60f, 0.30f, 1.00f, 1.0f);
    c[ImGuiCol_Header]           = ImVec4(0.49f, 0.23f, 0.93f, 0.5f);
    c[ImGuiCol_HeaderHovered]    = ImVec4(0.49f, 0.23f, 0.93f, 0.8f);
    c[ImGuiCol_HeaderActive]     = ImVec4(0.49f, 0.23f, 0.93f, 1.0f);
    c[ImGuiCol_Tab]              = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
    c[ImGuiCol_TabHovered]       = ImVec4(0.49f, 0.23f, 0.93f, 0.7f);
    c[ImGuiCol_TabActive]        = ImVec4(0.49f, 0.23f, 0.93f, 1.0f);
    c[ImGuiCol_DockingPreview]   = ImVec4(0.10f, 0.85f, 0.92f, 0.5f);  // ARX cyan
    c[ImGuiCol_DockingEmptyBg]   = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);

    // Backend.
    auto* win = static_cast<GLFWwindow*>(window_->native_handle());
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL2_Init();
    imgui_ready_ = true;
}

void EditorMain::shutdown_imgui_() {
    if (!imgui_ready_) return;
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    imgui_ready_ = false;
}

} // namespace arx
