// ==============================================================================
// src/editor/gui/project_manager.cpp
// ==============================================================================
#include "project_manager.hpp"
#include "core/logging.hpp"

#include <imgui.h>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace arx {

namespace fs = std::filesystem;

static const char* kTemplates[] = {
    "Empty 2D",
    "Empty 3D",
    "Platformer 2D",
    "App Template (non-game)",
};

void ProjectManager::render() {
    if (!open_) return;

    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(),
                             ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    if (!ImGui::Begin("Project Manager", &open_,
                       ImGuiWindowFlags_NoDocking)) {
        ImGui::End();
        return;
    }

    if (mode_ == Mode::Browse) render_browse();
    else                        render_new_project_wizard();

    ImGui::End();
}

void ProjectManager::render_browse() {
    ImGui::TextDisabled("ARX Engine — Projects");
    ImGui::Separator();

    ImGui::BeginChild("recent_list", ImVec2(0, -40), true);
    if (recent_.empty()) {
        ImGui::TextDisabled("No recent projects.");
        ImGui::TextDisabled("Click 'New Project' to create one.");
    }
    for (const auto& p : recent_) {
        ImGui::PushID(p.path.c_str());
        ImGui::Bullet();
        ImGui::TextUnformatted(p.name.c_str());
        ImGui::SameLine(200);
        ImGui::TextDisabled("[%s]", p.kind.c_str());
        ImGui::SameLine(280);
        ImGui::TextDisabled("%s", p.last_modified.c_str());
        ImGui::SameLine();
        if (ImGui::Button("Open")) {
            current_project_ = p.path;
            open_ = false;
            ARX_LOG_INFO("Abriendo proyecto: {}", p.path);
        }
        ImGui::PopID();
    }
    ImGui::EndChild();

    if (ImGui::Button("New Project")) {
        mode_ = Mode::New;
        // Default path: ~/ARXProjects/
#ifdef _WIN32
        snprintf(path_buf_, sizeof(path_buf_), "%s\\ARXProjects\\",
                 getenv("USERPROFILE"));
#else
        snprintf(path_buf_, sizeof(path_buf_), "%s/ARXProjects/",
                 getenv("HOME"));
#endif
    }
    ImGui::SameLine();
    if (ImGui::Button("Open Folder...")) {
        // TODO: native dialog.
        ARX_LOG_INFO("Open folder dialog (TODO)");
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) open_ = false;
}

void ProjectManager::render_new_project_wizard() {
    ImGui::TextDisabled("New Project — wizard");
    ImGui::Separator();

    ImGui::Text("Project name:");
    ImGui::InputText("##name", name_buf_, sizeof(name_buf_));

    ImGui::Text("Location:");
    ImGui::InputText("##path", path_buf_, sizeof(path_buf_),
                      ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ImGui::Button("Browse...")) {
        // TODO: native dialog.
    }

    ImGui::Separator();
    ImGui::Text("Template:");
    for (int i = 0; i < IM_ARRAYSIZE(kTemplates); ++i) {
        ImGui::RadioButton(kTemplates[i], &template_idx_, i);
        if (i < IM_ARRAYSIZE(kTemplates) - 1) ImGui::SameLine();
    }

    ImGui::Separator();
    ImGui::Text("Engine version: 0.0.0   |   Renderer: OpenGL 3.3+");
    ImGui::Text("Platforms: Windows, Linux, Android, Web (HTML5)");

    ImGui::Separator();
    if (ImGui::Button("Create", ImVec2(120, 0))) {
        // Crear la estructura del proyecto.
        fs::path root = fs::path(path_buf_) / name_buf_;
        fs::create_directories(root);
        fs::create_directories(root / "scenes");
        fs::create_directories(root / "scripts");
        fs::create_directories(root / "assets" / "sprites");
        fs::create_directories(root / "assets" / "models");
        fs::create_directories(root / "assets" / "sounds");

        // project.arx (manifest)
        std::ofstream of(root / "project.arx");
        of << "{\n";
        of << "  \"name\": \"" << name_buf_ << "\",\n";
        of << "  \"engine_version\": \"0.0.0\",\n";
        of << "  \"kind\": \"" << (template_idx_ == 3 ? "app" : "game") << "\",\n";
        of << "  \"template\": \"" << kTemplates[template_idx_] << "\",\n";
        of << "  \"main_scene\": \"scenes/main.scene\",\n";
        of << "  \"platforms\": [\"windows\", \"linux\", \"android\", \"web\"],\n";
        of << "  \"export_presets\": []\n";
        of << "}\n";

        // main.scene vacío
        std::ofstream sc(root / "scenes" / "main.scene");
        sc << "[scene_main]\nroot=Vox\n";

        current_project_ = root.string();
        mode_ = Mode::Browse;
        open_ = false;
        ARX_LOG_INFO("Proyecto creado: {}", current_project_);
    }
    ImGui::SameLine();
    if (ImGui::Button("Back")) mode_ = Mode::Browse;
}

void ProjectManager::scan_recent_projects() {
    // TODO: leer ~/.config/arx/recent.json
}

} // namespace arx
