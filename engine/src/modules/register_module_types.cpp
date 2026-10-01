// ==============================================================================
// src/modules/register_module_types.cpp — Registro central de clases en ClassDB.
// ==============================================================================
#include "core/object.hpp"
#include "core/string_db.hpp"

// Scene
#include "scene/vox.hpp"
#include "scene/scene_tree.hpp"
#include "scene/state_machine.hpp"
#include "scene/2d/vox2d.hpp"
#include "scene/2d/vox_sprite_2d.hpp"
#include "scene/2d/vox_camera_2d.hpp"
#include "scene/2d/vox_camera_effects.hpp"
#include "scene/2d/vox_physics_2d.hpp"
#include "scene/3d/vox3d.hpp"
#include "scene/3d/vox_mesh_instance_3d.hpp"
#include "scene/3d/vox_physics_3d.hpp"
#include "scene/3d/vox_extra_3d.hpp"

// UI
#include "scene/ui/control.hpp"
#include "scene/ui/widgets.hpp"

// Animation
#include "scene/animation/animation.hpp"

// Particles
#include "scene/particles/particles.hpp"

// Tilemap
#include "scene/tilemap/tilemap.hpp"

// Resources
#include "scene/resources/resource.hpp"
#include "scene/resources/packed_scene.hpp"

// Editor (no registrar en runtime)
#ifdef ARX_BUILD_EDITOR
#include "editor/editor_main.hpp"
#include "editor/plugins/editor_plugin.hpp"
// Exporters
#include "export/export_plugin.hpp"
#include "export/aex_exporter.hpp"
#include "export/standalone_exporter.hpp"
#endif

namespace arx {

// Llamada desde main.cpp / runtime_main.cpp al iniciar el motor.
void register_module_types() {
    // Scene tree
    ClassDB::register_class<Vox>();
    ClassDB::register_class<SceneTree>();
    ClassDB::register_class<StateMachine>();

    // 2D
    ClassDB::register_class<Vox2D>();
    ClassDB::register_class<VoxSprite2D>();
    ClassDB::register_class<VoxCamera2D>();
    ClassDB::register_class<Label>();
    ClassDB::register_class<VoxRigidBody2D>();
    ClassDB::register_class<VoxStaticBody2D>();
    ClassDB::register_class<VoxCharacterBody2D>();
    ClassDB::register_class<VoxArea2D>();
    ClassDB::register_class<VoxCollisionShape2D>();
    ClassDB::register_class<VoxRayCast2D>();

    // 3D
    ClassDB::register_class<Vox3D>();
    ClassDB::register_class<VoxMeshInstance3D>();
    ClassDB::register_class<VoxCamera3D>();
    ClassDB::register_class<VoxDirectionalLight3D>();
    ClassDB::register_class<VoxOmniLight3D>();
    ClassDB::register_class<VoxRigidBody3D>();
    ClassDB::register_class<VoxStaticBody3D>();
    ClassDB::register_class<VoxKinematicBody3D>();
    ClassDB::register_class<VoxCharacterBody3D>();
    ClassDB::register_class<VoxArea3D>();
    ClassDB::register_class<VoxCollisionShape3D>();
    ClassDB::register_class<VoxRayCast3D>();
    ClassDB::register_class<VoxSprite3D>();
    ClassDB::register_class<VoxMultiMeshInstance3D>();
    ClassDB::register_class<VoxDecal>();
    ClassDB::register_class<VoxMarker3D>();
    ClassDB::register_class<VoxRemoteTransform3D>();
    ClassDB::register_class<VoxVisibleOnScreenNotifier3D>();
    ClassDB::register_class<VoxSkeleton3D>();

    // UI
    ClassDB::register_class<Control>();
    ClassDB::register_class<Label>();
    ClassDB::register_class<ColorRect>();
    ClassDB::register_class<Image>();
    ClassDB::register_class<Spacer>();
    ClassDB::register_class<Button>();
    ClassDB::register_class<LineEdit>();
    ClassDB::register_class<Slider>();
    ClassDB::register_class<ProgressBar>();
    ClassDB::register_class<CheckBox>();
    ClassDB::register_class<Container>();
    ClassDB::register_class<VBox>();
    ClassDB::register_class<HBox>();
    ClassDB::register_class<Grid>();
    ClassDB::register_class<ScrollContainer>();
    ClassDB::register_class<UIWindow>();
    ClassDB::register_class<OptionButton>();
    ClassDB::register_class<TextEdit>();

    // Animation
    ClassDB::register_class<VoxAnimationPlayer>();
    ClassDB::register_class<Tween>();
    ClassDB::register_class<VoxSpriteFrames>();

    // Particles
    ClassDB::register_class<VoxParticles2D>();
    ClassDB::register_class<VoxParticles3D>();

    // Tilemap
    ClassDB::register_class<VoxTileSet>();
    ClassDB::register_class<VoxTileMap>();

    // Resources
    ClassDB::register_class<Resource>();
    ClassDB::register_class<PackedScene>();

    // Editor
#ifdef ARX_BUILD_EDITOR
    ClassDB::register_class<EditorMain>();
    ClassDB::register_class<EditorPlugin>();
    // InspectorPlugin es abstracta, no se puede registrar como ClassDB
    // (sus subclases concretas se registran donde corresponda).

    // Registrar exporters disponibles
    auto& reg = ExportPluginRegistry::instance();
    reg.register_plugin(std::make_unique<AexExporter>());
    reg.register_plugin(std::make_unique<StandaloneExporter>("linux"));
    reg.register_plugin(std::make_unique<StandaloneExporter>("windows"));
    reg.register_plugin(std::make_unique<StandaloneExporter>("android"));
    reg.register_plugin(std::make_unique<StandaloneExporter>("web"));
#endif
}

void unregister_module_types() {
    // ClassDB no lo soporta en esta versión: la idea es que los registros
    // persisten durante todo el proceso y se limpian con el exit.
}

namespace arx_script {
    void register_all_classes() {
        // Vacío: cada .arx.cpp tiene su propia línea
        // static const bool _registered_X = ClassDB::register_class<T>().valid;
    }
}

} // namespace arx
