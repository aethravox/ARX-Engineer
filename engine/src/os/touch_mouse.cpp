// ==============================================================================
// src/os/touch_mouse.cpp — Emulador de mouse para pantallas táctiles.
// ==============================================================================
#include "touch_mouse.hpp"
#include <cmath>
#include <algorithm>

namespace arx {

TouchMouseEmulator& TouchMouseEmulator::instance() {
    static TouchMouseEmulator inst;
    return inst;
}

void TouchMouseEmulator::init(int screen_w, int screen_h) {
    screen_w_ = screen_w;
    screen_h_ = screen_h;
    cursor_x_ = screen_w * 0.5f;
    cursor_y_ = screen_h * 0.5f;
}

void TouchMouseEmulator::on_touch(int action, float x, float y) {
    if (!active_) return;

    if (action == 0) {
        // Touch down
        touching_ = true;
        last_touch_x_ = x;
        last_touch_y_ = y;
    } else if (action == 1) {
        // Touch move
        if (touching_) {
            float dx = (x - last_touch_x_) * sensitivity_;
            float dy = (y - last_touch_y_) * sensitivity_;
            cursor_x_ += dx;
            cursor_y_ += dy;
            // Clamp a pantalla
            cursor_x_ = std::clamp(cursor_x_, 0.0f, (float)screen_w_);
            cursor_y_ = std::clamp(cursor_y_, 0.0f, (float)screen_h_);
            last_touch_x_ = x;
            last_touch_y_ = y;
        }
    } else if (action == 2) {
        // Touch up
        touching_ = false;
    }
}

void TouchMouseEmulator::on_volume_button(int button, bool pressed) {
    if (button == 0) {
        // Volume up
        vol_up_held_ = pressed;
    } else if (button == 1) {
        // Volume down
        vol_down_held_ = pressed;
    }

    // Detectar toggle: ambos presionados
    bool both = vol_up_held_ && vol_down_held_;
    if (both && !both_was_held_ && toggle_cooldown_ <= 0) {
        // Toggle!
        active_ = !active_;
        both_was_held_ = true;
        toggle_cooldown_ = 0.5f;  // 500ms cooldown
        return;
    }
    if (!both) {
        both_was_held_ = false;
    }

    // Si no estamos en modo toggle, no procesar clicks
    if (!active_ || both) return;

    // Click izquierdo = volume up
    if (button == 0) {
        if (pressed) {
            left_down_ = true;
            left_pressed_ = true;
        } else {
            left_down_ = false;
        }
    }

    // Click derecho = volume down
    if (button == 1) {
        if (pressed) {
            right_down_ = true;
            right_pressed_ = true;
        } else {
            right_down_ = false;
        }
    }
}

void TouchMouseEmulator::update(float delta) {
    // Decrementar cooldown
    if (toggle_cooldown_ > 0) {
        toggle_cooldown_ -= delta;
        if (toggle_cooldown_ < 0) toggle_cooldown_ = 0;
    }
}

} // namespace arx
