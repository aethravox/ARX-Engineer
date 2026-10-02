// ==============================================================================
// src/input/input.hpp — Sistema de input: Input, InputMap, acciones.
// ==============================================================================
#pragma once

#include "core/types.hpp"
#include "core/object.hpp"
#include "os/os.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace arx {

// Tipos de input.
enum class InputDevice : uint8_t {
    Keyboard, Mouse, Joystick, Touch, Gamepad
};

// Una "acción" puede estar mapeada a varios dispositivos/teclas.
// Ejemplo: "ui_left" → [keyboard:left, keyboard:a, gamepad:dpad_left]
struct InputBinding {
    InputDevice device = InputDevice::Keyboard;
    int         code   = 0;        // Key code, mouse button, joystick button.
    int         gamepad_index = 0; // Para gamepad.
    float       deadzone = 0.2f;   // Para ejes analógicos.
};

// Estado de una acción en este frame.
struct ActionState {
    bool pressed       = false;    // True justo este frame.
    bool released      = false;    // True justo este frame.
    bool held          = false;    // True mientras se mantiene.
    float value        = 0.0f;     // 0..1 (ejes analógicos).
    float prev_value   = 0.0f;
};

// InputMap — mapea nombres de acción a bindings.
class InputMap {
public:
    static InputMap& instance();

    void add_action(const std::string& name);
    void remove_action(const std::string& name);
    bool has_action(const std::string& name) const;

    void add_binding(const std::string& action, const InputBinding& b);
    void clear_bindings(const std::string& action);

    std::vector<InputBinding> get_bindings(const std::string& action) const;
    std::vector<std::string>  get_actions() const;

    // Cargar desde un archivo .arx (InputMap del proyecto).
    bool load_from_file(const std::string& path);

private:
    InputMap() = default;
    std::unordered_map<std::string, std::vector<InputBinding>> actions_;
};

// Input — estado del input en este frame, consultable desde ARXScript.
class Input : public Object {
public:
    ARX_CLASS(Input, Object);
public:

    // Llamar al inicio de cada frame.
    static void begin_frame();
    static void end_frame();
    static void process_event(const InputEvent& ev);

    // Queries estilo Godot.
    static bool is_action_pressed(const std::string& action);
    static bool is_action_just_pressed(const std::string& action);
    static bool is_action_just_released(const std::string& action);
    static float get_action_strength(const std::string& action);

    static bool is_key_down(Key k);
    static bool is_key_pressed(Key k);     // Just this frame
    static bool is_key_released(Key k);

    static bool is_mouse_button_down(MouseButton b);
    static bool is_mouse_button_pressed(MouseButton b);
    static bool is_mouse_button_released(MouseButton b);

    static Vector2 get_mouse_position();
    static Vector2 get_mouse_delta();

    static Vector2 get_joy_axis(int joy, int axis);
    static float   get_joy_axis_value(int joy, int axis);
    static bool    is_joy_button_pressed(int joy, int button);

    // Touch.
    static Vector2 get_touch_position(int idx);
    static bool    is_touch_pressed(int idx);

private:
    struct State {
        std::unordered_map<std::string, ActionState> actions;
        std::unordered_map<int, bool>                keys_down;
        std::unordered_map<int, bool>                keys_pressed_this_frame;
        std::unordered_map<int, bool>                keys_released_this_frame;
        std::unordered_map<int, bool>                mouse_buttons_down;
        std::unordered_map<int, bool>                mouse_buttons_pressed;
        std::unordered_map<int, bool>                mouse_buttons_released;
        Vector2 mouse_pos;
        Vector2 mouse_delta;
        std::unordered_map<int, Vector2>             touches;
        std::unordered_map<int, std::vector<float>>  joy_axes;
        std::unordered_map<int, std::unordered_map<int, bool>> joy_buttons;
    };
    static State& state();
};

} // namespace arx
