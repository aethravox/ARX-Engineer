// ==============================================================================
// src/editor/gui/inspector_dock.cpp — Inspector de atributos.
//
// ACTUALIZADO v2: atributos específicos por tipo de Vox.
//   - Vox: visible, process, children
//   - Vox2D: position, rotation, scale
//   - Vox3D: position, rotation, scale
//   - VoxSprite2D: color, texture path
//   - VoxCamera2D: zoom, limit
//   - VoxCamera3D: fov, near, far, projection (perspective/orthogonal)
//   - VoxMeshInstance3D: mesh path, material color
//   - VoxRigidBody3D: mass, friction, restitution, linear_velocity, angular_velocity, gravity_scale, sleeping
//   - VoxStaticBody3D: friction, restitution
//   - VoxCharacterBody3D: up_direction, floor_max_angle, walk_speed
//   - VoxArea3D: monitoring, collide_with_bodies, collide_with_areas
//   - VoxRayCast3D: target_position, enabled, collision_mask
//   - VoxCollisionShape3D: shape (box/sphere/capsule), half_extents, radius, height
// ==============================================================================
#include "inspector_dock.hpp"
#include "core/logging.hpp"

#include <imgui.h>
#include <cstdio>

// Vox types
#include "scene/vox.hpp"
#include "scene/2d/vox2d.hpp"
#include "scene/3d/vox3d.hpp"
#include "scene/2d/vox_sprite_2d.hpp"
#include "scene/2d/vox_camera_2d.hpp"
#include "scene/3d/vox_mesh_instance_3d.hpp"
#include "scene/3d/vox_physics_3d.hpp"

namespace arx {

void InspectorDock::render() {
    if (!ImGui::Begin("Inspector")) { ImGui::End(); return; }

    if (!target_) {
        ImGui::TextDisabled("Selecciona un Vox para ver sus atributos");
        ImGui::End();
        return;
    }

    // Cabecera: tipo + nombre
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Atributos");
    ImGui::PopFont();
    ImGui::Separator();

    // Nombre editable
    char name_buf[256];
    std::snprintf(name_buf, sizeof(name_buf), "%s", target_->get_name().c_str());
    ImGui::TextDisabled("Nombre:");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##name", name_buf, sizeof(name_buf), ImGuiInputTextFlags_EnterReturnsTrue)) {
        target_->set_name(name_buf);
    }
    ImGui::PopItemWidth();

    // Tipo (read-only)
    std::string cls = std::string(target_->get_class_name());
    ImGui::TextDisabled("Tipo: %s", cls.c_str());
    ImGui::Separator();

    // Atributos comunes (todo Vox tiene)
    draw_common_attributes(target_);

    // Atributos específicos por tipo
    if (cls == "Vox2D" || cls == "VoxSprite2D" || cls == "VoxCamera2D" ||
        cls == "VoxRigidBody2D" || cls == "VoxStaticBody2D" || cls == "VoxCharacterBody2D") {
        auto* n2d = dynamic_cast<Vox2D*>(target_);
        if (n2d) draw_2d_attributes(n2d);
    }
    if (cls == "Vox3D" || cls == "VoxMeshInstance3D" || cls == "VoxCamera3D" ||
        cls == "VoxRigidBody3D" || cls == "VoxStaticBody3D" || cls == "VoxCharacterBody3D" ||
        cls == "VoxArea3D" || cls == "VoxRayCast3D" || cls == "VoxCollisionShape3D") {
        auto* n3d = dynamic_cast<Vox3D*>(target_);
        if (n3d) draw_3d_attributes(n3d);
    }

    // Tipos específicos
    if (cls == "VoxSprite2D") {
        auto* s = dynamic_cast<VoxSprite2D*>(target_);
        if (s) draw_sprite2d_attributes(s);
    }
    if (cls == "VoxCamera3D") {
        auto* c = dynamic_cast<VoxCamera3D*>(target_);
        if (c) draw_camera3d_attributes(c);
    }
    if (cls == "VoxRigidBody3D") {
        auto* r = dynamic_cast<VoxRigidBody3D*>(target_);
        if (r) draw_rigidbody3d_attributes(r);
    }
    if (cls == "VoxStaticBody3D") {
        auto* r = dynamic_cast<VoxStaticBody3D*>(target_);
        if (r) draw_staticbody3d_attributes(r);
    }
    if (cls == "VoxCharacterBody3D") {
        auto* r = dynamic_cast<VoxCharacterBody3D*>(target_);
        if (r) draw_characterbody3d_attributes(r);
    }
    if (cls == "VoxArea3D") {
        auto* r = dynamic_cast<VoxArea3D*>(target_);
        if (r) draw_area3d_attributes(r);
    }
    if (cls == "VoxRayCast3D") {
        auto* r = dynamic_cast<VoxRayCast3D*>(target_);
        if (r) draw_raycast3d_attributes(r);
    }
    if (cls == "VoxCollisionShape3D") {
        auto* r = dynamic_cast<VoxCollisionShape3D*>(target_);
        if (r) draw_collision_shape3d_attributes(r);
    }
    if (cls == "VoxMeshInstance3D") {
        auto* m = dynamic_cast<VoxMeshInstance3D*>(target_);
        if (m) draw_mesh_instance3d_attributes(m);
    }
    if (cls == "VoxCamera2D") {
        auto* c = dynamic_cast<VoxCamera2D*>(target_);
        if (c) draw_camera2d_attributes(c);
    }

    ImGui::End();
}

// ===== Atributos comunes a todo Vox =====
void InspectorDock::draw_common_attributes(Vox* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("General");
    ImGui::PopFont();

    bool visible = n->is_visible();
    if (ImGui::Checkbox("Visible", &visible)) {
        n->set_visible(visible);
    }

    bool processing = n->is_processing();
    if (ImGui::Checkbox("Process", &processing)) {
        // n->set_process(processing); // TODO: existe este método?
    }

    ImGui::TextDisabled("Hijos: %d", n->get_child_count());

    ImGui::Separator();
}

// ===== Atributos 2D =====
void InspectorDock::draw_2d_attributes(Vox2D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Transform 2D");
    ImGui::PopFont();

    auto pos = n->get_position();
    float pos_arr[2] = { pos.x, pos.y };
    if (ImGui::InputFloat2("Posicion", pos_arr, "%.1f")) {
        n->set_position(Vector2(pos_arr[0], pos_arr[1]));
    }

    float rot = n->get_rotation();
    if (ImGui::InputFloat("Rotacion", &rot, 0.0f, 0.0f, "%.1f")) {
        n->set_rotation(rot);
    }

    auto scale = n->get_scale();
    float scale_arr[2] = { scale.x, scale.y };
    if (ImGui::InputFloat2("Escala", scale_arr, "%.2f")) {
        n->set_scale(Vector2(scale_arr[0], scale_arr[1]));
    }

    ImGui::Separator();
}

// ===== Atributos 3D =====
void InspectorDock::draw_3d_attributes(Vox3D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Transform 3D");
    ImGui::PopFont();

    auto pos = n->get_position();
    float pos_arr[3] = { pos.x, pos.y, pos.z };
    if (ImGui::InputFloat3("Posicion", pos_arr, "%.1f")) {
        n->set_position(Vector3(pos_arr[0], pos_arr[1], pos_arr[2]));
    }

    auto rot = n->get_rotation();
    float rot_arr[3] = { rot.x, rot.y, rot.z };
    if (ImGui::InputFloat3("Rotacion", rot_arr, "%.1f")) {
        n->set_rotation(Vector3(rot_arr[0], rot_arr[1], rot_arr[2]));
    }

    auto scale = n->get_scale();
    float scale_arr[3] = { scale.x, scale.y, scale.z };
    if (ImGui::InputFloat3("Escala", scale_arr, "%.2f")) {
        n->set_scale(Vector3(scale_arr[0], scale_arr[1], scale_arr[2]));
    }

    ImGui::Separator();
}

// ===== VoxSprite2D =====
void InspectorDock::draw_sprite2d_attributes(VoxSprite2D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Sprite 2D");
    ImGui::PopFont();

    // Color modulate
    static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    ImGui::ColorEdit4("Color", color);

    // Texture path
    static char tex_path[256] = "";
    ImGui::InputText("Textura", tex_path, sizeof(tex_path));

    // Flip
    static bool flip_h = false, flip_v = false;
    ImGui::Checkbox("Flip H", &flip_h);
    ImGui::SameLine();
    ImGui::Checkbox("Flip V", &flip_v);

    ImGui::Separator();
}

// ===== VoxCamera2D =====
void InspectorDock::draw_camera2d_attributes(VoxCamera2D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Camara 2D");
    ImGui::PopFont();

    static float zoom = 1.0f;
    ImGui::SliderFloat("Zoom", &zoom, 0.1f, 10.0f);

    static bool enabled = true;
    ImGui::Checkbox("Enabled", &enabled);

    ImGui::Separator();
}

// ===== VoxCamera3D =====
void InspectorDock::draw_camera3d_attributes(VoxCamera3D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Camara 3D");
    ImGui::PopFont();

    static float fov = 60.0f;
    ImGui::SliderFloat("FOV", &fov, 10.0f, 170.0f, "%.0f grados");

    static float near_plane = 0.1f;
    ImGui::InputFloat("Near", &near_plane, 0.0f, 0.0f, "%.2f");

    static float far_plane = 200.0f;
    ImGui::InputFloat("Far", &far_plane, 0.0f, 0.0f, "%.1f");

    static int projection = 0;  // 0=perspective, 1=orthogonal
    const char* proj_names[] = {"Perspectiva", "Ortogonal"};
    ImGui::Combo("Proyeccion", &projection, proj_names, 2);

    ImGui::Separator();
}

// ===== VoxRigidBody3D =====
void InspectorDock::draw_rigidbody3d_attributes(VoxRigidBody3D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Fisica (RigidBody3D)");
    ImGui::PopFont();

    float mass = n->get_mass();
    if (ImGui::InputFloat("Masa", &mass, 0.0f, 0.0f, "%.2f")) {
        n->set_mass(mass);
    }

    float friction = n->get_friction();
    if (ImGui::InputFloat("Friccion", &friction, 0.0f, 0.0f, "%.2f")) {
        n->set_friction(friction);
    }

    float restitution = n->get_restitution();
    if (ImGui::InputFloat("Rebote", &restitution, 0.0f, 0.0f, "%.2f")) {
        n->set_restitution(restitution);
    }

    auto he = n->half_extents_;
    float he_arr[3] = { he.x, he.y, he.z };
    if (ImGui::InputFloat3("Half Extents", he_arr, "%.2f")) {
        n->half_extents_ = Vector3(he_arr[0], he_arr[1], he_arr[2]);
    }

    auto vel = n->get_linear_velocity();
    ImGui::TextDisabled("Velocidad: (%.1f, %.1f, %.1f)", vel.x, vel.y, vel.z);

    static float gravity_scale = 1.0f;
    ImGui::SliderFloat("Gravity Scale", &gravity_scale, -2.0f, 2.0f);

    static bool can_sleep = true;
    ImGui::Checkbox("Can Sleep", &can_sleep);

    static int collision_layer = 1;
    ImGui::InputInt("Collision Layer", &collision_layer);

    static int collision_mask = 0xFFFFFFFF;
    ImGui::InputInt("Collision Mask", &collision_mask);

    ImGui::Separator();
}

// ===== VoxStaticBody3D =====
void InspectorDock::draw_staticbody3d_attributes(VoxStaticBody3D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Fisica (StaticBody3D)");
    ImGui::PopFont();

    float friction = n->get_friction();
    if (ImGui::InputFloat("Friccion", &friction, 0.0f, 0.0f, "%.2f")) {
        n->set_friction(friction);
    }

    float restitution = n->get_restitution();
    if (ImGui::InputFloat("Rebote", &restitution, 0.0f, 0.0f, "%.2f")) {
        n->set_restitution(restitution);
    }

    auto he = n->half_extents_;
    float he_arr[3] = { he.x, he.y, he.z };
    if (ImGui::InputFloat3("Half Extents", he_arr, "%.2f")) {
        n->half_extents_ = Vector3(he_arr[0], he_arr[1], he_arr[2]);
    }

    static int collision_layer = 1;
    ImGui::InputInt("Collision Layer", &collision_layer);

    ImGui::Separator();
}

// ===== VoxCharacterBody3D =====
void InspectorDock::draw_characterbody3d_attributes(VoxCharacterBody3D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Fisica (CharacterBody3D)");
    ImGui::PopFont();

    auto up = n->get_up_direction();
    float up_arr[3] = { up.x, up.y, up.z };
    if (ImGui::InputFloat3("Up Direction", up_arr, "%.1f")) {
        n->set_up_direction(Vector3(up_arr[0], up_arr[1], up_arr[2]));
    }

    float floor_angle = 0.7853982f;  // default 45 grados (no hay getter)
    if (ImGui::SliderFloat("Floor Max Angle", &floor_angle, 0.0f, 1.57f, "%.2f rad")) {
        n->set_floor_max_angle(floor_angle);
    }

    auto he = n->half_extents_;
    float he_arr[3] = { he.x, he.y, he.z };
    if (ImGui::InputFloat3("Half Extents", he_arr, "%.2f")) {
        n->half_extents_ = Vector3(he_arr[0], he_arr[1], he_arr[2]);
    }

    static float walk_speed = 5.0f;
    ImGui::SliderFloat("Walk Speed", &walk_speed, 0.1f, 20.0f);

    static float jump_force = 5.0f;
    ImGui::SliderFloat("Jump Force", &jump_force, 0.0f, 20.0f);

    ImGui::Separator();
}

// ===== VoxArea3D =====
void InspectorDock::draw_area3d_attributes(VoxArea3D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("Area3D");
    ImGui::PopFont();

    bool monitoring = n->is_monitoring();
    if (ImGui::Checkbox("Monitoring", &monitoring)) {
        n->set_monitoring(monitoring);
    }

    auto he = n->half_extents_;
    float he_arr[3] = { he.x, he.y, he.z };
    if (ImGui::InputFloat3("Half Extents", he_arr, "%.2f")) {
        n->half_extents_ = Vector3(he_arr[0], he_arr[1], he_arr[2]);
    }

    static int collision_layer = 1;
    ImGui::InputInt("Collision Layer", &collision_layer);

    static int collision_mask = 0xFFFFFFFF;
    ImGui::InputInt("Collision Mask", &collision_mask);

    ImGui::Separator();
}

// ===== VoxRayCast3D =====
void InspectorDock::draw_raycast3d_attributes(VoxRayCast3D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("RayCast3D");
    ImGui::PopFont();

    auto target = n->get_target_position();
    float target_arr[3] = { target.x, target.y, target.z };
    if (ImGui::InputFloat3("Target Position", target_arr, "%.1f")) {
        n->set_target_position(Vector3(target_arr[0], target_arr[1], target_arr[2]));
    }

    bool enabled = n->is_enabled();
    if (ImGui::Checkbox("Enabled", &enabled)) {
        n->set_enabled(enabled);
    }

    static uint32_t mask = 0xFFFFFFFF;
    ImGui::InputInt("Collision Mask", (int*)&mask);

    ImGui::TextDisabled("Colliding: %s", n->is_colliding() ? "SI" : "NO");
    if (n->is_colliding()) {
        auto pt = n->get_collision_point();
        ImGui::TextDisabled("Point: (%.1f, %.1f, %.1f)", pt.x, pt.y, pt.z);
    }

    ImGui::Separator();
}

// ===== VoxCollisionShape3D =====
void InspectorDock::draw_collision_shape3d_attributes(VoxCollisionShape3D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("CollisionShape3D");
    ImGui::PopFont();

    int shape = (int)n->get_shape();
    const char* shape_names[] = {"Box", "Sphere", "Capsule", "Cylinder", "Convex", "Concave"};
    if (ImGui::Combo("Shape", &shape, shape_names, 6)) {
        n->set_shape((VoxCollisionShape3D::Shape)shape);
    }

    if (shape == 0) {  // Box
        auto he = n->get_half_extents();
        float he_arr[3] = { he.x, he.y, he.z };
        if (ImGui::InputFloat3("Half Extents", he_arr, "%.2f")) {
            n->set_half_extents(Vector3(he_arr[0], he_arr[1], he_arr[2]));
        }
    } else if (shape == 1) {  // Sphere
        float r = n->get_radius();
        if (ImGui::InputFloat("Radius", &r, 0.0f, 0.0f, "%.2f")) {
            n->set_radius(r);
        }
    } else if (shape == 2 || shape == 3) {  // Capsule/Cylinder
        float r = n->get_radius();
        float h = n->get_height();
        if (ImGui::InputFloat("Radius", &r, 0.0f, 0.0f, "%.2f")) {
            n->set_radius(r);
        }
        if (ImGui::InputFloat("Height", &h, 0.0f, 0.0f, "%.2f")) {
            n->set_height(h);
        }
    }

    ImGui::Separator();
}

// ===== VoxMeshInstance3D =====
void InspectorDock::draw_mesh_instance3d_attributes(VoxMeshInstance3D* n) {
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts.Size > 1 ? ImGui::GetIO().Fonts->Fonts[1] : nullptr);
    ImGui::Text("MeshInstance3D");
    ImGui::PopFont();

    static char mesh_path[256] = "";
    ImGui::InputText("Mesh Path", mesh_path, sizeof(mesh_path));

    static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    ImGui::ColorEdit4("Material Color", color);

    static bool cast_shadow = true;
    ImGui::Checkbox("Cast Shadow", &cast_shadow);

    static bool receive_shadow = true;
    ImGui::Checkbox("Receive Shadow", &receive_shadow);

    ImGui::Separator();
}

} // namespace arx
