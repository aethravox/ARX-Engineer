// ==============================================================================
// src/scene/animation/animation.hpp — Sistema de animación.
// VoxAnimationPlayer + VoxSpriteFrames (2D) + Animation (curvas interpoladas).
// ==============================================================================
#pragma once

#include "scene/vox.hpp"
#include "core/types.hpp"
#include "render/texture.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace arx {

// ===================== VoxSpriteFrames (2D) ====================================
// Conjunto de animaciones (cada una con sus frames y FPS).
class VoxSpriteFrames : public Object {
public:
    ARX_CLASS(VoxSpriteFrames, Object);
public:

    void add_animation(const std::string& name);
    void remove_animation(const std::string& name);
    bool has_animation(const std::string& name) const;

    void add_frame(const std::string& anim, std::shared_ptr<Texture> tex,
                    float duration = 0.1f);
    void set_fps(const std::string& anim, int fps);
    void set_loop(const std::string& anim, bool loop);

    int  get_frame_count(const std::string& anim) const;
    std::shared_ptr<Texture> get_frame(const std::string& anim, int idx) const;
    int  get_fps(const std::string& anim) const;
    bool get_loop(const std::string& anim) const;

    std::vector<std::string> get_animations() const;

private:
    struct Anim {
        std::vector<std::shared_ptr<Texture>> frames;
        std::vector<float>                    durations;  // segundos por frame
        int  fps  = 10;
        bool loop = true;
    };
    std::unordered_map<std::string, Anim> anims_;
};

// ===================== VoxAnimationPlayer ======================================
// Reproduce animaciones que afectan propiedades de Nodes vía curvas interpoladas.
class VoxAnimationPlayer : public Vox {
public:
    ARX_CLASS(VoxAnimationPlayer, Vox);
public:

    // Una pista (track) anima una propiedad de un Vox.
    enum class TrackType { Property, Method, Audio };

    struct Keyframe {
        float time;
        Variant value;
        // Tipo de interpolación:
        // 0 = linear, 1 = ease_in, 2 = ease_out, 3 = ease_in_out, 4 = cubic
        int    interp = 0;
    };

    struct Track {
        TrackType type = TrackType::Property;
        std::string node_path;       // "/root/Main/Player"
        std::string property;        // "position"
        std::vector<Keyframe> keys;
    };

    struct Animation {
        std::string name;
        float       length = 1.0f;
        bool        loop   = false;
        std::vector<Track> tracks;
    };

    void add_animation(const Animation& anim);
    void remove_animation(const std::string& name);
    bool has_animation(const std::string& name) const;

    void play(const std::string& name);
    void stop();
    void pause();
    void resume();

    void seek(float t);
    float get_position() const { return position_; }
    float get_length() const;
    bool  is_playing() const { return playing_; }
    std::string get_current_animation() const { return current_; }

    void set_speed(float s) { speed_ = s; }
    float get_speed() const { return speed_; }

    void set_autoplay(const std::string& name) { autoplay_ = name; }

    void process(float delta) override;

    // Callbacks.
    void on_animation_finished(std::function<void(const std::string&)> cb) {
        on_finished_ = std::move(cb);
    }

private:
    std::unordered_map<std::string, Animation> anims_;
    std::string current_;
    bool   playing_  = false;
    bool   paused_   = false;
    float  position_ = 0.0f;
    float  speed_    = 1.0f;
    std::string autoplay_;
    std::function<void(const std::string&)> on_finished_;

    Variant interpolate(const Track& t, float time);
    void    apply_track(const Track& t, float time);
};

// ===================== Tween ================================================
// Animación procedural: interpola una propiedad de un Vox entre dos valores.
class Tween : public Vox {
public:
    ARX_CLASS(Tween, Vox);
public:

    enum class Transition { Linear, Sine, Quint, Quart, Expo, Circ, Cubic, Quad, Bounce, Back, Elastic };
    enum class Ease       { In, Out, InOut, OutIn };

    struct Tweener {
        Object*   target   = nullptr;
        std::string property;
        Variant  from;
        Variant  to;
        float    duration   = 1.0f;
        float    delay      = 0.0f;
        Transition trans    = Transition::Linear;
        Ease      ease      = Ease::InOut;
        float    elapsed    = 0.0f;
        bool     finished   = false;
    };

    Tween& tween_property(Object* target, const std::string& prop,
                            Variant to, float duration);
    Tween& tween_property(Object* target, const std::string& prop,
                            Variant from, Variant to, float duration);
    Tween& set_parallel(bool p) { parallel_ = p; return *this; }
    Tween& set_pause_timer(bool p) { pause_timer_ = p; return *this; }
    Tween& set_loops(int n) { loops_ = n; return *this; }

    void play();
    void pause();
    void stop();
    void kill();

    bool is_running() const { return running_; }
    bool is_finished() const;

    void process(float delta) override;

    // Callbacks.
    void on_finished(std::function<void()> cb) { on_finished_ = std::move(cb); }

private:
    std::vector<Tweener>      tweeners_;
    bool parallel_   = false;     // false = sequential, true = all at once
    bool pause_timer_ = false;
    int  loops_       = 1;
    bool running_     = false;
    int  current_idx_ = 0;        // Para sequential
    std::function<void()> on_finished_;

    float apply_transition(float t, Transition tr, Ease e);
};

} // namespace arx
