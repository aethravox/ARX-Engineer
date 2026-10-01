// ==============================================================================
// src/editor/gui/viewport.hpp — Viewport del editor (2D y 3D).
//
// ACTUALIZADO v3: ahora soporta Gizmos 3D (Move/Rotate/Scale) sobre el Vox
// seleccionado. Hotkeys W/E/R cambian de modo.
// ==============================================================================
#pragma once

#include "render/renderer.hpp"
#include "core/types.hpp"

#include <imgui.h>
#include <string>
// Forward declaration de glm::mat4 (es un typedef, no se puede forward-declarar
// como class, así que incluimos el header completo).
#include <glm/glm.hpp>

namespace arx {

class Vox;
class Vox2D;
class Vox3D;

class ViewportPanel {
public:
    explicit ViewportPanel(Renderer* r) : renderer_(r) {}
    ~ViewportPanel() = default;

    void render(float delta);

    void set_mode_2d() { mode_2d_ = true; }
    void set_mode_3d() { mode_2d_ = false; }
    void set_root(Vox* root) { root_ = root; }
    void set_selected(Vox* v) { selected_ = v; }
    Vox* get_selected() const { return selected_; }

    // Modo del gizmo (cambiable con W/E/R)
    enum class GizmoMode {
        Move    = 0,
        Rotate  = 1,
        Scale   = 2,
    };
    GizmoMode gizmo_mode() const { return gizmo_mode_; }
    void set_gizmo_mode(GizmoMode m) { gizmo_mode_ = m; }

private:
    void render_toolbar();
    void render_2d_(ImDrawList* dl, ImVec2 p0, ImVec2 p1, ImVec2 size);
    void render_3d_(ImDrawList* dl, ImVec2 p0, ImVec2 p1, ImVec2 size);
    void draw_vox_2d_(Vox2D* n, ImDrawList* dl, ImVec2 origin, float scale);
    void draw_vox_3d_real_(Vox3D* n, ImDrawList* dl, ImVec2 sp, float ss,
                            ImU32 color, const std::string& name);
    // Compat con la firma vieja (no-op, evita link errors)
    void draw_vox_3d_(Vox3D* n, ImDrawList* dl, float cx, float horizon, float scale);

    void handle_input_();
    void handle_input_(ImVec2 p0, ImVec2 p1, ImVec2 size);
    Vox* pick_vox_at_(ImVec2 mouse, ImVec2 origin, ImVec2 size);

    // === Gizmo 3D ===
    // Dibuja el gizmo (3 ejes) sobre el Vox seleccionado. Devuelve el eje
    // que está bajo el mouse (-1 si ninguno: 0=X, 1=Y, 2=Z).
    int  draw_gizmo_3d_(ImDrawList* dl, ImVec2 origin, ImVec2 size,
                         const glm::mat4& vp);
    // Procesa clicks/drags sobre el gizmo. Devuelve true si el evento fue
    // consumido por el gizmo (para no seleccionar voxes debajo).
    bool handle_gizmo_input_(ImVec2 mouse, ImVec2 origin, ImVec2 size,
                              const glm::mat4& vp);

    Renderer* renderer_;
    Vox*      root_          = nullptr;
    Vox*      selected_      = nullptr;
    bool      mode_2d_       = false;
    bool      playing_       = false;

    Vector2   pan_offset_    = {0, 0};
    float     zoom_          = 1.0f;
    float     cam_distance_  = 10.0f;
    Vector2   cam_yaw_pitch_ = {0.4f, 0.5f};
    Vector3   cam_target_    = {0, 0, 0};

    // Gizmo state
    GizmoMode gizmo_mode_        = GizmoMode::Move;
    int       gizmo_hover_axis_  = -1;   // 0=X, 1=Y, 2=Z, -1=ninguno
    int       gizmo_active_axis_ = -1;   // eje que se está arrastrando
    bool      gizmo_dragging_    = false;
    // Posición inicial del Vox y del mouse al empezar el drag (para calcular delta)
    Vector3   drag_start_vox_pos_{0, 0, 0};
    Vector3   drag_start_vox_rot_{0, 0, 0};
    Vector3   drag_start_vox_scl_{1, 1, 1};
    ImVec2    drag_start_mouse_{0, 0};
};

} // namespace arx
