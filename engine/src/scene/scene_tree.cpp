// ==============================================================================
// src/scene/scene_tree.cpp
// ==============================================================================
#include "scene_tree.hpp"
#include "vox.hpp"
#include "render/renderer.hpp"
#include "os/os.hpp"
#include "core/logging.hpp"
#include "core/math.hpp"
#include "physics/physics_server.hpp"

#include <algorithm>

namespace arx {

SceneTree::SceneTree() {
    // Raíz implícita "root".
    root_ = new Vox();
    root_->set_name("root");
    root_->set_tree(this);
}
SceneTree::~SceneTree() {
    delete root_;
}

bool SceneTree::init(Window* w, Renderer* r) {
    window_ = w;
    renderer_ = r;

    // Inicializar PhysicsServer 3D (Bullet)
    physics_server_ = PhysicsServer::create();
    if (physics_server_) {
        physics_server_->init();
        // Crear espacio (world) y setear gravedad por defecto
        default_space_ = physics_server_->space_create();
        physics_server_->space_set_gravity(default_space_, Vector3{0.0f, -9.8f, 0.0f});
        physics_server_->space_set_active(default_space_, true);
        ARX_LOG_INFO("SceneTree: PhysicsServer iniciado (gravedad: -9.8 m/s², space id={})",
                     default_space_.id);
    } else {
        ARX_LOG_WARN("SceneTree: no se pudo crear PhysicsServer");
    }

    return true;
}

void SceneTree::set_root(Vox* root) {
    if (root_ && root_ != root) delete root_;
    root_ = root;
    root_->set_tree(this);
    root_->set_name("root");
    add_node_to_tree(root_);
}

void SceneTree::add_node_to_tree(Vox* n) {
    if (!n) return;
    if (!n->inside_tree_) {
        n->inside_tree_ = true;
        n->_enter_tree();
        n->_ready();
        for (auto* c : n->get_children()) add_node_to_tree(c);
    }
}

void SceneTree::remove_node_from_tree(Vox* n) {
    if (!n) return;
    n->_exit_tree();
    n->inside_tree_ = false;
    for (auto* c : n->get_children()) remove_node_from_tree(c);
}

void SceneTree::start() {
    ARX_LOG_INFO("SceneTree iniciado");
}

bool SceneTree::process(float delta) {
    if (quit_requested_) return false;

    // 1. Process input events.
    if (window_) {
        auto events = window_->drain_events();
        for (const auto& ev : events) {
            // Repartir a todos los nodos (simplificación).
            if (root_) {
                std::function<void(Vox*)> dispatch = [&](Vox* n) {
                    n->input((const void*)&ev);
                    for (auto* c : n->get_children()) dispatch(c);
                };
                dispatch(root_);
            }
        }
    }

    // 2. Process.
    if (!paused_ && root_) {
        process_node(root_, delta);
    }

    // 2.5. Physics: primero _physics_process en los Voxes (leen transforms del server),
    //     después step() avanza la simulación.
    if (!paused_ && root_) {
        physics_process_node(root_, delta);
    }
    if (!paused_ && physics_server_) {
        physics_server_->step(delta);
    }

    // 3. Render.
    if (renderer_) {
        renderer_->begin_frame();
        if (root_) draw_node(root_);
        renderer_->end_frame();
    }

    // 4. Swap.
    if (window_) window_->swap_buffers();

    // Estadísticas
    stats_.delta_seconds = delta;
    stats_.fps = 1.0f / std::max(delta, 1e-6f);
    if (renderer_) {
        const auto& rs = renderer_->get_stats();
        stats_.draw_calls = rs.draw_calls;
        stats_.vertices   = rs.vertices;
    }

    return true;
}

void SceneTree::finish() {
    if (physics_server_) {
        physics_server_->shutdown();
        physics_server_.reset();
    }
    ARX_LOG_INFO("SceneTree finalizado");
}

void SceneTree::process_node(Vox* n, float delta) {
    if (n->is_processing()) n->process(delta);
    for (auto* c : n->get_children()) process_node(c, delta);
}

void SceneTree::physics_process_node(Vox* n, float delta) {
    if (n->is_processing()) n->_physics_process(delta);
    for (auto* c : n->get_children()) physics_process_node(c, delta);
}

void SceneTree::draw_node(Vox* n) {
    if (!n->is_visible()) return;
    n->propagate_notification(NOTIFICATION_DRAW);
    for (auto* c : n->get_children()) draw_node(c);
}

void SceneTree::add_to_group(const std::string& g, Vox* n) {
    auto& vec = groups_[g];
    if (std::find(vec.begin(), vec.end(), n) == vec.end()) vec.push_back(n);
}

void SceneTree::remove_from_group(const std::string& g, Vox* n) {
    auto it = groups_.find(g);
    if (it == groups_.end()) return;
    auto& vec = it->second;
    vec.erase(std::remove(vec.begin(), vec.end(), n), vec.end());
}

std::vector<Vox*> SceneTree::get_nodes_in_group(const std::string& g) const {
    auto it = groups_.find(g);
    if (it == groups_.end()) return {};
    return it->second;
}

} // namespace arx
