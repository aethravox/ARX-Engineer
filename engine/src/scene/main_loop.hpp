// ==============================================================================
// src/scene/main_loop.hpp — bucle principal del motor.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"

#include <memory>
#include <vector>

namespace arx {

struct InputEvent;
class  Window;
class  SceneTree;

// MainLoop: el bucle principal. Es abstracto; el editor y el runtime
// implementan subclases concretas (EditorMainLoop y RuntimeMainLoop).
class MainLoop : public Object {
public:
    ARX_CLASS(MainLoop, Object);
public:

    virtual void start()    {}
    virtual bool process(float delta) = 0;   // devuelve false => salir
    virtual void finish()   {}

    void set_window(Window* w) { window_ = w; }
    Window* get_window() const { return window_; }

    // Estadísticas del último frame (para el editor).
    struct Stats {
        float  delta_seconds  = 0.0f;
        float  fps            = 0.0f;
        int    draw_calls     = 0;
        int    vertices       = 0;
        uint64_t frame_usec   = 0;
    };
    const Stats& get_stats() const { return stats_; }

protected:
    Window* window_ = nullptr;
    Stats   stats_;
};

} // namespace arx
