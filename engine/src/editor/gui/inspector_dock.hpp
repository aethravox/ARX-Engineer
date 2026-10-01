// ==============================================================================
// src/editor/gui/inspector_dock.hpp — Inspector de atributos.
//
// ACTUALIZADO v2: atributos específicos por tipo de Vox.
// ==============================================================================
#pragma once

#include "scene/vox.hpp"

namespace arx {

class InspectorDock {
public:
    void render();
    void inspect(Vox* n) { target_ = n; }
    Vox* get_target() const { return target_; }

private:
    void draw_common_attributes(Vox* n);
    void draw_2d_attributes(class Vox2D* n);
    void draw_3d_attributes(class Vox3D* n);
    void draw_rigidbody3d_attributes(class VoxRigidBody3D* n);
    void draw_staticbody3d_attributes(class VoxStaticBody3D* n);
    void draw_characterbody3d_attributes(class VoxCharacterBody3D* n);
    void draw_area3d_attributes(class VoxArea3D* n);
    void draw_raycast3d_attributes(class VoxRayCast3D* n);
    void draw_collision_shape3d_attributes(class VoxCollisionShape3D* n);
    void draw_sprite2d_attributes(class VoxSprite2D* n);
    void draw_camera2d_attributes(class VoxCamera2D* n);
    void draw_camera3d_attributes(class VoxCamera3D* n);
    void draw_mesh_instance3d_attributes(class VoxMeshInstance3D* n);
    void draw_audio_stream_player_attributes(class VoxAudioStreamPlayer* n);
    void draw_animation_player_attributes(class VoxAnimationPlayer* n);
    void draw_tween_attributes(class Tween* n);
    void draw_tilemap_attributes(class VoxTileMap* n);
    void draw_particles_attributes(class Vox2D* n);
    void draw_light3d_attributes(class VoxLight3D* n);
    void draw_sprite3d_attributes(class VoxSprite3D* n);
    void draw_navigation_agent_attributes(class NavigationAgent3D* n);
    void draw_multiplayer_attributes(class Vox* n);

    Vox* target_ = nullptr;
};

} // namespace arx
