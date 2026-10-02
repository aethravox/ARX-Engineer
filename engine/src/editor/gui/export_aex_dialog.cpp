// ==============================================================================
// engine/src/editor/gui/export_aex_dialog.cpp
// ==============================================================================

#include "export_aex_dialog.hpp"
#include "export/aex_exporter.hpp"
#include "core/logging.hpp"

#include <imgui.h>

#include <filesystem>
#include <cstring>

namespace arx {

namespace fs = std::filesystem;

void ExportAexDialog::open(const std::string& project_root) {
    project_root_ = project_root;
    open_ = true;
    // Default output path: <project_root>/build/<app_name>.aex
    if (project_root_.empty()) {
        std::strcpy(out_path_buf_, "build/output.aex");
    } else {
        fs::path default_out = fs::path(project_root) / "build" / "output.aex";
        std::strcpy(out_path_buf_, default_out.string().c_str());
    }
}

void ExportAexDialog::close() {
    open_ = false;
}

bool ExportAexDialog::render() {
    if (!open_) return false;

    ImGui::SetNextWindowSize(ImVec2(640, 360), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f,
                                     ImGui::GetIO().DisplaySize.y * 0.5f),
                              ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    bool export_done = false;

    if (ImGui::Begin("Export to .aex", &open_,
                      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse)) {
        ImGui::TextDisabled("ARX Package (.aex) — formato firmado criptograficamente");
        ImGui::Separator();

        // App name
        ImGui::Text("App name:");
        ImGui::SameLine();
        ImGui::InputText("##app_name", app_name_buf_, sizeof(app_name_buf_));

        // Output path
        ImGui::Text("Output:");
        ImGui::SameLine();
        ImGui::InputText("##out_path", out_path_buf_, sizeof(out_path_buf_));
        ImGui::SameLine();
        if (ImGui::Button("...##out_browse")) {
            // TODO: usar tinyfiledialogs para OpenFileDialog nativo
            // Por ahora, no hacemos nada (el usuario escribe la ruta a mano)
        }

        // Creator key (opcional)
        ImGui::Text("Creator key:");
        ImGui::SameLine();
        ImGui::InputTextWithHint("##key_path", "(opcional — se genera efimera)",
                                   key_path_buf_, sizeof(key_path_buf_));
        ImGui::SameLine();
        if (ImGui::Button("...##key_browse")) {
            // TODO: file dialog nativo
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Ruta a un archivo binario de 64 bytes (seed || pub).\n"
                              "Genera una con: arx_keygen creator_priv.bin creator_pub.bin");
        }

        ImGui::Separator();

        // Botones
        if (ImGui::Button("Export", ImVec2(120, 0))) {
            // Ejecutar export
            AexExporter exporter;
            exporter.set_creator_key_path(std::string(key_path_buf_));

            ExportPreset preset;
            preset.platform   = "aex";
            preset.app_name   = app_name_buf_;
            preset.app_version = "1.0.0";
            preset.aot_arxscript = false;
            preset.embed_assets  = true;

            fs::path root = project_root_.empty() ? fs::current_path() : fs::path(project_root_);
            last_result_ = exporter.export_project(root, preset);
            last_result_valid_ = true;
            export_done = true;

            if (last_result_.success) {
                ARX_LOG_INFO("Export .aex OK: {}", last_result_.output_path);
                show_success_modal_ = true;
            } else {
                ARX_LOG_ERROR("Export .aex FAIL: {}", last_result_.error_message);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            open_ = false;
        }

        // Resultado
        if (last_result_valid_) {
            ImGui::Separator();
            if (last_result_.success) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));
                ImGui::TextWrapped("OK: %s", last_result_.output_path.c_str());
                ImGui::PopStyleColor();
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                ImGui::TextWrapped("FAIL: %s", last_result_.error_message.c_str());
                ImGui::PopStyleColor();
            }

            // Warnings
            for (const auto& w : last_result_.warnings) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.4f, 1.0f));
                ImGui::TextWrapped("WARN: %s", w.c_str());
                ImGui::PopStyleColor();
            }

            // Logs
            if (ImGui::CollapsingHeader("Logs")) {
                ImGui::BeginChild("logs", ImVec2(0, 120), true);
                for (const auto& l : last_result_.logs) {
                    ImGui::TextUnformatted(l.c_str());
                }
                ImGui::EndChild();
            }
        }

        ImGui::TextDisabled("Tip: si no provees una clave, se generara una efimera "
                              "(no verificable por trust stores reales).");
    }
    ImGui::End();

    // Modal de exito
    if (show_success_modal_) {
        ImGui::OpenPopup("Export OK###aex_success");
        show_success_modal_ = false;
    }
    if (ImGui::BeginPopupModal("Export OK###aex_success", nullptr,
                                 ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Paquete .aex creado:");
        ImGui::Separator();
        ImGui::TextWrapped("%s", last_result_.output_path.c_str());
        ImGui::Text("Tamano: %lld bytes",
                     static_cast<long long>(last_result_.total_bytes));
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            open_ = false;
        }
        ImGui::EndPopup();
    }

    return export_done;
}

} // namespace arx
