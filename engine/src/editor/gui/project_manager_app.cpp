// ==============================================================================
// src/editor/gui/project_manager_app.cpp
// ==============================================================================
#include "project_manager_app.hpp"
#include "core/logging.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib>

namespace arx {

namespace fs = std::filesystem;

// Definición de la variable global declarada en main.cpp
// (comunicación PM → main loop para transición PM → Editor)
extern std::string g_selected_project;

ProjectManagerApp::~ProjectManagerApp() {
    if (imgui_ready_) {
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

bool ProjectManagerApp::init(Window* win, Renderer* r) {
    window_   = win;
    renderer_ = r;
    ARX_LOG_INFO("ProjectManagerApp inicializado");
    return true;
}

void ProjectManagerApp::start() {
    ARX_LOG_INFO("ProjectManagerApp::start — inicializando ImGui");
    if (!imgui_ready_) {
        setup_imgui_();
    }
    // Escanear carpeta default de proyectos
    scan_default_folder_();
}

bool ProjectManagerApp::process(float delta) {
    window_->poll_events();
    render_main_ui_();
    window_->swap_buffers();
    return !window_->should_close() && selected_project_.empty();
}

void ProjectManagerApp::finish() {
    if (imgui_ready_) {
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        imgui_ready_ = false;
    }
    ARX_LOG_INFO("ProjectManagerApp::finish");
}

void ProjectManagerApp::scan_default_folder_() {
    // Carpeta default: ~/ARXProjects
    const char* home = getenv("HOME");
    if (!home) return;
    fs::path projects_dir = fs::path(home) / "ARXProjects";
    if (!fs::exists(projects_dir)) {
        fs::create_directories(projects_dir);
    }
    scan_folder_(projects_dir);
}

void ProjectManagerApp::scan_folder_(const fs::path& folder) {
    recent_.clear();
    if (!fs::exists(folder)) return;
    for (auto& e : fs::directory_iterator(folder)) {
        if (!e.is_directory()) continue;
        // Un proyecto válido tiene project.arx
        fs::path proj_file = e.path() / "project.arx";
        if (!fs::exists(proj_file)) continue;

        RecentProject rp;
        rp.name = e.path().filename().string();
        rp.path = e.path().string();

        // Last modified
        auto ftime = fs::last_write_time(e.path());
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t t = std::chrono::system_clock::to_time_t(sctp);
        std::ostringstream ss;
        ss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M");
        rp.last_modified = ss.str();

        // Kind por ahora todos "game" (se podría leer del project.arx)
        rp.kind = "game";
        rp.icon_color = "#7c3aed";  // púrpura

        recent_.push_back(rp);
    }
    ARX_LOG_INFO("ProjectManagerApp: {} proyectos encontrados en {}", recent_.size(), folder.string());
}

void ProjectManagerApp::render_main_ui_() {
    // ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Pantalla completa
    auto* win = static_cast<GLFWwindow*>(window_->native_handle());
    int ww, wh;
    glfwGetWindowSize(win, &ww, &wh);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(ww, wh));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoBringToFrontOnFocus;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("###ProjectManagerApp", nullptr, flags);
    ImGui::PopStyleVar(2);

    // Header con título grande
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // bold
    ImGui::Text("ARX Engine");
    ImGui::PopFont();
    ImGui::SameLine();
    ImGui::TextDisabled("Project Manager");
    ImGui::Separator();

    if (show_wizard_) {
        render_new_project_wizard_();
    } else {
        // Toolbar
        if (ImGui::Button("New Project", ImVec2(120, 30))) {
            show_wizard_ = true;
            // Default path
            const char* home = getenv("HOME");
            if (home) {
                snprintf(path_buf_, sizeof(path_buf_), "%s/ARXProjects/", home);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Scan Folder...", ImVec2(120, 30))) {
            // Usar zenity (Linux) para abrir diálogo nativo de selección de carpeta
            std::string cmd = "zenity --file-selection --directory --title='Select projects folder' 2>/dev/null";
            FILE* pipe = popen(cmd.c_str(), "r");
            if (pipe) {
                char buffer[1024];
                std::string result;
                while (fgets(buffer, sizeof(buffer), pipe)) {
                    result += buffer;
                }
                pclose(pipe);
                // Quitar newline
                while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
                    result.pop_back();
                }
                if (!result.empty()) {
                    ARX_LOG_INFO("Scaneando carpeta: {}", result);
                    scan_folder_(result);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh", ImVec2(80, 30))) {
            // Reescanear carpeta default
            scan_default_folder_();
            ARX_LOG_INFO("Lista de proyectos refrescada");
        }
        ImGui::SameLine();
        ImGui::TextDisabled("  |  Filter:");
        ImGui::SameLine();
        ImGui::PushItemWidth(200);
        ImGui::InputText("##filter", filter_buf_, sizeof(filter_buf_));
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Clear", ImVec2(60, 30))) {
            filter_buf_[0] = 0;
        }
        ImGui::SameLine();
        // About button en PM
        if (ImGui::Button("About", ImVec2(60, 30))) {
            show_about_ = true;
        }

        // About dialog del PM
        if (show_about_) {
            ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
            if (ImGui::Begin("About ARX Engine##PM", &show_about_,
                             ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse)) {
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
                ImGui::Text("ARX Engine");
                ImGui::PopFont();
                ImGui::TextDisabled("Version %s", "0.0.0");
                ImGui::Separator();
                ImGui::TextWrapped("Motor de apps y juegos nativo C++ con Zen Lang.");
                ImGui::TextWrapped("Powered by Aethravox Studios.");
                ImGui::Separator();
                ImGui::BulletText("Zen: bilingue ES/EN, LLVM AOT + VM");
                ImGui::BulletText("Renderer: OpenGL 2.1+ / GLES 3.0");
                ImGui::BulletText("Fisica: Bullet + Jolt (dual)");
                ImGui::BulletText("UI: Dear ImGui con docking");
                ImGui::BulletText("Cripto: Ed25519 + SHA-256");
                ImGui::Separator();
                ImGui::TextDisabled("Licencia: MIT");
                ImGui::TextDisabled("100%% codigo original");
                ImGui::Separator();
                if (ImGui::Button("Cerrar", ImVec2(100, 0))) show_about_ = false;
            }
            ImGui::End();
        }

        ImGui::Separator();

        // Grid de proyectos
        if (recent_.empty()) {
            ImGui::TextDisabled("\n\nNo projects found.");
            ImGui::TextDisabled("Create one with 'New Project' button.");
        } else {
            // Layout grid: 4 columnas, cada tile ~200x180
            float tile_w = 200.0f;
            float tile_h = 180.0f;
            float spacing = 12.0f;
            int cols = std::max(1, (int)(ww / (tile_w + spacing)));

            int visible_count = 0;
            for (size_t i = 0; i < recent_.size(); i++) {
                const auto& p = recent_[i];

                // Filter
                if (filter_buf_[0] != 0) {
                    std::string filter = filter_buf_;
                    if (p.name.find(filter) == std::string::npos) continue;
                }

                ImGui::PushID((int)i);

                // Posición en grid
                int row = visible_count / cols;
                int col = visible_count % cols;
                ImGui::SetCursorPos(ImVec2(20 + col * (tile_w + spacing),
                                            100 + row * (tile_h + spacing)));

                // Tile
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16f, 0.16f, 0.22f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.16f, 0.16f, 0.22f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.22f, 0.18f, 0.30f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.30f, 0.22f, 0.45f, 1.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);

                ImGui::BeginChild(("###proj" + std::to_string(i)).c_str(),
                                  ImVec2(tile_w, tile_h), true,
                                  ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
                {
                    // Icono grande (cuadrado de color)
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.49f, 0.23f, 0.93f, 1.0f));
                    ImGui::BeginChild("###icon", ImVec2(tile_w - 24, 80), true);
                    ImGui::EndChild();
                    ImGui::PopStyleColor();

                    // Nombre del proyecto
                    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // bold
                    ImGui::TextWrapped("%s", p.name.c_str());
                    ImGui::PopFont();

                    // Fecha
                    ImGui::TextDisabled("%s", p.last_modified.c_str());

                    // Botón Open
                    if (ImGui::Button("Open", ImVec2(tile_w - 24, 24))) {
                        selected_project_ = p.path;
                        g_selected_project = p.path;
                        ARX_LOG_INFO("Proyecto seleccionado: {}", p.path);
                    }
                }
                ImGui::EndChild();

                ImGui::PopStyleVar();
                ImGui::PopStyleColor(4);
                ImGui::PopID();

                visible_count++;
            }
        }
    }

    ImGui::End();

    // Render
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    int fbw, fbh;
    glfwGetFramebufferSize(win, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);
    glClearColor(0.10f, 0.10f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset GL state (igual que en editor_main)
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

void ProjectManagerApp::render_new_project_wizard_() {
    ImGui::TextDisabled("New Project — wizard");
    ImGui::Separator();

    ImGui::Text("Project name:");
    ImGui::InputText("##name", name_buf_, sizeof(name_buf_));

    ImGui::Text("Location:");
    ImGui::InputText("##path", path_buf_, sizeof(path_buf_));
    ImGui::SameLine();
    if (ImGui::Button("Browse...")) {
        // Usar zenity para seleccionar carpeta
        std::string cmd = "zenity --file-selection --directory --title='Select project location' 2>/dev/null";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[1024];
            std::string result;
            while (fgets(buffer, sizeof(buffer), pipe)) {
                result += buffer;
            }
            pclose(pipe);
            while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
                result.pop_back();
            }
            if (!result.empty()) {
                // Asegurar que termine con /
                if (result.back() != '/') result += "/";
                strncpy(path_buf_, result.c_str(), sizeof(path_buf_) - 1);
                path_buf_[sizeof(path_buf_) - 1] = 0;
            }
        }
    }

    ImGui::Text("Template:");
    const char* templates[] = {"Empty 2D", "Empty 3D", "Platformer 2D", "App Template"};
    ImGui::Combo("##template", &template_idx_, templates, 4);

    ImGui::Separator();

    if (ImGui::Button("Create", ImVec2(100, 30))) {
        // Validar nombre
        std::string name = name_buf_;
        if (name.empty()) {
            ARX_LOG_ERROR("Nombre de proyecto vacío");
        } else {
            // Path: si path_buf_ está vacío, usar ~/ARXProjects/
            std::string path = path_buf_;
            if (path.empty()) {
                const char* home = getenv("HOME");
                if (home) path = std::string(home) + "/ARXProjects/";
            }
            fs::path proj_dir = fs::path(path) / name;
            if (fs::exists(proj_dir)) {
                ARX_LOG_ERROR("El directorio ya existe: {}", proj_dir.string());
            } else {
                fs::create_directories(proj_dir);
                // Crear project.arx vacío
                std::ofstream(proj_dir / "project.arx") << "";
                // Crear main.zen básico
                std::ofstream(proj_dir / "main.zen") << 
                    "# " << name << "\n"
                    "muestra \"Hello from " << name << "!\"\n";

                // Crear assets/
                fs::create_directories(proj_dir / "assets");

                selected_project_ = proj_dir.string();
                g_selected_project = proj_dir.string();
                ARX_LOG_INFO("Proyecto creado: {}", proj_dir.string());
                // Reescanear carpeta de proyectos para que aparezca en la lista
                scan_default_folder_();
            }
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(100, 30))) {
        show_wizard_ = false;
    }
}

void ProjectManagerApp::setup_imgui_() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Cargar fuentes (DejaVu Sans ya está en el sistema)
    ImFontConfig config;
    config.OversampleH = 2;
    config.OversampleV = 2;

    // Font 0: Regular 16px
    io.Fonts->AddFontFromFileTTF(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        16.0f, &config, io.Fonts->GetGlyphRangesDefault());

    // Font 1: Bold 18px (para títulos)
    io.Fonts->AddFontFromFileTTF(
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        18.0f, &config, io.Fonts->GetGlyphRangesDefault());

    // Font 2: Mono 14px
    io.Fonts->AddFontFromFileTTF(
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        14.0f, &config, io.Fonts->GetGlyphRangesDefault());

    io.IniFilename = "arx_pm.ini";

    // Tema oscuro estilo Godot/VS Code
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

    // Backend
    auto* win = static_cast<GLFWwindow*>(window_->native_handle());
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL2_Init();
    imgui_ready_ = true;
}

} // namespace arx
