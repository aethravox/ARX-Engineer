// ==============================================================================
#include "core/variant.hpp"
// src/scene/animation/animation_player.cpp
// ==============================================================================
#include "animation_player.hpp"
#include "core/math.hpp"
#include "core/logging.hpp"

#include <algorithm>

namespace arx {

void VoxAnimationPlayer::add_animation(const std::string& name,
                                       std::shared_ptr<Animation> anim) {
    animations_[name] = std::move(anim);
}
bool VoxAnimationPlayer::has_animation(const std::string& name) const {
    return animations_.find(name) != animations_.end();
}
std::shared_ptr<Animation> VoxAnimationPlayer::get_animation(const std::string& name) const {
    auto it = animations_.find(name);
    return it != animations_.end() ? it->second : nullptr;
}
void VoxAnimationPlayer::remove_animation(const std::string& name) {
    animations_.erase(name);
}

void VoxAnimationPlayer::play(const std::string& name, float blend) {
    (void)blend;
    if (!has_animation(name)) {
        ARX_LOG_WARN("VoxAnimationPlayer::play: animación '{}' no existe", name);
        return;
    }
    current_anim_      = name;
    current_position_  = 0.0f;
    playing_           = true;
    paused_            = false;
    backwards_         = false;
    emit_signal(sid("animation_started"));
}

void VoxAnimationPlayer::play_backwards(const std::string& name) {
    play(name);
    backwards_ = true;
    auto anim = get_animation(name);
    if (anim) current_position_ = anim->get_length();
}

void VoxAnimationPlayer::stop() {
    playing_ = false;
    current_position_ = 0.0f;
    current_anim_.clear();
}

void VoxAnimationPlayer::pause()  { paused_  = true; }
void VoxAnimationPlayer::resume() { paused_ = false; }

float VoxAnimationPlayer::get_current_length() const {
    auto a = get_animation(current_anim_);
    return a ? a->get_length() : 0.0f;
}

void VoxAnimationPlayer::process(float delta) {
    if (autoplay_.empty() && current_anim_.empty()) return;
    if (autoplay_ != "" && !playing_) { play(autoplay_); autoplay_.clear(); }
    if (!playing_ || paused_ || current_anim_.empty()) return;

    auto anim = get_animation(current_anim_);
    if (!anim) return;

    float step = delta * speed_ * (backwards_ ? -1.0f : 1.0f);
    current_position_ += step;

    if (current_position_ >= anim->get_length()) {
        if (anim->is_loop()) {
            current_position_ = std::fmod(current_position_, anim->get_length());
        } else {
            current_position_ = anim->get_length();
            playing_ = false;
            emit_signal(sid("animation_finished"));
            return;
        }
    } else if (current_position_ < 0.0f) {
        if (anim->is_loop()) current_position_ = anim->get_length() + current_position_;
        else { current_position_ = 0.0f; playing_ = false; return; }
    }

    apply_animation_(*anim, current_position_);
}

void VoxAnimationPlayer::apply_animation_(Animation& anim, float time) {
    for (const auto& track : anim.get_track_count() ? std::vector<Animation::Track>{} : std::vector<Animation::Track>{}) {
        (void)track;  // silence
    }
    for (int i = 0; i < anim.get_track_count(); ++i) {
        const auto& track = anim.get_track(i);
        if (track.type != Animation::Track::Type::Property) continue;

        Variant value = interpolate_track_(track, time);
        // Buscar el nodo objetivo.
        Vox* target = get_node(track.path);
        if (!target) continue;
        target->set_property(sid(track.property.c_str()), value);
    }
}

Variant VoxAnimationPlayer::interpolate_track_(const Animation::Track& t, float time) {
    if (t.keyframes.empty()) return {};
    if (time <= t.keyframes.front().first) return t.keyframes.front().second;
    if (time >= t.keyframes.back().first)  return t.keyframes.back().second;

    for (size_t i = 1; i < t.keyframes.size(); ++i) {
        if (t.keyframes[i].first >= time) {
            auto& [t0, v0] = t.keyframes[i-1];
            auto& [t1, v1] = t.keyframes[i];
            float alpha = (time - t0) / (t1 - t0);
            // Interpolación por tipo.
            if (v0.get_type() == Variant::Type::Float) {
                return Variant(math::lerp(v0.to_float(), v1.to_float(), alpha));
            } else if (v0.get_type() == Variant::Type::Vector2) {
                return Variant(math::lerp(v0.to_vector2(), v1.to_vector2(), alpha));
            } else if (v0.get_type() == Variant::Type::Vector3) {
                return Variant(math::lerp(v0.to_vector3(), v1.to_vector3(), alpha));
            } else if (v0.get_type() == Variant::Type::Color) {
                Color a = v0.to_color(), b = v1.to_color();
                return Variant(Color(
                    math::lerp(a.r, b.r, alpha),
                    math::lerp(a.g, b.g, alpha),
                    math::lerp(a.b, b.b, alpha),
                    math::lerp(a.a, b.a, alpha)
                ));
            }
            // Default: snap.
            return v1;
        }
    }
    return t.keyframes.back().second;
}

} // namespace arx
