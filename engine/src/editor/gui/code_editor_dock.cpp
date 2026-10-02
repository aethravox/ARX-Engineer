// ==============================================================================
// src/editor/gui/code_editor_dock.cpp — Editor de código Zen con tabs múltiples.
// ==============================================================================
#include "code_editor_dock.hpp"
#include "core/logging.hpp"

#include <imgui.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <filesystem>

namespace arx {

// Keywords de Zen (bilingüe ES/EN)
static const char* kZenKeywords[] = {
    "si", "sino", "para", "mientras", "muestra", "funcion", "estructura",
    "retorna", "romper", "continuar", "repetir", "desde", "hasta", "cada",
    "verdad", "falso", "nulo", "extern", "plataforma",
    "if", "else", "for", "while", "show", "print", "function", "struct",
    "return", "break", "continue", "repeat", "from", "to", "in",
    "true", "false", "null", "yes", "no",
    "y", "o", "no", "es", "en",
    "and", "or", "not", "is", "in",
    "coincidir", "caso", "por_defecto",
    "match", "case", "default",
    "importar", "incluir",
    "import", "include",
    "sino_si", "elif",
    "veces", "times",
    "siguiente"
};
constexpr int kNumKeywords = sizeof(kZenKeywords) / sizeof(kZenKeywords[0]);

static const char* kZenBuiltins[] = {
    "longitud", "contiene", "quitar", "azar", "numero", "texto", "leer_linea",
    "leer_archivo", "escribir_archivo", "reloj", "dormir", "limpiar",
    "subtexto", "buscar", "reemplazar", "mayusculas", "minusculas", "recortar",
    "agregar",
    "length", "contains", "pop", "random", "number", "string", "read_line",
    "read_file", "write_file", "clock", "sleep", "clear",
    "substring", "find", "replace", "uppercase", "lowercase", "trim",
    "push", "append", "add",
    "rango", "range", "salir", "exit"
};
constexpr int kNumBuiltins = sizeof(kZenBuiltins) / sizeof(kZenBuiltins[0]);

static bool is_ident_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}
static bool is_digit(char c) { return c >= '0' && c <= '9'; }

static bool iequals(const char* a, const char* b, int len) {
    for (int i = 0; i < len; ++i) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return false;
    }
    return true;
}

static bool is_keyword(const char* word, int len) {
    for (int i = 0; i < kNumKeywords; ++i) {
        if ((int)std::strlen(kZenKeywords[i]) == len && iequals(word, kZenKeywords[i], len))
            return true;
    }
    return false;
}

static bool is_builtin(const char* word, int len) {
    for (int i = 0; i < kNumBuiltins; ++i) {
        if ((int)std::strlen(kZenBuiltins[i]) == len && iequals(word, kZenBuiltins[i], len))
            return true;
    }
    return false;
}

// ==============================================================================
void CodeEditorDock::render() {
    if (!ImGui::Begin("Code Editor", nullptr,
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    render_menu_bar_();
    render_tabs_();
    render_editor_();

    ImGui::End();
}

// ==============================================================================
void CodeEditorDock::render_tabs_() {
    if (open_files_.empty()) return;

    ImGuiStyle& style = ImGui::GetStyle();
    float tab_height = ImGui::GetTextLineHeight() + style.FramePadding.y * 2.0f;

    ImGui::BeginChild("###code_tabs", ImVec2(0, tab_height + 4), false,
                       ImGuiWindowFlags_NoScrollbar);

    for (int i = 0; i < (int)open_files_.size(); ++i) {
        auto& f = open_files_[i];
        ImGui::PushID(i);

        // Botón X para cerrar
        bool clicked_close = false;
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 0.6f));
        std::string close_label = "x##close_" + std::to_string(i);
        if (ImGui::SmallButton(close_label.c_str())) {
            clicked_close = true;
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine();

        // Tab label (con * si está dirty)
        std::string label = f.name;
        if (f.dirty) label = "*" + label;

        bool is_active = (i == active_tab_);
        if (is_active) {
            ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_TabActive]);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_TabHovered]);
        }

        if (ImGui::SmallButton(label.c_str())) {
            active_tab_ = i;
        }

        if (is_active) ImGui::PopStyleColor(2);

        ImGui::SameLine();
        ImGui::PopID();

        if (clicked_close) {
            close_tab_(i);
            // Salir del loop porque los índices cambiaron
            break;
        }
    }

    ImGui::EndChild();
    ImGui::Separator();
}

// ==============================================================================
void CodeEditorDock::close_tab_(int idx) {
    if (idx < 0 || idx >= (int)open_files_.size()) return;

    // TODO: preguntar si guardar cambios si dirty
    // Por ahora, guardamos automáticamente
    if (open_files_[idx].dirty) {
        // Re-activar temporalmente para guardar
        int prev_active = active_tab_;
        active_tab_ = idx;
        save_file();
        active_tab_ = prev_active;
    }

    open_files_.erase(open_files_.begin() + idx);
    if (active_tab_ >= (int)open_files_.size()) {
        active_tab_ = (int)open_files_.size() - 1;
    }
    ARX_LOG_INFO("CodeEditor: cerrado tab {}", idx);
}

// ==============================================================================
void CodeEditorDock::render_menu_bar_() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                save_file();
            }
            if (ImGui::MenuItem("Reload from Disk")) {
                if (active_tab_ >= 0 && !open_files_[active_tab_].path.empty()) {
                    load_file(open_files_[active_tab_].path);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close Tab")) {
                if (active_tab_ >= 0) close_tab_(active_tab_);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Load main.zen")) {
                if (!project_dir_.empty()) {
                    load_file(project_dir_ + "/main.zen");
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Line numbers", nullptr, &show_line_numbers_);
            ImGui::SliderFloat("Font scale", &font_scale_, 0.5f, 2.0f);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Hotkey Ctrl+S
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S)) {
        save_file();
    }
}

// ==============================================================================
void CodeEditorDock::render_editor_() {
    if (open_files_.empty() || active_tab_ < 0) {
        ImGui::TextDisabled("No hay archivo cargado.");
        if (!project_dir_.empty()) {
            ImGui::Spacing();
            if (ImGui::Button("Cargar main.zen del proyecto")) {
                load_file(project_dir_ + "/main.zen");
            }
        }
        ImGui::TextDisabled("Doble-click en un .zen del FileSystem para abrirlo.");
        return;
    }

    auto& f = open_files_[active_tab_];

    ImGui::TextDisabled("%s", f.path.c_str());
    if (f.dirty) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1, 0.8f, 0, 1), "(sin guardar)");
    }

    ImGui::Separator();

    ImVec2 avail = ImGui::GetContentRegionAvail();
    if (avail.y < 50) return;

    // Line numbers panel
    if (show_line_numbers_) {
        ImGui::BeginChild("###line_numbers", ImVec2(45, avail.y), true);
        // Contar líneas
        int line_count = 1;
        for (char c : f.buffer) if (c == '\n') line_count++;
        for (int i = 1; i <= line_count; ++i) {
            ImGui::TextDisabled("%d", i);
        }
        ImGui::EndChild();
        ImGui::SameLine();
    }

    // Editor
    ImGui::PushItemWidth(-1);
    // Buffer editable: necesitamos uno con tamaño amplio
    static thread_local std::vector<char> edit_buf;
    edit_buf.assign(f.buffer.begin(), f.buffer.end());
    edit_buf.push_back('\\0');
    // Dejar espacio para que el usuario agregue texto
    edit_buf.resize(edit_buf.size() + 1024);

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    if (ImGui::InputTextMultiline("###code_editor", edit_buf.data(), edit_buf.size(),
                                   ImVec2(-1, avail.y - 20), flags)) {
        // Cambió el texto: actualizar buffer y marcar dirty
        std::string new_text(edit_buf.data());
        if (new_text != f.buffer) {
            f.buffer = new_text;
            f.dirty = true;
        }
    }
    ImGui::PopItemWidth();
}

// ==============================================================================
bool CodeEditorDock::load_file(const std::string& path) {
    if (path.empty()) return false;

    // ¿Ya está abierto? Solo activar el tab
    for (int i = 0; i < (int)open_files_.size(); ++i) {
        if (open_files_[i].path == path) {
            active_tab_ = i;
            ARX_LOG_INFO("CodeEditor: tab ya abierto, activado: {}", path);
            return true;
        }
    }

    // Cargar del disco
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        ARX_LOG_ERROR("CodeEditor: no se pudo abrir '{}'", path);
        return false;
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string content = ss.str();

    OpenFile f;
    f.path = path;
    f.name = std::filesystem::path(path).filename().string();
    f.buffer = content;
    f.dirty = false;

    open_files_.push_back(std::move(f));
    active_tab_ = (int)open_files_.size() - 1;

    ARX_LOG_INFO("CodeEditor: cargado '{}' ({} bytes) — tab #{}",
                 path, (int)content.size(), active_tab_);
    return true;
}

// ==============================================================================
bool CodeEditorDock::save_file() {
    if (active_tab_ < 0 || active_tab_ >= (int)open_files_.size()) return false;
    auto& f = open_files_[active_tab_];
    if (f.path.empty()) return false;

    std::ofstream ofs(f.path);
    if (!ofs.is_open()) {
        ARX_LOG_ERROR("CodeEditor: no se pudo escribir '{}'", f.path);
        return false;
    }
    ofs << f.buffer;
    ofs.close();
    f.dirty = false;

    ARX_LOG_INFO("CodeEditor: guardado '{}' ({} bytes)", f.path, (int)f.buffer.size());
    return true;
}

// ==============================================================================
void CodeEditorDock::set_project_dir(const std::string& d) {
    if (project_dir_ == d) return;
    project_dir_ = d;
    // Auto-cargar main.zen si existe
    if (!d.empty()) {
        namespace fs = std::filesystem;
        auto main_zen = fs::path(d) / "main.zen";
        if (fs::exists(main_zen)) {
            load_file(main_zen.string());
        }
    }
}

// ==============================================================================
const std::string& CodeEditorDock::file_path() const {
    static const std::string empty;
    if (active_tab_ < 0 || active_tab_ >= (int)open_files_.size()) return empty;
    return open_files_[active_tab_].path;
}

bool CodeEditorDock::is_dirty() const {
    if (active_tab_ < 0 || active_tab_ >= (int)open_files_.size()) return false;
    return open_files_[active_tab_].dirty;
}

} // namespace arx
