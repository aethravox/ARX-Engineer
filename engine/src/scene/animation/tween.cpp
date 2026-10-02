// ==============================================================================
#include "core/variant.hpp"
// src/scene/animation/tween.cpp
// ==============================================================================
#include "tween.hpp"
#include "core/object.hpp"
#include "core/math.hpp"
#include "core/logging.hpp"

#include <cmath>

namespace arx {

// ===================== Tween =================================================
Tween& Tween::tween_property(Object* obj, const std::string& prop,
                                const Variant& target, float duration) {
    Step s;
    s.object   = obj;
    s.property = prop;
    s.target   = target;
    s.duration = duration;
    if (obj) s.from = obj->get_property(sid(prop.c_str()));
    steps_.push_back(s);
    return *this;
}

Tween& Tween::tween_method(std::function<void(float)> cb, float from, float to,
                              float duration) {
    Step s;
    s.is_method        = true;
    s.method_callback  = std::move(cb);
    s.method_from      = from;
    s.method_to        = to;
    s.duration         = duration;
    steps_.push_back(s);
    return *this;
}

void Tween::play()  { running_ = true; paused_ = false; }
void Tween::pause() { paused_  = true; }
void Tween::stop()  { running_ = false; paused_ = false; total_elapsed_ = 0.0f; }
void Tween::kill()  { running_ = false; steps_.clear(); }

float Tween::get_progress() const {
    if (steps_.empty()) return 1.0f;
    float max_dur = 0.0f;
    for (const auto& s : steps_) max_dur = std::max(max_dur, s.duration + s.elapsed);
    return std::clamp(total_elapsed_ / max_dur, 0.0f, 1.0f);
}

void Tween::process(float delta) {
    if (!running_ || paused_ || steps_.empty()) return;

    total_elapsed_ += delta;

    // En modo paralelo, todos los steps avanzan juntos. En secuencia, uno a uno.
    if (parallel_) {
        for (auto& s : steps_) {
            s.elapsed += delta;
            float t = std::clamp(s.elapsed / s.duration, 0.0f, 1.0f);
            float e = apply_easing_(easing_, t);

            if (s.is_method) {
                float v = s.method_from + (s.method_to - s.method_from) * e;
                if (s.method_callback) s.method_callback(v);
            } else if (s.object) {
                Variant v = s.from;
                // Interpolación simple por tipo.
                if (s.from.get_type() == Variant::Type::Float) {
                    double f = s.from.to_float();
                    double t2 = s.target.to_float();
                    v = Variant(f + (t2 - f) * e);
                } else if (s.from.get_type() == Variant::Type::Vector2) {
                    Vector2 a = s.from.to_vector2();
                    Vector2 b = s.target.to_vector2();
                    v = Variant(math::lerp(a, b, e));
                } else if (s.from.get_type() == Variant::Type::Vector3) {
                    Vector3 a = s.from.to_vector3();
                    Vector3 b = s.target.to_vector3();
                    v = Variant(math::lerp(a, b, e));
                } else if (s.from.get_type() == Variant::Type::Color) {
                    Color a = s.from.to_color();
                    Color b = s.target.to_color();
                    v = Variant(Color(
                        math::lerp(a.r, b.r, e),
                        math::lerp(a.g, b.g, e),
                        math::lerp(a.b, b.b, e),
                        math::lerp(a.a, b.a, e)
                    ));
                }
                s.object->set_property(sid(s.property.c_str()), v);
            }
        }
    } else {
        // Secuencial: avanza el primer step no completado.
        for (auto& s : steps_) {
            if (s.elapsed >= s.duration) continue;
            s.elapsed += delta;
            float t = std::clamp(s.elapsed / s.duration, 0.0f, 1.0f);
            float e = apply_easing_(easing_, t);
            if (s.is_method) {
                float v = s.method_from + (s.method_to - s.method_from) * e;
                if (s.method_callback) s.method_callback(v);
            }
            break;
        }
    }

    // ¿Terminó?
    bool all_done = true;
    for (const auto& s : steps_) {
        if (s.elapsed < s.duration) { all_done = false; break; }
    }
    if (all_done) {
        if (trans_ == TransitionMode::Loop) {
            for (auto& s : steps_) s.elapsed = 0.0f;
            total_elapsed_ = 0.0f;
        } else if (trans_ == TransitionMode::PingPong) {
            // TODO: revertir
            for (auto& s : steps_) s.elapsed = 0.0f;
            total_elapsed_ = 0.0f;
        } else {
            running_ = false;
            if (on_complete_) on_complete_();
        }
    }
}

float Tween::apply_easing_(Ease e, float t) {
    auto in_out = [](float (*f)(float), float x) {
        return x < 0.5f ? f(x * 2.0f) * 0.5f
                         : 1.0f - f((1.0f - x) * 2.0f) * 0.5f;
    };
    auto quad  = [](float x){ return x*x; };
    auto cubic = [](float x){ return x*x*x; };
    auto quart = [](float x){ return x*x*x*x; };
    auto quint = [](float x){ return x*x*x*x*x; };
    auto sine  = [](float x){ return 1.0f - std::cos(x * 3.14159265358979f * 0.5f); };
    auto expo  = [](float x){ return x == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * (x - 1.0f)); };
    auto circ  = [](float x){ return 1.0f - std::sqrt(1.0f - x*x); };
    auto back  = [](float x){
        const float c1 = 1.70158f, c3 = c1 + 1.0f;
        return 1.0f + c3 * std::pow(x - 1.0f, 3.0f) + c1 * std::pow(x - 1.0f, 2.0f);
    };
    auto bounce_out = [](float x){
        const float n1 = 7.5625f, d1 = 2.75f;
        if (x < 1.0f/d1)       return n1 * x * x;
        else if (x < 2.0f/d1) { x -= 1.5f/d1; return n1 * x * x + 0.75f; }
        else if (x < 2.5f/d1) { x -= 2.25f/d1; return n1 * x * x + 0.9375f; }
        else                   { x -= 2.625f/d1; return n1 * x * x + 0.984375f; }
    };
    auto elastic = [](float x){
        if (x == 0.0f) return 0.0f;
        if (x == 1.0f) return 1.0f;
        const float c4 = (2.0f * 3.14159265358979f) / 3.0f;
        return std::pow(2.0f, -10.0f * x) * std::sin((x * 10.0f - 0.75f) * c4) + 1.0f;
    };

    switch (e) {
        case Ease::Linear: return t;
        case Ease::InQuad:     return quad(t);
        case Ease::OutQuad:    return 1.0f - quad(1.0f - t);
        case Ease::InOutQuad:  return in_out(quad, t);
        case Ease::InCubic:    return cubic(t);
        case Ease::OutCubic:   return 1.0f - cubic(1.0f - t);
        case Ease::InOutCubic: return in_out(cubic, t);
        case Ease::InQuart:    return quart(t);
        case Ease::OutQuart:   return 1.0f - quart(1.0f - t);
        case Ease::InOutQuart: return in_out(quart, t);
        case Ease::InQuint:    return quint(t);
        case Ease::OutQuint:   return 1.0f - quint(1.0f - t);
        case Ease::InOutQuint: return in_out(quint, t);
        case Ease::InSine:     return 1.0f - sine(1.0f - t);
        case Ease::OutSine:    return sine(t);
        case Ease::InOutSine:  return 0.5f * (1.0f - std::cos(3.14159265358979f * t));
        case Ease::InExpo:     return expo(t);
        case Ease::OutExpo:    return 1.0f - expo(1.0f - t);
        case Ease::InOutExpo:  return in_out(expo, t);
        case Ease::InCirc:     return circ(t);
        case Ease::OutCirc:    return 1.0f - circ(1.0f - t);
        case Ease::InOutCirc:  return in_out(circ, t);
        case Ease::InElastic:  return elastic(t);
        case Ease::OutElastic: return 1.0f - elastic(1.0f - t);
        case Ease::InOutElastic:return in_out(elastic, t);
        case Ease::InBack:     return back(t);
        case Ease::OutBack:    {
            const float c1 = 1.70158f, c3 = c1 + 1.0f;
            return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
        }
        case Ease::InOutBack:  return in_out(back, t);
        case Ease::InBounce:   return 1.0f - bounce_out(1.0f - t);
        case Ease::OutBounce:  return bounce_out(t);
        case Ease::InOutBounce:return t < 0.5f
            ? (1.0f - bounce_out(1.0f - 2.0f * t)) * 0.5f
            : bounce_out(2.0f * t - 1.0f) * 0.5f + 0.5f;
    }
    return t;
}

// ===================== Timer =================================================
void Timer::start() {
    running_   = true;
    time_left_ = wait_time_;
}
void Timer::stop() {
    running_  = false;
    time_left_ = 0.0f;
}

void Timer::process(float delta) {
    if (!running_) return;
    time_left_ -= delta;
    if (time_left_ <= 0.0f) {
        emit_signal(sid("timeout"));
        if (one_shot_) running_ = false;
        time_left_ = wait_time_;
    }
}

} // namespace arx
