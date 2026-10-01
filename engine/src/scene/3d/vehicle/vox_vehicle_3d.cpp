// ==============================================================================
// src/scene/3d/vehicle/vehicle_3d.cpp
// ==============================================================================
#include "vox_vehicle_3d.hpp"
#include "core/math.hpp"
#include "core/logging.hpp"

#include <cmath>

namespace arx {

// ===================== Vehicle3D ============================================
void Vehicle3D::add_wheel(const Wheel& w) {
    wheels_.push_back(w);
}

void Vehicle3D::_physics_process(float delta) {
    // Aplicar engine_force y brake_force a las ruedas motrices.
    // Aplicar steering a las ruedas de dirección.
    for (auto& w : wheels_) {
        w.steering_angle = steering_ * max_steering_;
        w.rotation += (engine_force_ * delta) / w.wheel_radius;
        // Suspensión por raycast (simplificada):
        // En una impl real se haría un raytest desde connection_point a lo
        // largo de steering_axis * suspension_rest_length.
        // Si toca el suelo, aplicar fuerza de suspensión + fricción.
    }
    // Aplicar fuerza al rigidbody (usando la base de VoxRigidBody3D).
    if (engine_force_ != 0) {
        // Simplificación: empujar hacia adelante.
        Vector3 forward = get_global_transform().basis * Vector3(0, 0, -1);
        apply_impulse(forward * engine_force_ * delta);
    }
    if (brake_force_ > 0) {
        // Frenar (reducir velocidad lineal).
        Vector3 v = get_linear_velocity();
        set_linear_velocity(v * (1.0f - brake_force_ * delta * 0.5f));
    }
}

float Vehicle3D::get_current_speed_kmh() const {
    return glm::length(get_linear_velocity()) * 3.6f;
}

// ===================== SoftBody3D ===========================================
void SoftBody3D::generate_from_mesh(std::shared_ptr<Mesh> mesh, float resolution) {
    (void)mesh; (void)resolution;
    ARX_LOG_INFO("SoftBody3D::generate_from_mesh (stub)");
}

void SoftBody3D::generate_rope(int segments, float seg_len, Vector3 start, Vector3 end) {
    nodes_.clear(); springs_.clear();
    Vector3 dir = (end - start) / (float)segments;
    for (int i = 0; i <= segments; ++i) {
        SoftNode n;
        n.position = start + dir * (float)i;
        n.mass     = mass_per_node_;
        nodes_.push_back(n);
    }
    for (int i = 0; i < segments; ++i) {
        Spring s;
        s.a = i; s.b = i + 1;
        s.rest_length = seg_len;
        s.stiffness = stiffness_;
        springs_.push_back(s);
    }
}

void SoftBody3D::generate_cloth(int w, int h, float spacing) {
    nodes_.clear(); springs_.clear();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            SoftNode n;
            n.position = position_ + Vector3(x * spacing, 0, y * spacing);
            n.mass = mass_per_node_;
            nodes_.push_back(n);
        }
    }
    auto idx = [w](int x, int y) { return y * w + x; };
    // Sprints horizontales y verticales.
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (x < w - 1) {
                Spring s{ idx(x,y), idx(x+1,y), spacing, stiffness_ };
                springs_.push_back(s);
            }
            if (y < h - 1) {
                Spring s{ idx(x,y), idx(x,y+1), spacing, stiffness_ };
                springs_.push_back(s);
            }
            // Diagonales (shear).
            if (x < w - 1 && y < h - 1) {
                Spring s{ idx(x,y), idx(x+1,y+1), spacing * 1.4142f, stiffness_ * 0.5f };
                springs_.push_back(s);
            }
        }
    }
}

void SoftBody3D::pin_node(int i) {
    if (i >= 0 && i < (int)nodes_.size()) nodes_[i].pinned = true;
}

void SoftBody3D::unpin_node(int i) {
    if (i >= 0 && i < (int)nodes_.size()) nodes_[i].pinned = false;
}

void SoftBody3D::_physics_process(float delta) {
    // Limpiar fuerzas.
    for (auto& n : nodes_) n.force_accum = Vector3(0, -9.81f * n.mass, 0);

    // Aplicar resortes.
    for (const auto& s : springs_) {
        Vector3 delta_p = nodes_[s.b].position - nodes_[s.a].position;
        float dist = glm::length(delta_p);
        if (dist < 1e-6f) continue;
        Vector3 dir = delta_p / dist;
        float extension = dist - s.rest_length;
        Vector3 force = dir * extension * s.stiffness;
        nodes_[s.a].force_accum += force;
        nodes_[s.b].force_accum -= force;
    }

    // Integrar.
    for (auto& n : nodes_) {
        if (n.pinned) { n.velocity = {}; continue; }
        n.velocity += (n.force_accum / n.mass) * delta;
        n.velocity *= (1.0f - damping_ * delta);
        n.position += n.velocity * delta;
    }
}

} // namespace arx
