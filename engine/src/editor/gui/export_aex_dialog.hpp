// ==============================================================================
// engine/src/editor/gui/export_aex_dialog.hpp
//
// Modal de ImGui que pide al usuario:
//   - Ruta de salida del .aex
//   - Ruta al archivo de clave privada del creador (opcional)
//
// Y al confirmar, instancia AexExporter y exporta el proyecto.
// Muestra logs en tiempo real.
// ==============================================================================

#pragma once

#include "export/export_plugin.hpp"

#include <string>
#include <vector>

namespace arx {

class ExportAexDialog {
public:
    ExportAexDialog() = default;
    ~ExportAexDialog() = default;

    // Abre el modal. Llama despues de haber seteado project_root_.
    void open(const std::string& project_root);
    void close();
    bool is_open() const { return open_; }

    // Renderiza el modal. Llama cada frame si is_open().
    // Devuelve true si se completo un export en este frame.
    bool render();

    // Resultado del ultimo export (para mostrar logs).
    const ExportResult& last_result() const { return last_result_; }

private:
    bool open_ = false;
    char out_path_buf_[1024]   = "build/output.aex";
    char key_path_buf_[1024]   = "";
    char app_name_buf_[128]    = "my_app";
    std::string project_root_;
    ExportResult last_result_;
    bool last_result_valid_ = false;
    bool show_success_modal_ = false;
};

} // namespace arx
