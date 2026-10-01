// ==============================================================================
// src/editor/editor_main.cpp
// ==============================================================================
#include "editor_main.hpp"
#include "gui/editor_dock_manager.hpp"
#include "gui/project_manager.hpp"
#include "scene/scene_tree.hpp"
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
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <cstdio>

namespace arx {

EditorMain::EditorMain()  = default;
EditorMain::~EditorMain() = default;

bool EditorMain::init(Window* win, Renderer* r) {
    window_  = win;
    renderer_ = r;
    setup_imgui_();

    docks_       = std::make_unique<EditorDockManager>();
    project_mgr_ = std::make_unique<ProjectManager>();

    docks_->init(renderer_);
    ARX_LOG_INFO("EditorMain inicializado");
    return true;
}

void EditorMain::start() {
    ARX_LOG_INFO("EditorMain::start — splash visible");
    splash_time_ = 0.0f;
    splash_done_ = false;
}

bool EditorMain::process(float delta) {
    // 1. Poll events.
    window_->poll_events();

    // 2. Splash (2 segundos).
    if (!splash_done_) {
        splash_time_ += delta;
        render_splash_(splash_time_);
        window_->swap_buffers();
        if (splash_time_ >= 2.0f) splash_done_ = true;
        return true;
    }

    // 3. Main UI (ImGui).
    render_main_ui_(delta);

    // 4. Swap.
    window_->swap_buffers();

    stats_.delta_seconds = delta;
    stats_.fps = 1.0f / std::max(delta, 1e-6f);
    stats_.frame_usec = static_cast<uint64_t>(delta * 1e6);

    return !window_->should_close();
}

void EditorMain::finish() {
    shutdown_imgui_();
    ARX_LOG_INFO("EditorMain::finish");
}

// ===================== Splash ================================================
void EditorMain::render_splash_(float t) {
    // Cargar splash.png desde assets embebidos (ARX_SPLASH_PATH).
    static bool loaded = false;
    static unsigned int tex = 0;
    static int w = 0, h = 0;
    if (!loaded) {
        int c = 0;
        unsigned char* data = stbi_load(ARX_SPLASH_PATH, &w, &h, &c, 4);
        if (data) {
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            stbi_image_free(data);
            loaded = true;
        }
    }

    // Frame simple: limpia + dibuja la textura centrada + fade in/out.
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!imgui_ready_) {
        // No usamos ImGui para el splash (mejor rendimiento al inicio).
        // Render simple con OpenGL fijo ( omitted por brevedad ).
    } else {
        // Splash con ImGui (suficiente para mostrar la imagen).
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        auto* win = static_cast<GLFWwindow*>(window_->native_handle());
        int ww, wh; glfwGetWindowSize(win, &ww, &wh);
        ImGui::SetNextWindowSize(ImVec2(ww, wh));
        ImGui::Begin("Splash", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
        ImVec2 sz(static_cast<float>(w), static_cast<float>(h));
        ImVec2 pos((ww - sz.x) * 0.5f, (wh - sz.y) * 0.5f);
        ImGui::SetCursorPos(pos);

        // Fade-in / fade-out.
        float alpha = 1.0f;
        if (t < 0.3f) alpha = t / 0.3f;
        else if (t > 1.7f) alpha = (2.0f - t) / 0.3f;
        ImGui::Image(static_cast<ImTextureID>((intptr_t)tex), sz,
                     ImVec2(0,0), ImVec2(1,1),
                     ImVec4(1,1,1,alpha), ImVec4(0,0,0,0));

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}

// ===================== Main UI ===============================================
void EditorMain::render_main_ui_(float delta) {
    // ImGui frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

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
    if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr) {
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
            ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::DockBuilderSetNodeSize(dockspace_id, vp->WorkSize);

        ImGuiID main = dockspace_id;
        ImGuiID bottom, top;
        ImGui::DockBuilderSplitNode(main, ImGuiDir_Down, 0.25f, &bottom, &top);

        ImGuiID left, center, right;
        ImGui::DockBuilderSplitNode(top, ImGuiDir_Left,  0.20f, &left, &center);
        ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.25f, &right, &center);

        ImGui::DockBuilderDockWindow("Scene Tree",   left);
        ImGui::DockBuilderDockWindow("FileSystem",   left);
        ImGui::DockBuilderDockWindow("Viewport",     center);
        ImGui::DockBuilderDockWindow("Inspector",    right);
        ImGui::DockBuilderDockWindow("Output",       bottom);
        ImGui::DockBuilderFinish(dockspace_id);
    }
    ImGui::DockSpace(dockspace_id, ImVec2(0,0),
                     ImGuiDockNodeFlags_PassthruCentralNode |
                     ImGuiDockNodeFlags_NoTabBar);
    ImGui::End();

    // Menu bar superior.
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Project"))  {}
            if (ImGui::MenuItem("Open Project")) { project_mgr_->show_open_dialog(); }
            ImGui::Separator();
            if (ImGui::MenuItem("Save Scene"))   {}
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
            if (ImGui::MenuItem("Quit", "Alt+F4")) {
                if (auto* w = window_) glfwSetWindowShouldClose(
                    static_cast<GLFWwindow*>(w->native_handle()), GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Run", "F5"))    {}
            if (ImGui::MenuItem("Pause","F7"))   {}
            if (ImGui::MenuItem("Stop", "F8"))   {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Editor")) {
            if (ImGui::MenuItem("Project Settings...")) {}
            if (ImGui::MenuItem("Editor Settings..."))  {}
            ImGui::Separator();
            if (ImGui::MenuItem("About ARX Engine"))    {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("Documentation"))     {}
            if (ImGui::MenuItem("Report a Bug"))      {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Export .aex dialog (modal).
    if (export_aex_dialog_.is_open()) {
        export_aex_dialog_.render();
    }

    // Docks (cada uno dibuja su ventana).
    docks_->render_all(delta);

    // Project Manager (modal si está abierto).
    project_mgr_->render();

    // Render final.
    ImGui::Render();
    int fbw, fbh;
    glfwGetFramebufferSize(win, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);
    glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// ===================== ImGui setup ===========================================
void EditorMain::setup_imgui_() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = "arx_editor.ini";

    // Tema oscuro estilo Godot.
    ImGui::StyleColorsDark();
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding    = 4.0f;
    s.FrameRounding     = 2.0f;
    s.GrabRounding      = 2.0f;
    s.TabRounding       = 4.0f;
    s.ScrollbarRounding = 6.0f;
    s.ScrollbarSize     = 13.0f;
    s.WindowPadding     = ImVec2(8, 8);
    s.FramePadding      = ImVec2(4, 2);
    s.ItemSpacing       = ImVec2(8, 4);

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]         = ImVec4(0.13f, 0.13f, 0.16f, 1.0f);
    c[ImGuiCol_ChildBg]          = ImVec4(0.10f, 0.10f, 0.13f, 1.0f);
    c[ImGuiCol_PopupBg]          = ImVec4(0.13f, 0.13f, 0.16f, 0.96f);
    c[ImGuiCol_Border]           = ImVec4(0.22f, 0.22f, 0.27f, 1.0f);
    c[ImGuiCol_Text]             = ImVec4(0.89f, 0.89f, 0.92f, 1.0f);
    c[ImGuiCol_TextDisabled]     = ImVec4(0.50f, 0.50f, 0.55f, 1.0f);
    c[ImGuiCol_FrameBg]          = ImVec4(0.20f, 0.20f, 0.24f, 1.0f);
    c[ImGuiCol_FrameBgHovered]   = ImVec4(0.28f, 0.28f, 0.32f, 1.0f);
    c[ImGuiCol_FrameBgActive]    = ImVec4(0.35f, 0.35f, 0.40f, 1.0f);
    c[ImGuiCol_TitleBg]          = ImVec4(0.10f, 0.10f, 0.13f, 1.0f);
    c[ImGuiCol_TitleBgActive]    = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);
    c[ImGuiCol_MenuBarBg]        = ImVec4(0.13f, 0.13f, 0.16f, 1.0f);
    c[ImGuiCol_ScrollbarBg]      = ImVec4(0.10f, 0.10f, 0.13f, 1.0f);
    c[ImGuiCol_ScrollbarGrab]    = ImVec4(0.30f, 0.30f, 0.34f, 1.0f);
    c[ImGuiCol_CheckMark]        = ImVec4(0.42f, 0.20f, 0.85f, 1.0f);  // ARX purple
    c[ImGuiCol_SliderGrab]       = ImVec4(0.42f, 0.20f, 0.85f, 1.0f);
    c[ImGuiCol_Button]           = ImVec4(0.22f, 0.22f, 0.27f, 1.0f);
    c[ImGuiCol_ButtonHovered]    = ImVec4(0.42f, 0.20f, 0.85f, 1.0f);
    c[ImGuiCol_ButtonActive]     = ImVec4(0.55f, 0.30f, 0.95f, 1.0f);
    c[ImGuiCol_Header]           = ImVec4(0.42f, 0.20f, 0.85f, 0.5f);
    c[ImGuiCol_HeaderHovered]    = ImVec4(0.42f, 0.20f, 0.85f, 0.8f);
    c[ImGuiCol_HeaderActive]     = ImVec4(0.42f, 0.20f, 0.85f, 1.0f);
    c[ImGuiCol_Tab]              = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
    c[ImGuiCol_TabHovered]       = ImVec4(0.42f, 0.20f, 0.85f, 0.7f);
    c[ImGuiCol_TabActive]        = ImVec4(0.42f, 0.20f, 0.85f, 1.0f);
    c[ImGuiCol_DockingPreview]   = ImVec4(0.10f, 0.85f, 0.92f, 0.5f);  // ARX cyan
    c[ImGuiCol_DockingEmptyBg]   = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);

    // Backend.
    auto* win = static_cast<GLFWwindow*>(window_->native_handle());
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    imgui_ready_ = true;
}

void EditorMain::shutdown_imgui_() {
    if (!imgui_ready_) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    imgui_ready_ = false;
}

} // namespace arx
