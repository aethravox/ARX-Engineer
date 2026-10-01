// ==============================================================================
// src/scene/scene_tree.hpp — Árbol de voxes, main loop, paquete de procesos.
// ==============================================================================
#pragma once

#include "scene/main_loop.hpp"
#include "scene/vox.hpp"
#include "core/variant.hpp"
#include "physics/physics_server.hpp"  // para RID

#include <memory>
#include <vector>

namespace arx {

class Window;
class Renderer;
class InputState;
class PhysicsServer;
class PhysicsServer2D;
class AudioServer;

// SceneTree: raíz del mundo del juego. Maneja un único árbol de Nodes,
// periodicamente llama a process() y physics_process() en todos los nodos
// habilitados, y los dibuja a través del renderer.
class SceneTree : public MainLoop {
public:
    ARX_CLASS(SceneTree, MainLoop);
public:

    SceneTree();
    ~SceneTree() override;

    bool init(Window* window, Renderer* renderer);

    // MainLoop
    void start() override;
    bool process(float delta) override;
    void finish() override;

    // Tree ops
    Vox* get_root() const { return root_; }
    void  set_root(Vox* root);

    // Recorrer el árbol
    void  add_node_to_tree(Vox* n);   // Llama a enter_tree / ready
    void  remove_node_from_tree(Vox* n);

    // Servicios
    Renderer*       get_renderer()        const { return renderer_; }
    PhysicsServer*  get_physics_server()  const { return physics_server_.get(); }
    PhysicsServer2D* get_physics_server_2d() const { return physics_server_2d_.get(); }
    AudioServer*    get_audio_server()    const { return audio_server_.get(); }

    // Space RID del mundo de física por defecto (para que los Voxes lo usen)
    RID  get_default_space() const { return default_space_; }

    // Grupos (estilo Godot: add_to_group("enemies"))
    void add_to_group(const std::string& group, Vox* n);
    void remove_from_group(const std::string& group, Vox* n);
    std::vector<Vox*> get_nodes_in_group(const std::string& group) const;

    // Pause
    void set_pause(bool p) { paused_ = p; }
    bool is_paused() const { return paused_; }

    // Quit
    void quit() { quit_requested_ = true; }

private:
    void process_node(Vox* n, float delta);
    void physics_process_node(Vox* n, float delta);
    void draw_node(Vox* n);

    Vox* root_ = nullptr;
    Renderer* renderer_ = nullptr;
    std::shared_ptr<PhysicsServer>   physics_server_;
    std::shared_ptr<PhysicsServer2D> physics_server_2d_;
    std::shared_ptr<AudioServer>     audio_server_;

    bool paused_ = false;
    bool quit_requested_ = false;
    RID  default_space_;  // espacio de física 3D por defecto

    // Groups
    std::unordered_map<std::string, std::vector<Vox*>> groups_;
};

} // namespace arx
