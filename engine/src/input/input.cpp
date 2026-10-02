// ==============================================================================
// src/input/input.cpp
// ==============================================================================
#include "input.hpp"
#include "core/logging.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace arx {

// ===================== InputMap ==============================================
InputMap& InputMap::instance() {
    static InputMap inst;
    return inst;
}

void InputMap::add_action(const std::string& name) {
    if (actions_.find(name) == actions_.end()) actions_[name] = {};
}

void InputMap::remove_action(const std::string& name) {
    actions_.erase(name);
}

bool InputMap::has_action(const std::string& name) const {
    return actions_.find(name) != actions_.end();
}

void InputMap::add_binding(const std::string& action, const InputBinding& b) {
    add_action(action);
    actions_[action].push_back(b);
}

void InputMap::clear_bindings(const std::string& action) {
    auto it = actions_.find(action);
    if (it != actions_.end()) it->second.clear();
}

std::vector<InputBinding> InputMap::get_bindings(const std::string& action) const {
    auto it = actions_.find(action);
    if (it == actions_.end()) return {};
    return it->second;
}

std::vector<std::string> InputMap::get_actions() const {
    std::vector<std::string> out;
    out.reserve(actions_.size());
    for (const auto& [name, _] : actions_) out.push_back(name);
    return out;
}

bool InputMap::load_from_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty() || line[0] == '#') continue;
        // Formato: action_name = device:code[,device:code...]
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string action = line.substr(0, eq);
        // trim
        while (!action.empty() && std::isspace(action.back())) action.pop_back();
        std::string rest = line.substr(eq + 1);

        std::stringstream ss(rest);
        std::string token;
        while (std::getline(ss, token, ',')) {
            // trim
            while (!token.empty() && std::isspace(token.front())) token.erase(0, 1);
            while (!token.empty() && std::isspace(token.back()))  token.pop_back();
            size_t colon = token.find(':');
            if (colon == std::string::npos) continue;
            std::string dev = token.substr(0, colon);
            std::string code_s = token.substr(colon + 1);
            InputBinding b;
            if (dev == "keyboard")  b.device = InputDevice::Keyboard;
            else if (dev == "mouse") b.device = InputDevice::Mouse;
            else if (dev == "gamepad") b.device = InputDevice::Gamepad;
            else if (dev == "touch") b.device = InputDevice::Touch;
            b.code = std::atoi(code_s.c_str());
            add_binding(action, b);
        }
    }
    return true;
}

// ===================== Input =================================================
Input::State& Input::state() {
    static State s;
    return s;
}

void Input::begin_frame() {
    auto& s = state();
    s.keys_pressed_this_frame.clear();
    s.keys_released_this_frame.clear();
    s.mouse_buttons_pressed.clear();
    s.mouse_buttons_released.clear();
    s.mouse_delta = Vector2(0, 0);

    // Reset action transitions.
    for (auto& [name, st] : s.actions) {
        st.pressed  = false;
        st.released = false;
        st.prev_value = st.value;
    }
}

void Input::end_frame() {}

void Input::process_event(const InputEvent& ev) {
    auto& s = state();
    int code = static_cast<int>(ev.key);

    switch (ev.kind) {
        case InputEvent::Kind::Key:
            if (ev.pressed) {
                if (!s.keys_down[code]) {
                    s.keys_pressed_this_frame[code] = true;
                    s.keys_down[code] = true;
                }
            } else if (ev.released) {
                s.keys_down[code] = false;
                s.keys_released_this_frame[code] = true;
            }
            break;
        case InputEvent::Kind::MouseButton:
            code = static_cast<int>(ev.mouse_button);
            if (ev.pressed) {
                if (!s.mouse_buttons_down[code]) {
                    s.mouse_buttons_pressed[code] = true;
                    s.mouse_buttons_down[code] = true;
                }
            } else if (ev.released) {
                s.mouse_buttons_down[code] = false;
                s.mouse_buttons_released[code] = true;
            }
            break;
        case InputEvent::Kind::MouseMotion:
            s.mouse_delta += ev.mouse_rel;
            s.mouse_pos = ev.mouse_pos;
            break;
        case InputEvent::Kind::Touch:
            s.touches[ev.touch_id] = ev.mouse_pos;
            break;
        default:
            break;
    }

    // Actualizar acciones del InputMap.
    auto& map = InputMap::instance();
    for (const auto& action_name : map.get_actions()) {
        bool is_down = false;
        for (const auto& b : map.get_bindings(action_name)) {
            int bcode = b.code;
            if (b.device == InputDevice::Keyboard) {
                if (s.keys_down.count(bcode) && s.keys_down[bcode]) { is_down = true; break; }
            } else if (b.device == InputDevice::Mouse) {
                if (s.mouse_buttons_down.count(bcode) && s.mouse_buttons_down[bcode]) { is_down = true; break; }
            }
        }
        auto& st = s.actions[action_name];
        if (is_down && !st.held) {
            st.pressed = true;
            st.held    = true;
            st.value   = 1.0f;
        } else if (!is_down && st.held) {
            st.released = true;
            st.held     = false;
            st.value    = 0.0f;
        }
    }
}

bool Input::is_action_pressed(const std::string& action) {
    auto& s = state();
    auto it = s.actions.find(action);
    return it != s.actions.end() && it->second.held;
}

bool Input::is_action_just_pressed(const std::string& action) {
    auto& s = state();
    auto it = s.actions.find(action);
    return it != s.actions.end() && it->second.pressed;
}

bool Input::is_action_just_released(const std::string& action) {
    auto& s = state();
    auto it = s.actions.find(action);
    return it != s.actions.end() && it->second.released;
}

float Input::get_action_strength(const std::string& action) {
    auto& s = state();
    auto it = s.actions.find(action);
    return it != s.actions.end() ? it->second.value : 0.0f;
}

bool Input::is_key_down(Key k) {
    auto& s = state();
    int code = static_cast<int>(k);
    return s.keys_down.count(code) && s.keys_down[code];
}
bool Input::is_key_pressed(Key k) {
    auto& s = state();
    return s.keys_pressed_this_frame.count(static_cast<int>(k)) &&
           s.keys_pressed_this_frame[static_cast<int>(k)];
}
bool Input::is_key_released(Key k) {
    auto& s = state();
    return s.keys_released_this_frame.count(static_cast<int>(k)) &&
           s.keys_released_this_frame[static_cast<int>(k)];
}

bool Input::is_mouse_button_down(MouseButton b) {
    auto& s = state();
    int code = static_cast<int>(b);
    return s.mouse_buttons_down.count(code) && s.mouse_buttons_down[code];
}
bool Input::is_mouse_button_pressed(MouseButton b) {
    auto& s = state();
    int code = static_cast<int>(b);
    return s.mouse_buttons_pressed.count(code) && s.mouse_buttons_pressed[code];
}
bool Input::is_mouse_button_released(MouseButton b) {
    auto& s = state();
    int code = static_cast<int>(b);
    return s.mouse_buttons_released.count(code) && s.mouse_buttons_released[code];
}

Vector2 Input::get_mouse_position() {
    return state().mouse_pos;
}
Vector2 Input::get_mouse_delta() {
    return state().mouse_delta;
}

Vector2 Input::get_joy_axis(int joy, int axis) {
    auto& s = state();
    auto it = s.joy_axes.find(joy);
    if (it == s.joy_axes.end() || axis >= (int)it->second.size()) return {};
    return { it->second[axis * 2], it->second[axis * 2 + 1] };
}

float Input::get_joy_axis_value(int joy, int axis) {
    auto it = state().joy_axes.find(joy);
    if (it == state().joy_axes.end() || axis >= (int)it->second.size()) return 0.0f;
    return it->second[axis];
}

bool Input::is_joy_button_pressed(int joy, int button) {
    auto it = state().joy_buttons.find(joy);
    if (it == state().joy_buttons.end()) return false;
    auto bit = it->second.find(button);
    return bit != it->second.end() && bit->second;
}

Vector2 Input::get_touch_position(int idx) {
    auto it = state().touches.find(idx);
    return it != state().touches.end() ? it->second : Vector2{};
}

bool Input::is_touch_pressed(int idx) {
    return state().touches.find(idx) != state().touches.end();
}

} // namespace arx
