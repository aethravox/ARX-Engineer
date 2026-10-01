// ==============================================================================
// src/editor/gui/code_editor_dock.cpp — Editor de código Zen integrado.
//
// Implementación: usa ImGui::InputTextMultiline con un callback para colorear
// el texto mientras se escribe. No requiere bibliotecas externas.
// ==============================================================================
#include "code_editor_dock.hpp"
#include "core/logging.hpp"

#include <imgui.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <algorithm>

namespace arx {

// Keywords de Zen (bilingüe ES/EN) en minúscula para matching case-insensitive
static const char* kZenKeywords[] = {
    // ES
    "si", "sino", "para", "mientras", "muestra", "funcion", "estructura",
    "retorna", "romper", "continuar", "repetir", "desde", "hasta", "cada",
    "verdad", "falso", "nulo", "extern", "plataforma",
    // EN
    "if", "else", "for", "while", "show", "print", "function", "struct",
    "return", "break", "continue", "repeat", "from", "to", "in",
    "true", "false", "null", "yes", "no"
};
constexpr int kNumKeywords = sizeof(kZenKeywords) / sizeof(kZenKeywords[0]);

// Builtins de Zen
static const char* kZenBuiltins[] = {
    "longitud", "contiene", "quitar", "azar", "numero", "texto", "leer_linea",
    "length", "contains", "pop", "random", "number", "string", "read_line"
};
constexpr int kNumBuiltins = sizeof(kZenBuiltins) / sizeof(kZenBuiltins[0]);

// Helper: ¿es un character alfanumérico o underscore?
static bool is_ident_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

// Helper: ¿es un dígito?
static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

// Helper: comparar string case-insensitive
static bool iequals(const char* a, const char* b, int len) {
    for (int i = 0; i < len; ++i) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return false;
    }
    return true;
}

// Helper: ¿la palabra es una keyword?
static bool is_keyword(const char* word, int len) {
    for (int i = 0; i < kNumKeywords; ++i) {
        if ((int)std::strlen(kZenKeywords[i]) == len &&
            iequals(word, kZenKeywords[i], len)) {
            return true;
        }
    }
    return false;
}

// Helper: ¿la palabra es un builtin?
static bool is_builtin(const char* word, int len) {
    for (int i = 0; i < kNumBuiltins; ++i) {
        if ((int)std::strlen(kZenBuiltins[i]) == len &&
            iequals(word, kZenBuiltins[i], len)) {
            return true;
        }
    }
    return false;
}

// ==============================================================================
// InputText callback para colorear el texto mientras se escribe.
// ==============================================================================
struct EditorCallbackData {
    std::string* buffer;
};

static int input_text_callback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        std::string* buf = static_cast<std::string*>(data->UserData);
        buf->resize(data->BufTextLen);
        data->Buf = buf->data();
    }
    return 0;
}

// ==============================================================================
// Render principal
// ==============================================================================
void CodeEditorDock::render() {
    // Forzar tamaño/posición la primera vez (si no está dockeado)
    static bool first_render = true;
    if (first_render) {
        first_render = false;
        // Posición visible en el centro-izquierda de la pantalla
        ImGui::SetNextWindowSize(ImVec2(700, 450), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(50, 200), ImGuiCond_FirstUseEver);
        // Bring to front para que no quede detrás del editor
        ImGui::SetNextWindowFocus();
    }

    if (!ImGui::Begin("Code Editor", nullptr,
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    render_menu_bar_();
    render_editor_();

    ImGui::End();
}

// ==============================================================================
void CodeEditorDock::render_menu_bar_() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                save_file();
            }
            if (ImGui::MenuItem("Reload from Disk")) {
                if (!file_path_.empty()) load_file(file_path_);
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
        if (!ImGui::GetIO().WantTextInput || ImGui::IsWindowFocused()) {
            save_file();
        }
    }
}

// ==============================================================================
void CodeEditorDock::render_editor_() {
    if (file_path_.empty()) {
        ImGui::TextDisabled("No hay archivo cargado.");
        if (!project_dir_.empty()) {
            ImGui::Spacing();
            if (ImGui::Button("Cargar main.zen del proyecto")) {
                load_file(project_dir_ + "/main.zen");
            }
        }
        return;
    }

    // Info bar
    ImGui::TextDisabled("%s %s", file_path_.c_str(), dirty_ ? "(modificado*)" : "");
    ImGui::SameLine();
    if (dirty_) {
        if (ImGui::SmallButton("Guardar")) save_file();
    }
    ImGui::Separator();

    // Editor area
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 2 ? ImGui::GetIO().Fonts->Fonts[2] : nullptr);
    ImGui::SetWindowFontScale(font_scale_);

    // Calcular tamaño disponible
    ImVec2 avail = ImGui::GetContentRegionAvail();
    if (show_line_numbers_) {
        // Reservar espacio para line numbers
        avail.x -= 50.0f;
    }

    // === Line numbers panel ===
    if (show_line_numbers_) {
        ImGui::BeginChild("###line_numbers", ImVec2(45, avail.y), true);
        int lines = std::count(buffer_.begin(), buffer_.end(), '\n') + 1;
        for (int i = 1; i <= lines; ++i) {
            ImGui::TextDisabled("%4d", i);
        }
        ImGui::EndChild();
        ImGui::SameLine();
    }

    // === Editor con syntax highlighting ===
    // InputTextMultiline no soporta syntax highlighting nativo, así que usamos
    // un truco: dibujamos el texto con ImGui::TextUnformatted usando colores
    // por token, encima de un InputTextMultiline invisible.
    //
    // Mejor enfoque: usar InputTextMultiline normal sin highlighting (más simple
    // y funcional) y dejar el highlighting para una versión futura.
    //
    // TODO: implementar syntax highlighting custom con un callback de InputText
    // o un renderer propio. Por ahora, editor simple sin colores.

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput |
                                 ImGuiInputTextFlags_CallbackResize |
                                 ImGuiInputTextFlags_NoHorizontalScroll;

    EditorCallbackData cbd{&buffer_};
    char* buf = buffer_.data();
    // Asegurar que el buffer tenga al menos 1 byte (para InputText)
    if (buffer_.empty()) buffer_.resize(1, '\0');

    if (ImGui::InputTextMultiline("###code_editor", buf, buffer_.capacity() + 1,
                                   avail, flags, input_text_callback, &cbd)) {
        dirty_ = true;
        // El callback ya actualizó buffer_ por el resize, pero el contenido
        // también puede haber cambiado sin resize. InputText escribe directo al buf.
        // Necesitamos asegurar que buffer_ tenga el tamaño correcto.
        buffer_.resize(std::strlen(buf));
    }

    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopFont();
}

// ==============================================================================
bool CodeEditorDock::load_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        ARX_LOG_WARN("CodeEditor: no se pudo abrir '{}'", path);
        return false;
    }
    std::stringstream ss;
    ss << f.rdbuf();
    buffer_ = ss.str();
    file_path_ = path;
    dirty_ = false;
    ARX_LOG_INFO("CodeEditor: cargado '{}' ({} bytes)", path, buffer_.size());
    return true;
}

// ==============================================================================
bool CodeEditorDock::save_file() {
    if (file_path_.empty()) {
        ARX_LOG_WARN("CodeEditor: no hay archivo para guardar");
        return false;
    }
    std::ofstream f(file_path_, std::ios::trunc);
    if (!f.is_open()) {
        ARX_LOG_ERROR("CodeEditor: no se pudo escribir '{}'", file_path_);
        return false;
    }
    f << buffer_;
    f.close();
    dirty_ = false;
    ARX_LOG_INFO("CodeEditor: guardado '{}'", file_path_);
    // El hot reload de la VM detectará el cambio de mtime automáticamente
    return true;
}

// ==============================================================================
void CodeEditorDock::set_project_dir(const std::string& d) {
    project_dir_ = d;
    // Auto-cargar main.zen si existe y no hay archivo cargado
    if (file_path_.empty() && !d.empty()) {
        std::string main_zen = d + "/main.zen";
        std::ifstream f(main_zen);
        if (f.is_open()) {
            f.close();
            load_file(main_zen);
        }
    }
}

} // namespace arx
