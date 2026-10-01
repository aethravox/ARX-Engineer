// ==============================================================================
// src/editor/gui/code_editor_dock.hpp — Editor de código Zen integrado.
//
// Permite editar main.zen (y otros .zen) dentro del editor ARX sin tener
// que abrir VSCode. Syntax highlighting básico para Zen lang.
//
// Features:
//   - Cargar/guardar archivos .zen
//   - Syntax highlighting: keywords ES/EN, strings, comentarios, números
//   - Ctrl+S para guardar
//   - Hot reload automático (la VM detecta el cambio de mtime de main.zen)
//   - Line numbers
//   - Auto-carga main.zen al abrir proyecto
// ==============================================================================
#pragma once

#include <string>

namespace arx {

class CodeEditorDock {
public:
    void render();

    // Cargar un archivo .zen en el editor
    bool load_file(const std::string& path);

    // Guardar el archivo actual
    bool save_file();

    // Setter del directorio del proyecto (para auto-cargar main.zen)
    void set_project_dir(const std::string& d);

    // ¿Hay archivo cargado?
    bool has_file() const { return !file_path_.empty(); }
    const std::string& file_path() const { return file_path_; }
    bool is_dirty() const { return dirty_; }

private:
    void render_menu_bar_();
    void render_editor_();
    void apply_syntax_highlighting_();

    std::string project_dir_;
    std::string file_path_;        // path del archivo cargado
    std::string buffer_;           // contenido del archivo
    bool        dirty_ = false;    // ¿cambios sin guardar?
    float       font_scale_ = 1.0f;
    bool        show_line_numbers_ = true;
};

} // namespace arx
