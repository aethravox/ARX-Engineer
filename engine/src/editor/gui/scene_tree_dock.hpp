// ==============================================================================
// src/editor/gui/scene_tree_dock.hpp — Árbol de voxes editable.
//
// Permite:
//   - Add Vox: abre popup con tipos disponibles (Vox, Vox2D, Vox3D, etc.)
//   - Delete: elimina el Vox seleccionado
//   - Rename: edita el nombre inline
//   - Click: selecciona un Vox
//   - Drag & drop: reorganiza jerarquía
// ==============================================================================
#pragma once

#include "scene/vox.hpp"

#include <string>
#include <vector>

namespace arx {

class SceneTreeDock {
public:
    void render();

    void set_root(Vox* root) { root_ = root; }
    Vox* get_root() const { return root_; }
    Vox* get_selected() const { return selected_; }
    void set_selected(Vox* v) { selected_ = v; }

    // Verifica si un Vox sigue siendo parte del arbol (no fue borrado).
    bool is_vox_in_tree(Vox* v) const;

    // Crear un root por defecto si no hay escena cargada
    void ensure_root();

private:
    void render_node_tree(Vox* n);
    void render_add_vox_popup();
    Vox* create_vox_by_type(const std::string& type_name, const std::string& name);

    Vox*  root_     = nullptr;
    Vox*  selected_ = nullptr;

    // Add Vox popup state
    bool  show_add_popup_ = false;
    char  name_buf_[128]  = "NewVox";
    int   type_idx_       = 0;

    // Rename state
    bool  renaming_       = false;
    char  rename_buf_[128] = "";
    Vox*  renaming_vox_   = nullptr;

    // Tipos disponibles para Add Vox
    struct VoxType {
        const char* name;       // nombre visible
        const char* class_name; // nombre de la clase C++
        const char* icon;       // emoji o texto
    };
    static const std::vector<VoxType> kVoxTypes;
};

} // namespace arx
