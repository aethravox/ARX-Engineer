// ==============================================================================
// src/editor/gui/scene_tree_dock.cpp — Árbol de Voxes editable.
//
// ACTUALIZADO v2: usa iconos SVG (vía IconManager + nanosvg) en lugar de emojis.
// ==============================================================================
#include "scene_tree_dock.hpp"
#include "icon_manager.hpp"
#include "core/logging.hpp"

#include <imgui.h>

// Vox types
#include "scene/vox.hpp"
#include "scene/2d/vox2d.hpp"
#include "scene/3d/vox3d.hpp"
#include "scene/2d/vox_sprite_2d.hpp"
#include "scene/2d/vox_camera_2d.hpp"
#include "scene/3d/vox_mesh_instance_3d.hpp"  // también define VoxCamera3D
#include "scene/3d/vox_physics_3d.hpp"
#include "scene/audio/vox_audio.hpp"
#include "scene/animation/animation_player.hpp"
#include "scene/animation/tween.hpp"
#include "scene/tilemap/tilemap.hpp"
#include "scene/particles/particles.hpp"
#include "scene/3d/vox_extra_3d.hpp"
#include "navigation/navigation.hpp"
#include "scene/net/vox_multiplayer.hpp"

namespace arx {
// ------------------------------------------------------------------------------
bool SceneTreeDock::is_vox_in_tree(Vox* v) const {
    if (!v || !root_) return false;
    if (v == root_) return true;
    // Buscar recursivamente en todo el arbol.
    std::function<bool(Vox*)> visit = [&](Vox* n) -> bool {
        if (n == v) return true;
        for (auto* c : n->get_children()) {
            if (visit(c)) return true;
        }
        return false;
    };
    return visit(root_);
}

// Lista de tipos disponibles en el popup "Add Vox".
// El campo `icon` ahora es el nombre del archivo SVG (sin extensión) en
// engine/src/editor/icons/voxes/.
const std::vector<SceneTreeDock::VoxType> SceneTreeDock::kVoxTypes = {
    {"Vox",                  "Vox",                  "vox"},
    {"Vox2D",                "Vox2D",                "vox2d"},
    {"Vox3D",                "Vox3D",                "vox3d"},
    {"VoxSprite2D",          "VoxSprite2D",          "vox_sprite_2d"},
    {"VoxCamera2D",          "VoxCamera2D",          "vox_camera_2d"},
    {"VoxMeshInstance3D",    "VoxMeshInstance3D",    "vox_mesh_3d"},
    {"VoxCamera3D",          "VoxCamera3D",          "vox_camera_3d"},
    {"VoxRigidBody3D",       "VoxRigidBody3D",       "vox_rigid_body_3d"},
    {"VoxStaticBody3D",      "VoxStaticBody3D",      "vox_static_body_3d"},
    {"VoxCharacterBody3D",   "VoxCharacterBody3D",   "vox_character_body_3d"},
    {"VoxArea3D",            "VoxArea3D",            "vox_area_3d"},
    {"VoxRayCast3D",         "VoxRayCast3D",         "vox_ray_cast_3d"},
    {"VoxCollisionShape3D",  "VoxCollisionShape3D",  "vox_collision_shape_3d"},
};

// Helper: nombre del icono SVG dado el class_name de un Vox.
static const char* icon_name_for_class(const std::string& cls) {
    if (cls == "VoxSprite2D")            return "vox_sprite_2d";
    if (cls == "VoxCamera2D")            return "vox_camera_2d";
    if (cls == "VoxCamera3D")            return "vox_camera_3d";
    if (cls == "VoxMeshInstance3D")      return "vox_mesh_3d";
    if (cls == "VoxRigidBody3D")         return "vox_rigid_body_3d";
    if (cls == "VoxStaticBody3D")        return "vox_static_body_3d";
    if (cls == "VoxCharacterBody3D")     return "vox_character_body_3d";
    if (cls == "VoxArea3D")              return "vox_area_3d";
    if (cls == "VoxRayCast3D")           return "vox_ray_cast_3d";
    if (cls == "VoxCollisionShape3D")    return "vox_collision_shape_3d";
    if (cls == "VoxAudioStreamPlayer") return "vox_audio";
    if (cls == "VoxAnimationPlayer") return "vox_animation";
    if (cls == "Tween") return "vox_tween";
    if (cls == "VoxTileMap") return "vox_tilemap";
    if (cls == "VoxParticles2D") return "vox_particles";
    if (cls == "VoxParticles3D") return "vox_particles";
    if (cls == "VoxDirectionalLight3D") return "vox_light_directional";
    if (cls == "VoxOmniLight3D") return "vox_light_omni";
    if (cls == "VoxLight3D") return "vox_light_directional";
    if (cls == "VoxSprite3D") return "vox_sprite3d";
    if (cls == "NavigationAgent3D") return "vox_navigation";
    if (cls == "VoxMultiplayerSpawner") return "vox_network";
    if (cls == "VoxMultiplayerSynchronizer") return "vox_network";
    if (cls == "Vox3D")                  return "vox3d";
    if (cls == "Vox2D")                  return "vox2d";
    return "vox"; // default para Vox y tipos desconocidos
}

// Helper: dibujar icono + texto en una línea de ImGui.
// icon_size_px = tamaño en píxeles (típicamente 16 o 18).
static void icon_with_label(const char* icon_name, const char* label,
                             int icon_size_px = 16, float spacing = 6.0f) {
    ImTextureID tex = IconManager::get().imgui_texture(icon_name);
    if (tex != 0) {
        ImGui::Image(tex, ImVec2((float)icon_size_px, (float)icon_size_px));
        ImGui::SameLine(0, spacing);
    }
    ImGui::TextUnformatted(label);
}

// ------------------------------------------------------------------------------
void SceneTreeDock::ensure_root() {
    if (!root_) {
        root_ = new Vox();
        root_->set_name("root");
        ARX_LOG_INFO("SceneTreeDock: root por defecto creado");

        // === Voxes de demo para que el viewport 3D no esté vacío ===
        // (Solo si no hay escena cargada. Cuando se cargue una escena real,
        //  estos Voxes se reemplazarán.)
        auto* demo1 = new Vox3D();
        demo1->set_name("DemoVox3D");
        demo1->set_position(Vector3(-2, 1, 0));
        root_->add_child(demo1);

        auto* demo2 = new VoxCamera3D();
        demo2->set_name("MainCamera");
        demo2->set_position(Vector3(2, 1, 0));
        root_->add_child(demo2);

        auto* demo3 = new VoxRigidBody3D();
        demo3->set_name("FallingBox");
        demo3->set_position(Vector3(0, 3, 0));
        root_->add_child(demo3);

        ARX_LOG_INFO("SceneTreeDock: 3 Voxes de demo creados (Vox3D, Camera3D, RigidBody3D)");
    }
}

// ------------------------------------------------------------------------------
Vox* SceneTreeDock::create_vox_by_type(const std::string& type_name,
                                       const std::string& name) {
    Vox* v = nullptr;

    if      (type_name == "Vox")                   v = new Vox();
    else if (type_name == "Vox2D")                 v = new Vox2D();
    else if (type_name == "Vox3D")                 v = new Vox3D();
    else if (type_name == "VoxSprite2D")           v = new VoxSprite2D();
    else if (type_name == "VoxCamera2D")           v = new VoxCamera2D();
    else if (type_name == "VoxMeshInstance3D")     v = new VoxMeshInstance3D();
    else if (type_name == "VoxCamera3D")           v = new VoxCamera3D();
    else if (type_name == "VoxRigidBody3D")        v = new VoxRigidBody3D();
    else if (type_name == "VoxStaticBody3D")       v = new VoxStaticBody3D();
    else if (type_name == "VoxCharacterBody3D")    v = new VoxCharacterBody3D();
    else if (type_name == "VoxArea3D")             v = new VoxArea3D();
    else if (type_name == "VoxRayCast3D")          v = new VoxRayCast3D();
    else if (type_name == "VoxCollisionShape3D")   v = new VoxCollisionShape3D();
    else if (type_name == "VoxAudioStreamPlayer")  v = new VoxAudioStreamPlayer();
    else if (type_name == "VoxAnimationPlayer")  v = new VoxAnimationPlayer();
    else if (type_name == "Tween")              v = new Tween();
    else if (type_name == "VoxTileMap")          v = new VoxTileMap();
    else if (type_name == "VoxParticles2D")     v = new VoxParticles2D();
    else if (type_name == "VoxParticles3D")     v = new VoxParticles3D();
    else if (type_name == "VoxDirectionalLight3D") v = new VoxDirectionalLight3D();
    else if (type_name == "VoxOmniLight3D")      v = new VoxOmniLight3D();
    else if (type_name == "VoxSprite3D")         v = new VoxSprite3D();
    else if (type_name == "NavigationAgent3D")  v = new NavigationAgent3D();
    else if (type_name == "VoxMultiplayerSpawner") v = new VoxMultiplayerSpawner();
    else if (type_name == "VoxMultiplayerSynchronizer") v = new VoxMultiplayerSynchronizer();
    else {
        ARX_LOG_WARN("SceneTreeDock: tipo desconocido '{}'", type_name);
        return nullptr;
    }

    v->set_name(name.empty() ? type_name : name);

    // Defaults razonables por tipo (para que se vean al instante en el viewport)
    if (type_name == "Vox3D" || type_name == "VoxMeshInstance3D" ||
        type_name == "VoxRigidBody3D" || type_name == "VoxStaticBody3D" ||
        type_name == "VoxCharacterBody3D" || type_name == "VoxArea3D") {
        auto* n3d = dynamic_cast<Vox3D*>(v);
        if (n3d) n3d->set_position(Vector3(0, 1, 0));
    }
    if (type_name == "VoxRigidBody3D") {
        auto* rb = dynamic_cast<VoxRigidBody3D*>(v);
        if (rb) rb->half_extents_ = Vector3(0.5f, 0.5f, 0.5f);
    }

    ARX_LOG_INFO("SceneTreeDock: creado Vox tipo '{}' nombre '{}'", type_name, v->get_name());
    return v;
}

// ------------------------------------------------------------------------------
void SceneTreeDock::render() {
    if (!ImGui::Begin("Vox Tree")) { ImGui::End(); return; }

    // ===== Toolbar =====
    if (ImGui::Button("+ Add Vox")) {
        ensure_root();
        show_add_popup_ = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Rename")) {
        if (selected_ && selected_ != root_) {
            renaming_ = true;
            renaming_vox_ = selected_;
            std::snprintf(rename_buf_, sizeof(rename_buf_), "%s", selected_->get_name().c_str());
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete")) {
        if (selected_ && selected_ != root_) {
            Vox* parent = selected_->get_parent();
            if (parent) {
                parent->remove_child(selected_);
                delete selected_;
                selected_ = nullptr;
            }
        }
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(%d)", root_ ? root_->get_child_count() : 0);

    ImGui::Separator();

    if (root_) {
        render_node_tree(root_);
    } else {
        ImGui::TextDisabled("No hay escena. Pulsa '+ Add Vox' para empezar.");
    }

    render_add_vox_popup();

    // ===== Popup Rename =====
    if (renaming_ && renaming_vox_) {
        ImGui::OpenPopup("###RenameVox");
        renaming_ = false;
    }
    if (ImGui::BeginPopup("###RenameVox")) {
        ImGui::TextDisabled("Renombrar Vox:");
        ImGui::InputText("##rename", rename_buf_, sizeof(rename_buf_));
        if (ImGui::Button("OK", ImVec2(80, 0)) || ImGui::IsKeyPressed(ImGuiKey_Enter)) {
            renaming_vox_->set_name(rename_buf_);
            renaming_vox_ = nullptr;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(80, 0))) {
            renaming_vox_ = nullptr;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}

// ------------------------------------------------------------------------------
void SceneTreeDock::render_node_tree(Vox* n) {
    if (!n) return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                ImGuiTreeNodeFlags_SpanAvailWidth;

    bool is_leaf = (n->get_child_count() == 0);
    if (is_leaf) flags |= ImGuiTreeNodeFlags_Leaf;
    if (selected_ == n) flags |= ImGuiTreeNodeFlags_Selected;

    // Determinar el nombre del icono según el tipo de Vox
    const char* icon_name = "vox";
    if (n == root_) {
        icon_name = "vox"; // root usa el genérico
    } else {
        icon_name = icon_name_for_class(std::string(n->get_class_name()));
    }

    // Dibujar el TreeNode con el icono SVG como parte del label.
    // Truco: usar ImGui::Image antes de TreeNodeEx no funciona bien con el
    // espaciado. En su lugar, hacemos el TreeNode con texto vacío y luego
    // dibujamos icono + nombre en el mismo espacio usando SetCursorPosX.
    // Alternativa más simple: incluir el icono como parte del texto no funciona
    // porque queremos la imagen. Por eso usamos un enfoque de "image inline":
    //
    //ImGuiTreeNodeFlags_DrawLinesNone  — no disponible en ImGui docking branch
    //
    // Solución final: TreeNodeEx con label de texto, y ponemos el icono
    // DENTRO del TreeNode usando un truco: llamar a TreeNodeEx con label vacío
    // y luego Image + Text en SameLine. Pero eso rompe el hit-test.
    //
    // La forma estándar (usada por Godot/Blender ImGui clones):
    // 1. Calcular ancho del icono + spacing
    // 2. TreeNodeEx con el nombre (texto)
    // 3. Si el TreeNode está renderizado, dibujar Image encima del texto con
    //    SetCursorScreenPos
    //
    // Como esto es complejo, usamos la forma más simple: TreeNode con texto
    // precedido por 2 espacios, y dibujamos el icono DESPUÉS del open arrow
    // usando SetCursorPosX en base al ancho del arrow.

    // Prefijar el label con 2 espacios para dejar sitio al icono SVG
    // (el icono se dibuja encima de esos espacios via ImDrawList)
    std::string label = std::string("  ") + n->get_name();
    bool open = ImGui::TreeNodeEx(label.c_str(), flags);

    // Dibujar icono SVG encima de los espacios en blanco del label.
    // GetItemRectMin() nos da la posición del TreeNode recién renderizado.
    if (true) {
        ImVec2 item_min = ImGui::GetItemRectMin();
        // Para tree nodes con hijos: el arrow ocupa ~16px + 4px de padding
        // Para leafs: el bullet ocupa ~12px
        float icon_x = item_min.x + (is_leaf ? 14.0f : 28.0f);
        float icon_y = item_min.y + (ImGui::GetItemRectSize().y - 16) * 0.5f;
        ImTextureID tex = IconManager::get().imgui_texture(icon_name);
        if (tex != 0) {
            ImGui::GetWindowDrawList()->AddImage(
                tex,
                ImVec2(icon_x, icon_y),
                ImVec2(icon_x + 16, icon_y + 16));
        }
    }

    if (ImGui::IsItemClicked()) selected_ = n;

    // Click derecho: menú contextual
    if (ImGui::BeginPopupContextItem(("###ctx" + label).c_str())) {
        if (ImGui::MenuItem("Add Child Vox")) {
            show_add_popup_ = true;
            selected_ = n;
        }
        if (n != root_) {
            if (ImGui::MenuItem("Rename")) {
                renaming_ = true;
                renaming_vox_ = n;
                std::snprintf(rename_buf_, sizeof(rename_buf_), "%s", n->get_name().c_str());
            }
            if (ImGui::MenuItem("Delete")) {
                Vox* parent = n->get_parent();
                if (parent) {
                    parent->remove_child(n);
                    delete n;
                    if (selected_ == n) selected_ = nullptr;
                }
            }
        }
        ImGui::EndPopup();
    }

    // Drag & drop
    if (ImGui::BeginDragDropSource()) {
        Vox* payload = n;
        ImGui::SetDragDropPayload("ARX_VOX", &payload, sizeof(payload));
        // En el tooltip de drag, usar icono + nombre
        icon_with_label(icon_name, n->get_name().c_str(), 16, 4.0f);
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget()) {
        if (auto* p = ImGui::AcceptDragDropPayload("ARX_VOX")) {
            Vox* dragged = *static_cast<Vox**>(p->Data);
            if (dragged && dragged != n && dragged != root_) {
                Vox* ancestor = n;
                while (ancestor) {
                    if (ancestor == dragged) { ancestor = nullptr; break; }
                    ancestor = ancestor->get_parent();
                }
                if (ancestor != nullptr || n == root_) {
                    Vox* old_parent = dragged->get_parent();
                    if (old_parent) old_parent->remove_child(dragged);
                    n->add_child(dragged);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (open) {
        for (auto* c : n->get_children()) render_node_tree(c);
        ImGui::TreePop();
    }
}

// ------------------------------------------------------------------------------
void SceneTreeDock::render_add_vox_popup() {
    if (show_add_popup_) {
        show_add_popup_ = false;
        ImGui::OpenPopup("###AddVoxPopup");
    }

    ImGui::SetNextWindowSize(ImVec2(380, 420), ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopup("###AddVoxPopup")) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
        ImGui::Text("➕ Añadir Vox");
        ImGui::PopFont();
        ImGui::Separator();

        // Nombre
        ImGui::TextDisabled("Nombre:");
        ImGui::SameLine();
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##voxname", name_buf_, sizeof(name_buf_));
        ImGui::PopItemWidth();

        // Lista de tipos con iconos SVG
        ImGui::TextDisabled("Tipo:");
        ImGui::BeginChild("###typelist", ImVec2(0, 220), true);
        for (int i = 0; i < (int)kVoxTypes.size(); ++i) {
            const auto& t = kVoxTypes[i];
            bool sel = (type_idx_ == i);
            // Prefijar el label con espacios para dejar sitio al icono
            char label[160];
            std::snprintf(label, sizeof(label), "   %s", t.name);
            if (ImGui::Selectable(label, sel)) {
                type_idx_ = i;
            }
            // Dibujar el icono encima de los espacios
            ImVec2 item_min = ImGui::GetItemRectMin();
            float icon_x = item_min.x + 4.0f;
            float icon_y = item_min.y + (ImGui::GetItemRectSize().y - 16) * 0.5f;
            ImTextureID tex = IconManager::get().imgui_texture(t.icon);
            if (tex != 0) {
                ImGui::GetWindowDrawList()->AddImage(
                    tex,
                    ImVec2(icon_x, icon_y),
                    ImVec2(icon_x + 16, icon_y + 16));
            }
        }
        ImGui::EndChild();

        ImGui::Separator();

        if (ImGui::Button("Crear", ImVec2(100, 0))) {
            Vox* parent = selected_ ? selected_ : root_;
            if (!parent) parent = root_;
            std::string nm = name_buf_;
            if (nm.empty()) nm = kVoxTypes[type_idx_].name;
            Vox* v = create_vox_by_type(kVoxTypes[type_idx_].class_name, nm);
            if (v && parent) {
                parent->add_child(v);
                selected_ = v;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancelar", ImVec2(100, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        {
            Vox* parent = selected_ ? selected_ : root_;
            ImGui::TextDisabled("Padre: %s", parent ? parent->get_name().c_str() : "(ninguno)");
        }

        ImGui::EndPopup();
    }
}

} // namespace arx
