// ==============================================================================
// src/scene/animation/animation_player.hpp — Reproductor de animaciones.
// ==============================================================================
#pragma once

#include "scene/vox.hpp"
#include "core/variant.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace arx {

// Animation: una secuencia de tracks (property paths + keyframes).
class Animation : public Object {
public:
    ARX_CLASS(Animation, Object);
public:

    struct Track {
        enum class Type { Property, Method, Bezier, Audio };
        Type        type = Type::Property;
        std::string path;       // ej "VoxSprite2D:modulate:r"
        std::string property;
        std::vector<std::pair<float, Variant>> keyframes;  // time, value
        bool       loop = false;
    };

    void set_name(const std::string& n) { name_ = n; }
    const std::string& get_name() const { return name_; }
    void set_length(float l) { length_ = l; }
    float get_length() const { return length_; }
    void set_loop(bool l) { loop_ = l; }
    bool is_loop() const { return loop_; }
    void set_fps(float f) { fps_ = f; }
    float get_fps() const { return fps_; }

    void add_track(const Track& t) { tracks_.push_back(t); }
    int  get_track_count() const { return static_cast<int>(tracks_.size()); }
    const Track& get_track(int i) const { return tracks_[i]; }

private:
    std::string name_;
    float       length_ = 1.0f;
    bool        loop_   = false;
    float       fps_    = 30.0f;
    std::vector<Track> tracks_;
};

class VoxAnimationPlayer : public Vox {
public:
    ARX_CLASS(VoxAnimationPlayer, Vox);
public:

    void add_animation(const std::string& name, std::shared_ptr<Animation> anim);
    bool has_animation(const std::string& name) const;
    std::shared_ptr<Animation> get_animation(const std::string& name) const;
    void remove_animation(const std::string& name);

    void play(const std::string& name, float blend = -1.0f);
    void play_backwards(const std::string& name);
    void stop();
    void pause();
    void resume();

    bool is_playing() const { return playing_; }
    bool is_paused() const { return paused_; }
    float get_current_position() const { return current_position_; }
    void  set_current_position(float p) { current_position_ = p; }
    float get_current_length() const;
    std::string get_current_animation() const { return current_anim_; }

    void set_speed(float s) { speed_ = s; }
    float get_speed() const { return speed_; }

    void set_autoplay(const std::string& name) { autoplay_ = name; }
    std::string get_autoplay() const { return autoplay_; }

    // Signals: animation_started, animation_finished, animation_changed

    void process(float delta) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Animation>> animations_;
    std::string current_anim_;
    std::string autoplay_;
    float       current_position_ = 0.0f;
    float       speed_            = 1.0f;
    bool        playing_          = false;
    bool        paused_           = false;
    bool        backwards_        = false;

    void apply_animation_(Animation& anim, float time);
    Variant interpolate_track_(const Animation::Track& t, float time);
};

} // namespace arx
