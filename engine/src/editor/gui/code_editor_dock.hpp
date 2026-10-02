// ==============================================================================
// src/editor/gui/code_editor_dock.hpp — Editor de código Zen con tabs múltiples.
//
// Permite editar múltiples archivos .zen a la vez, con tabs para cambiar entre ellos.
// ==============================================================================
#pragma once

#include <string>
#include <vector>

namespace arx {

class CodeEditorDock {
public:
    void render();

    // Cargar un archivo .zen en el editor (agrega un tab nuevo)
    // Si ya está abierto, solo activa ese tab.
    bool load_file(const std::string& path);

    // Guardar el archivo del tab activo
    bool save_file();

    // Setter del directorio del proyecto (para auto-cargar main.zen)
    void set_project_dir(const std::string& d);

    // ¿Hay algún archivo abierto?
    bool has_file() const { return !open_files_.empty(); }
    const std::string& file_path() const;
    bool is_dirty() const;

private:
    void render_menu_bar_();
    void render_editor_();
    void apply_syntax_highlighting_();
    void render_tabs_();
    void close_tab_(int idx);

    struct OpenFile {
        std::string path;       // path completo del archivo
        std::string name;       // nombre corto (para el tab)
        std::string buffer;     // contenido del archivo
        bool dirty = false;     // ¿cambios sin guardar?
    };

    std::vector<OpenFile> open_files_;
    int active_tab_ = -1;   // índice en open_files_ del tab activo

    std::string project_dir_;
    float       font_scale_ = 1.0f;
    bool        show_line_numbers_ = true;
};

} // namespace arx
