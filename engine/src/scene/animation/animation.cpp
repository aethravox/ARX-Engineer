#include "scene/scene_tree.hpp"
// ==============================================================================
#include "core/variant.hpp"
// src/scene/animation/animation.cpp
// ==============================================================================
#include "animation.hpp"
#include "core/math.hpp"
#include "core/logging.hpp"

#include <cmath>
#include <algorithm>

namespace arx {

// ===================== VoxSpriteFrames ==========================================
void VoxSpriteFrames::add_animation(const std::string& name) {
    if (anims_.find(name) == anims_.end()) anims_[name] = Anim{};
}

void VoxSpriteFrames::remove_animation(const std::string& name) { anims_.erase(name); }
bool VoxSpriteFrames::has_animation(const std::string& name) const {
    return anims_.find(name) != anims_.end();
}

void VoxSpriteFrames::add_frame(const std::string& anim, std::shared_ptr<Texture> tex,
                              float duration) {
    add_animation(anim);
    anims_[anim].frames.push_back(tex);
    anims_[anim].durations.push_back(duration);
}

void VoxSpriteFrames::set_fps(const std::string& anim, int fps) {
    if (has_animation(anim)) anims_[anim].fps = std::max(1, fps);
}
void VoxSpriteFrames::set_loop(const std::string& anim, bool loop) {
    if (has_animation(anim)) anims_[anim].loop = loop;
}

int VoxSpriteFrames::get_frame_count(const std::string& anim) const {
    auto it = anims_.find(anim);
    return it != anims_.end() ? (int)it->second.frames.size() : 0;
}

std::shared_ptr<Texture> VoxSpriteFrames::get_frame(const std::string& anim, int idx) const {
    auto it = anims_.find(anim);
    if (it == anims_.end()) return nullptr;
    if (idx < 0 || idx >= (int)it->second.frames.size()) return nullptr;
    return it->second.frames[idx];
}

int VoxSpriteFrames::get_fps(const std::string& anim) const {
    auto it = anims_.find(anim);
    return it != anims_.end() ? it->second.fps : 0;
}

bool VoxSpriteFrames::get_loop(const std::string& anim) const {
    auto it = anims_.find(anim);
    return it != anims_.end() ? it->second.loop : false;
}

std::vector<std::string> VoxSpriteFrames::get_animations() const {
    std::vector<std::string> out;
    for (const auto& [n, _] : anims_) out.push_back(n);
    return out;
}

// ===================== VoxAnimationPlayer =======================================
void VoxAnimationPlayer::add_animation(const Animation& anim) {
    anims_[anim.name] = anim;
}

void VoxAnimationPlayer::remove_animation(const std::string& name) {
    anims_.erase(name);
}

bool VoxAnimationPlayer::has_animation(const std::string& name) const {
    return anims_.find(name) != anims_.end();
}

void VoxAnimationPlayer::play(const std::string& name) {
    if (!has_animation(name)) {
        ARX_LOG_WARN("VoxAnimationPlayer: anim '{}' no existe", name);
        return;
    }
    current_  = name;
    position_ = 0.0f;
    playing_  = true;
    paused_   = false;
}

void VoxAnimationPlayer::stop()   { playing_ = false; position_ = 0.0f; }
void VoxAnimationPlayer::pause()  { paused_ = true; }
void VoxAnimationPlayer::resume() { paused_ = false; }

void VoxAnimationPlayer::seek(float t) {
    position_ = std::clamp(t, 0.0f, get_length());
}

float VoxAnimationPlayer::get_length() const {
    auto it = anims_.find(current_);
    return it != anims_.end() ? it->second.length : 0.0f;
}

void VoxAnimationPlayer::process(float delta) {
    if (autoplay_ != "" && !playing_) play(autoplay_);
    if (!playing_ || paused_) return;
    auto it = anims_.find(current_);
    if (it == anims_.end()) return;

    const auto& anim = it->second;
    position_ += delta * speed_;

    for (const auto& track : anim.tracks) {
        apply_track(track, position_);
    }

    if (position_ >= anim.length) {
        if (anim.loop) {
            position_ = std::fmod(position_, anim.length);
        } else {
            playing_ = false;
            if (on_finished_) on_finished_(current_);
        }
    }
}

Variant VoxAnimationPlayer::interpolate(const Track& t, float time) {
    if (t.keys.empty()) return {};
    if (time <= t.keys.front().time) return t.keys.front().value;
    if (time >= t.keys.back().time)  return t.keys.back().value;

    for (size_t i = 0; i < t.keys.size() - 1; ++i) {
        if (time >= t.keys[i].time && time <= t.keys[i+1].time) {
            float t0 = t.keys[i].time;
            float t1 = t.keys[i+1].time;
            float f  = (time - t0) / std::max(t1 - t0, 1e-6f);
            // Interpolación simple solo para tipos numéricos.
            const Variant& v0 = t.keys[i].value;
            const Variant& v1 = t.keys[i+1].value;
            if (v0.get_type() == Variant::Type::Float) {
                return Variant(math::lerp((float)v0.to_float(), (float)v1.to_float(), f));
            }
            if (v0.get_type() == Variant::Type::Vector2) {
                return Variant(math::lerp(v0.to_vector2(), v1.to_vector2(), f));
            }
            if (v0.get_type() == Variant::Type::Vector3) {
                return Variant(math::lerp(v0.to_vector3(), v1.to_vector3(), f));
            }
            if (v0.get_type() == Variant::Type::Color) {
                Color c0 = v0.to_color(), c1 = v1.to_color();
                return Variant(Color(
                    math::lerp(c0.r, c1.r, f),
                    math::lerp(c0.g, c1.g, f),
                    math::lerp(c0.b, c1.b, f),
                    math::lerp(c0.a, c1.a, f)
                ));
            }
            return v1;
        }
    }
    return t.keys.back().value;
}

void VoxAnimationPlayer::apply_track(const Track& t, float time) {
    if (t.type != TrackType::Property) return;
    // Buscar el Vox target por path.
    if (!get_tree()) return;
    Vox* target = get_node(t.node_path);
    if (!target) return;
    Variant v = interpolate(t, time);
    target->set_property(sid(t.property.c_str()), v);
}

// ===================== Tween =================================================
Tween& Tween::tween_property(Object* target, const std::string& prop,
                              Variant to, float duration) {
    Variant from = target->get_property(sid(prop.c_str()));
    return tween_property(target, prop, from, to, duration);
}

Tween& Tween::tween_property(Object* target, const std::string& prop,
                              Variant from, Variant to, float duration) {
    Tweener t;
    t.target   = target;
    t.property = prop;
    t.from     = from;
    t.to       = to;
    t.duration = duration;
    tweeners_.push_back(std::move(t));
    return *this;
}

void Tween::play()  { running_ = true; }
void Tween::pause() { running_ = false; }
void Tween::stop()  { running_ = false; for (auto& t : tweeners_) { t.elapsed = 0; t.finished = false; } current_idx_ = 0; }
void Tween::kill()  { tweeners_.clear(); running_ = false; }

bool Tween::is_finished() const {
    for (const auto& t : tweeners_) if (!t.finished) return false;
    return true;
}

void Tween::process(float delta) {
    if (!running_) return;

    if (parallel_) {
        bool all_done = true;
        for (auto& t : tweeners_) {
            if (t.finished) continue;
            t.elapsed += delta;
            float progress = std::clamp((t.elapsed - t.delay) / t.duration, 0.0f, 1.0f);
            float eased = apply_transition(progress, t.trans, t.ease);
            Variant v;
            // Interpolación según tipo.
            if (t.from.get_type() == Variant::Type::Float)
                v = Variant(math::lerp((float)t.from.to_float(), (float)t.to.to_float(), eased));
            else if (t.from.get_type() == Variant::Type::Vector2)
                v = Variant(math::lerp(t.from.to_vector2(), t.to.to_vector2(), eased));
            else if (t.from.get_type() == Variant::Type::Vector3)
                v = Variant(math::lerp(t.from.to_vector3(), t.to.to_vector3(), eased));
            else if (t.from.get_type() == Variant::Type::Color) {
                Color a = t.from.to_color(), b = t.to.to_color();
                v = Variant(Color(
                    math::lerp(a.r, b.r, eased),
                    math::lerp(a.g, b.g, eased),
                    math::lerp(a.b, b.b, eased),
                    math::lerp(a.a, b.a, eased)
                ));
            } else v = t.to;

            t.target->set_property(sid(t.property.c_str()), v);
            if (progress >= 1.0f) t.finished = true;
            else all_done = false;
        }
        if (all_done) {
            running_ = false;
            if (on_finished_) on_finished_();
        }
    } else {
        // Sequential
        if (current_idx_ >= (int)tweeners_.size()) {
            running_ = false;
            if (on_finished_) on_finished_();
            return;
        }
        auto& t = tweeners_[current_idx_];
        t.elapsed += delta;
        float progress = std::clamp((t.elapsed - t.delay) / t.duration, 0.0f, 1.0f);
        float eased = apply_transition(progress, t.trans, t.ease);
        if (t.from.get_type() == Variant::Type::Float)
            t.target->set_property(sid(t.property.c_str()),
                Variant(math::lerp((float)t.from.to_float(), (float)t.to.to_float(), eased)));
        else if (t.from.get_type() == Variant::Type::Vector2)
            t.target->set_property(sid(t.property.c_str()),
                Variant(math::lerp(t.from.to_vector2(), t.to.to_vector2(), eased)));
        else if (t.from.get_type() == Variant::Type::Vector3)
            t.target->set_property(sid(t.property.c_str()),
                Variant(math::lerp(t.from.to_vector3(), t.to.to_vector3(), eased)));
        if (progress >= 1.0f) {
            t.finished = true;
            ++current_idx_;
        }
    }
}

float Tween::apply_transition(float t, Transition tr, Ease e) {
    // Aplicar ease.
    auto apply_ease = [e](float x) {
        switch (e) {
            case Ease::In:    return x * x;
            case Ease::Out:   return 1 - (1 - x) * (1 - x);
            case Ease::InOut: return x < 0.5f ? 2.0f * x * x : 1.0f - static_cast<float>(std::pow(-2.0f * x + 2.0f, 2)) / 2.0f;
            case Ease::OutIn: return x < 0.5f ? 1.0f - static_cast<float>(std::pow(-2.0f * x + 2.0f, 2)) / 2.0f : 2.0f * x * x;
        }
        return x;
    };
    float x = apply_ease(t);
    // Aplicar transition (curva base).
    switch (tr) {
        case Transition::Linear: return x;
        case Transition::Sine:   return 1 - std::cos(x * 3.14159265358979f * 0.5f);
        case Transition::Quint:  return x * x * x * x * x;
        case Transition::Quart:  return x * x * x * x;
        case Transition::Expo:   return x == 0 ? 0 : std::pow(2, 10 * (x - 1));
        case Transition::Circ:   return 1 - std::sqrt(1 - x * x);
        case Transition::Cubic:  return x * x * x;
        case Transition::Quad:   return x * x;
        case Transition::Bounce: {
            if (x < 1/2.75f) return 7.5625f * x * x;
            else if (x < 2/2.75f) { x -= 1.5f/2.75f; return 7.5625f * x * x + 0.75f; }
            else if (x < 2.5f/2.75f) { x -= 2.25f/2.75f; return 7.5625f * x * x + 0.9375f; }
            else { x -= 2.625f/2.75f; return 7.5625f * x * x + 0.984375f; }
        }
        case Transition::Back: {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1;
            return 1 + c3 * std::pow(x - 1, 3) + c1 * std::pow(x - 1, 2);
        }
        case Transition::Elastic: {
            if (x == 0) return 0;
            if (x == 1) return 1;
            const float c4 = (2 * 3.14159265358979f) / 3;
            return std::pow(2, -10 * x) * std::sin((x * 10 - 0.75f) * c4) + 1;
        }
    }
    return x;
}

} // namespace arx
