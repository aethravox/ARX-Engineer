// ==============================================================================
// src/scene/particles/particles.hpp — Sistema de partículas 2D y 3D.
// ==============================================================================
#pragma once

#include "scene/2d/vox2d.hpp"
#include "scene/3d/vox3d.hpp"
#include "render/texture.hpp"
#include "core/types.hpp"

#include <vector>
#include <memory>

namespace arx {

// ===================== VoxParticles2D ==========================================
class VoxParticles2D : public Vox2D {
public:
    ARX_CLASS(VoxParticles2D, Vox2D);
public:

    void set_amount(int n)        { amount_ = std::max(1, n); restart(); }
    int  get_amount() const       { return amount_; }
    void set_lifetime(float t)    { lifetime_ = std::max(0.01f, t); }
    float get_lifetime() const    { return lifetime_; }
    void set_emitting(bool e)     { emitting_ = e; }
    bool is_emitting() const      { return emitting_; }
    void set_one_shot(bool o)     { one_shot_ = o; }
    void set_explosiveness(float e) { explosiveness_ = std::clamp(e, 0.0f, 1.0f); }
    void set_direction(Vector2 d) { direction_ = d; }
    void set_spread(float s)      { spread_ = s; }  // radianes
    void set_initial_velocity(float v) { initial_velocity_ = v; }
    void set_gravity(Vector2 g)   { gravity_ = g; }
    void set_color(Color c)       { color_ = c; }
    void set_texture(std::shared_ptr<Texture> t) { texture_ = t; }
    void set_scale(float s)       { scale_ = s; }
    void restart();

    void process(float delta) override;

private:
    struct Particle {
        Vector2 position;
        Vector2 velocity;
        Color   color;
        float   life;
        float   lifetime;
        float   scale;
        float   rotation;
        bool    active = false;
    };

    int     amount_           = 16;
    float   lifetime_         = 1.0f;
    bool    emitting_         = true;
    bool    one_shot_         = false;
    float   explosiveness_    = 0.0f;
    Vector2 direction_        = {0, -1};
    float   spread_           = 3.14159265358979f / 4.0f;
    float   initial_velocity_ = 100.0f;
    Vector2 gravity_          = {0, 200.0f};
    Color   color_            = Color::white;
    float   scale_            = 1.0f;
    std::shared_ptr<Texture> texture_;

    std::vector<Particle> particles_;
    float   time_since_emit_ = 0.0f;

    void emit_particle(Particle& p);
};

// ===================== VoxParticles3D ==========================================
class VoxParticles3D : public Vox3D {
public:
    ARX_CLASS(VoxParticles3D, Vox3D);
public:

    void set_amount(int n)        { amount_ = std::max(1, n); restart(); }
    int  get_amount() const       { return amount_; }
    void set_lifetime(float t)    { lifetime_ = std::max(0.01f, t); }
    void set_emitting(bool e)     { emitting_ = e; }
    void set_one_shot(bool o)     { one_shot_ = o; }
    void set_direction(Vector3 d) { direction_ = d; }
    void set_spread(float s)      { spread_ = s; }
    void set_initial_velocity(float v) { initial_velocity_ = v; }
    void set_gravity(Vector3 g)   { gravity_ = g; }
    void set_color(Color c)       { color_ = c; }
    void set_scale(float s)       { scale_ = s; }
    void restart();

    void process(float delta) override;

private:
    struct Particle3D {
        Vector3 position;
        Vector3 velocity;
        Color   color;
        float   life;
        float   lifetime;
        float   scale;
        bool    active = false;
    };

    int     amount_           = 32;
    float   lifetime_         = 1.0f;
    bool    emitting_         = true;
    bool    one_shot_         = false;
    Vector3 direction_        = {0, 1, 0};
    float   spread_           = 0.5f;
    float   initial_velocity_ = 2.0f;
    Vector3 gravity_          = {0, -9.81f, 0};
    Color   color_            = Color::white;
    float   scale_            = 0.1f;

    std::vector<Particle3D> particles_;
    float   time_since_emit_ = 0.0f;

    void emit_particle(Particle3D& p);
};

} // namespace arx
