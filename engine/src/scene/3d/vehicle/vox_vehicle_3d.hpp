// ==============================================================================
// src/scene/3d/vehicle/vehicle_3d.hpp — Vehículo con raycasts (estilo raycast vehicle).
// ==============================================================================
#pragma once

#include "scene/3d/vox_physics_3d.hpp"
#include <vector>

namespace arx {

class Vehicle3D : public VoxRigidBody3D {
public:
    ARX_CLASS(Vehicle3D, VoxRigidBody3D);
public:

    struct Wheel {
        Vector3 connection_point;   // Punto de attachment al chasis.
        Vector3 steering_axis;       // Eje de dirección (suele ser Y).
        Vector3 axle;                // Eje de rotación de la rueda (X o Z).
        float  suspension_rest_length = 0.3f;
        float  suspension_stiffness   = 40.0f;
        float  suspension_damping     = 4.0f;
        float  wheel_radius           = 0.3f;
        float  friction_slip          = 10.0f;
        float  roll_influence         = 0.1f;
        bool   is_steering_wheel      = true;
        bool   is_drive_wheel         = false;
        bool   is_brake_wheel         = true;

        // Runtime.
        float  rotation = 0.0f;      // Cuánto ha girado la rueda.
        float  steering_angle = 0.0f;
        bool   is_in_contact = false;
        Vector3 contact_point;
        Vector3 contact_normal;
        float  suspension_length = 0.0f;
    };

    void add_wheel(const Wheel& w);
    int  get_wheel_count() const { return (int)wheels_.size(); }
    Wheel& get_wheel(int i) { return wheels_[i]; }

    void set_engine_force(float f) { engine_force_ = f; }
    void set_brake_force(float f)  { brake_force_ = f; }
    void set_steering(float s)     { steering_ = s; }

    float get_engine_force() const { return engine_force_; }
    float get_brake_force() const  { return brake_force_; }
    float get_steering() const     { return steering_; }

    void set_max_steering(float s) { max_steering_ = s; }
    void set_max_engine_force(float f) { max_engine_force_ = f; }
    void set_max_brake_force(float f) { max_brake_force_ = f; }

    void _physics_process(float delta) override;

    // Current speed in km/h.
    float get_current_speed_kmh() const;

private:
    std::vector<Wheel> wheels_;
    float engine_force_ = 0.0f;
    float brake_force_  = 0.0f;
    float steering_     = 0.0f;
    float max_steering_       = 0.5f;
    float max_engine_force_   = 2000.0f;
    float max_brake_force_    = 100.0f;
};

// SoftBody3D — simulación de cuerpos blandos (tela, gel, etc.).
class SoftBody3D : public Vox3D {
public:
    ARX_CLASS(SoftBody3D, Vox3D);
public:

    void set_stiffness(float s) { stiffness_ = s; }
    void set_damping(float d)   { damping_ = d; }
    void set_mass(float m)      { mass_per_node_ = m; }
    void set_pressure(float p)  { pressure_ = p; }

    // Generar malla bland suave a partir de un Mesh.
    void generate_from_mesh(std::shared_ptr<class Mesh> mesh, float resolution = 0.5f);

    // Generar una cuerda (cadena de nodos).
    void generate_rope(int segments, float segment_length, Vector3 start, Vector3 end);

    // Generar tela (grid de nodos).
    void generate_cloth(int width, int height, float spacing);

    // Fijar nodos (no se mueven).
    void pin_node(int index);
    void unpin_node(int index);

    void _physics_process(float delta) override;

    int get_node_count() const { return (int)nodes_.size(); }
    Vector3 get_node_position(int i) const {
        return (i >= 0 && i < (int)nodes_.size()) ? nodes_[i].position : Vector3{};
    }

private:
    struct SoftNode {
        Vector3 position;
        Vector3 velocity;
        Vector3 force_accum;
        float   mass = 1.0f;
        bool    pinned = false;
    };
    struct Spring {
        int a, b;
        float rest_length;
        float stiffness;
    };

    std::vector<SoftNode> nodes_;
    std::vector<Spring>   springs_;
    float stiffness_      = 100.0f;
    float damping_        = 0.5f;
    float mass_per_node_  = 0.1f;
    float pressure_       = 0.0f;
};

// Rope3D — cuerda simple (alias para SoftBody3D generado como rope).
class Rope3D : public SoftBody3D {
public:
    ARX_CLASS(Rope3D, SoftBody3D);
public:

    void set_length(float l)   { length_ = l; }
    void set_segments(int s)   { segments_ = s; }
    void set_attach_point(Vector3 a) { attach_ = a; }

    void _enter_tree() override {
        generate_rope(segments_, length_ / segments_, attach_,
                       attach_ + Vector3(0, -length_, 0));
        pin_node(0);
    }

private:
    float   length_    = 2.0f;
    int     segments_  = 10;
    Vector3 attach_{0, 0, 0};
};

// Cloth3D — tela simple (alias para SoftBody3D generado como cloth).
class Cloth3D : public SoftBody3D {
public:
    ARX_CLASS(Cloth3D, SoftBody3D);
public:

    void set_width(int w)  { width_ = w; }
    void set_height(int h) { height_ = h; }
    void set_spacing(float s) { spacing_ = s; }

    void _enter_tree() override {
        generate_cloth(width_, height_, spacing_);
        // Fijar esquinas.
        pin_node(0);
        pin_node(width_ - 1);
    }

private:
    int   width_   = 10;
    int   height_  = 10;
    float spacing_ = 0.2f;
};

} // namespace arx
