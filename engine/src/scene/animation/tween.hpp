// ==============================================================================
// src/scene/animation/tween.hpp — Animación de properties por interpolación.
// ==============================================================================
#pragma once

#include "scene/vox.hpp"
#include "core/types.hpp"
#include "core/variant.hpp"

#include <vector>
#include <functional>
#include <memory>

namespace arx {

class Object;

// Tween: animación programática de properties.
// Ej:
//   tween_property(node, "position", Vector3(10,0,0), 1.0f)
//     .set_easing(Ease::InOutCubic)
//     .set_delay(0.5f);
class Tween : public Vox {
public:
    ARX_CLASS(Tween, Vox);
public:

    enum class Ease {
        Linear,
        InQuad, OutQuad, InOutQuad,
        InCubic, OutCubic, InOutCubic,
        InQuart, OutQuart, InOutQuart,
        InQuint, OutQuint, InOutQuint,
        InSine, OutSine, InOutSine,
        InExpo, OutExpo, InOutExpo,
        InCirc, OutCirc, InOutCirc,
        InElastic, OutElastic, InOutElastic,
        InBack, OutBack, InOutBack,
        InBounce, OutBounce, InOutBounce
    };

    enum class TransitionMode { Once, Loop, PingPong };

    // Builder API.
    Tween& tween_property(Object* obj, const std::string& prop,
                            const Variant& target, float duration);
    Tween& tween_method(std::function<void(float)> callback, float from, float to,
                          float duration);
    Tween& set_easing(Ease e) { easing_ = e; return *this; }
    Tween& set_delay(float d) { delay_ = d; return *this; }
    Tween& set_trans(TransitionMode m) { trans_ = m; return *this; }
    Tween& set_parallel(bool p) { parallel_ = p; return *this; }

    // Control.
    void play();
    void pause();
    void stop();
    void kill();

    bool is_running() const { return running_; }
    bool is_paused()  const { return paused_; }
    float get_progress() const;

    // Callbacks.
    void on_complete(std::function<void()> cb) { on_complete_ = std::move(cb); }

    void process(float delta) override;

private:
    struct Step {
        Object*                  object   = nullptr;
        std::string              property;
        Variant                  from;
        Variant                  target;
        float                    duration;
        float                    elapsed = 0.0f;
        std::function<void(float)> method_callback;
        float                    method_from = 0.0f;
        float                    method_to   = 0.0f;
        bool                     is_method   = false;
    };

    std::vector<Step> steps_;
    Ease              easing_    = Ease::Linear;
    float             delay_     = 0.0f;
    TransitionMode    trans_     = TransitionMode::Once;
    bool              parallel_  = false;
    bool              running_   = false;
    bool              paused_    = false;
    float             total_elapsed_ = 0.0f;
    std::function<void()> on_complete_;

    static float apply_easing_(Ease e, float t);
};

// ==============================================================================
// Timer — disparador de timeouts.
// ==============================================================================
class Timer : public Vox {
public:
    ARX_CLASS(Timer, Vox);
public:

    void set_wait_time(float t) { wait_time_ = t; }
    float get_wait_time() const { return wait_time_; }

    void set_one_shot(bool o) { one_shot_ = o; }
    bool is_one_shot() const { return one_shot_; }

    void set_autostart(bool a) { autostart_ = a; if (a) start(); }
    bool is_autostart() const { return autostart_; }

    void start();
    void stop();
    bool is_stopped() const { return !running_; }

    float get_time_left() const { return time_left_; }
    float get_progress() const {
        return wait_time_ > 0 ? 1.0f - (time_left_ / wait_time_) : 0.0f;
    }

    // Signals: timeout, started, stopped

    void process(float delta) override;

private:
    float wait_time_  = 1.0f;
    bool  one_shot_   = false;
    bool  autostart_  = false;
    bool  running_    = false;
    float time_left_  = 0.0f;
};

} // namespace arx
