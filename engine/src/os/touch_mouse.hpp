// ==============================================================================
// src/os/touch_mouse.hpp — Emulador de mouse para pantallas táctiles.
//
// En Android (y opcionalmente Linux touch):
//   - Arrastrar 1 dedo = mover cursor (delta, no absoluto)
//   - Volumen arriba solo = click izquierdo
//   - Volumen abajo solo = click derecho
//   - Volumen arriba + abajo juntos = toggle on/off
//   - Cuando está off, volumen funciona normal (subir/bajar audio)
//
// El cursor se dibuja como un punto/cruz en pantalla.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include <cstdint>

namespace arx {

class TouchMouseEmulator {
public:
    static TouchMouseEmulator& instance();

    // Inicializar con tamaño de pantalla
    void init(int screen_w, int screen_h);

    // Procesar evento de touch (1 dedo)
    // action: 0=down, 1=move, 2=up
    // x, y: posición del dedo (en pixels)
    void on_touch(int action, float x, float y);

    // Procesar botón de volumen
    // button: 0=volume_up, 1=volume_down
    // pressed: true=presionado, false=soltado
    void on_volume_button(int button, bool pressed);

    // Llamar cada frame para actualizar estado
    // Retorna true si el modo mouse está activo
    void update(float delta);

    // === Estado del mouse emulado ===
    bool is_active() const { return active_; }
    Vector2 get_cursor_pos() const { return {cursor_x_, cursor_y_}; }
    bool is_left_down() const { return left_down_; }
    bool is_left_pressed() const { return left_pressed_; }
    bool is_right_down() const { return right_down_; }
    bool is_right_pressed() const { return right_pressed_; }

    // Consumir clicks (para que no se repitan)
    void consume_left() { left_pressed_ = false; }
    void consume_right() { right_pressed_ = false; }

    // Sensibilidad del cursor (multiplicador del delta)
    void set_sensitivity(float s) { sensitivity_ = s; }

    // Activar/desactivar manualmente
    void set_active(bool a) { active_ = a; }

    // Cursor visible
    bool is_cursor_visible() const { return active_; }

private:
    TouchMouseEmulator() = default;

    bool active_ = false;          // modo mouse on/off
    bool vol_up_held_ = false;     // volumen arriba está siendo presionado
    bool vol_down_held_ = false;   // volumen abajo está siendo presionado
    bool both_was_held_ = false;   // ambos fueron presionados (para detectar toggle)

    bool left_down_ = false;       // click izquierdo sostenido
    bool left_pressed_ = false;    // click izquierdo justo este frame
    bool right_down_ = false;      // click derecho sostenido
    bool right_pressed_ = false;   // click derecho justo este frame

    float cursor_x_ = 0;          // posición del cursor
    float cursor_y_ = 0;
    float sensitivity_ = 1.5f;     // multiplicador de delta

    // Touch tracking
    bool touching_ = false;
    float last_touch_x_ = 0;
    float last_touch_y_ = 0;

    int screen_w_ = 1280;
    int screen_h_ = 720;

    float toggle_cooldown_ = 0;    // cooldown para evitar toggle rápido
};

} // namespace arx
