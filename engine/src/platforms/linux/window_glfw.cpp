// ==============================================================================
// src/platforms/linux/window_glfw.cpp — Implementación GLFW de Window.
// ==============================================================================
#include "window_glfw.hpp"
#include "core/logging.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <unordered_map>

namespace arx {

std::vector<WindowGLFW*> WindowGLFW::all_windows_;

// Mapeo de teclas GLFW → ARX Key.
static Key glfw_key_to_arx(int k) {
    if (k >= 'A' && k <= 'Z') return static_cast<Key>(k);
    if (k >= '0' && k <= '9') return static_cast<Key>(k);
    switch (k) {
        case GLFW_KEY_SPACE:        return Key::Space;
        case GLFW_KEY_ENTER:        return Key::Enter;
        case GLFW_KEY_ESCAPE:       return Key::Escape;
        case GLFW_KEY_TAB:          return Key::Tab;
        case GLFW_KEY_BACKSPACE:    return Key::Backspace;
        case GLFW_KEY_UP:           return Key::Up;
        case GLFW_KEY_DOWN:         return Key::Down;
        case GLFW_KEY_LEFT:         return Key::Left;
        case GLFW_KEY_RIGHT:        return Key::Right;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:  return Key::Shift;
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:return Key::Ctrl;
        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_RIGHT_ALT:    return Key::Alt;
        case GLFW_KEY_LEFT_SUPER:
        case GLFW_KEY_RIGHT_SUPER:  return Key::Super;
        case GLFW_KEY_F1:           return Key::F1;
        case GLFW_KEY_F2:           return Key::F2;
        case GLFW_KEY_F3:           return Key::F3;
        case GLFW_KEY_F4:           return Key::F4;
        case GLFW_KEY_F5:           return Key::F5;
        case GLFW_KEY_F6:           return Key::F6;
        case GLFW_KEY_F7:           return Key::F7;
        case GLFW_KEY_F8:           return Key::F8;
        case GLFW_KEY_F9:           return Key::F9;
        case GLFW_KEY_F10:          return Key::F10;
        case GLFW_KEY_F11:          return Key::F11;
        case GLFW_KEY_F12:          return Key::F12;
        case GLFW_KEY_COMMA:        return Key::Comma;
        case GLFW_KEY_PERIOD:       return Key::Period;
        case GLFW_KEY_MINUS:        return Key::Minus;
        case GLFW_KEY_EQUAL:        return Key::Plus;
        default:                    return Key::Unknown;
    }
}

static int arx_button_to_glfw(MouseButton b) {
    switch (b) {
        case MouseButton::Left:   return GLFW_MOUSE_BUTTON_LEFT;
        case MouseButton::Right:  return GLFW_MOUSE_BUTTON_RIGHT;
        case MouseButton::Middle: return GLFW_MOUSE_BUTTON_MIDDLE;
        case MouseButton::X1:     return GLFW_MOUSE_BUTTON_4;
        case MouseButton::X2:     return GLFW_MOUSE_BUTTON_5;
    }
    return GLFW_MOUSE_BUTTON_LEFT;
}

WindowGLFW::~WindowGLFW() {
    if (handle_) {
        all_windows_.erase(std::remove(all_windows_.begin(), all_windows_.end(), this),
                           all_windows_.end());
        glfwDestroyWindow(handle_);
    }
}

bool WindowGLFW::create(const WindowCreateInfo& info) {
    glfwWindowHint(GLFW_RESIZABLE,    info.resizable);
    glfwWindowHint(GLFW_DECORATED,    info.decorated);
    glfwWindowHint(GLFW_VISIBLE,      info.visible);
    glfwWindowHint(GLFW_SAMPLES,      info.samples);
    if (info.gl_es) {
        glfwWindowHint(GLFW_CLIENT_API,         GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, info.gl_major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, info.gl_minor);
    } else {
        glfwWindowHint(GLFW_CLIENT_API,         GLFW_OPENGL_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, info.gl_major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, info.gl_minor);
        // Compatibility profile para OpenGL 2.1 (o si el flag está activado).
        // Core profile solo para OpenGL 3.3+.
        if (info.gl_compat || (info.gl_major < 3) || (info.gl_major == 3 && info.gl_minor < 2)) {
            // No pedir CORE_PROFILE → usa compatibility por defecto
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
        } else {
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    }

    GLFWmonitor* mon = info.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    handle_ = glfwCreateWindow(info.width, info.height, info.title.c_str(),
                               mon, nullptr);
    if (!handle_) {
        ARX_LOG_ERROR("GLFW: no se pudo crear la ventana");
        return false;
    }
    glfwMakeContextCurrent(handle_);
    glfwSetWindowUserPointer(handle_, this);

    // Callbacks
    glfwSetKeyCallback(handle_,         &WindowGLFW::on_key);
    glfwSetMouseButtonCallback(handle_, &WindowGLFW::on_mouse_button);
    glfwSetCursorPosCallback(handle_,   &WindowGLFW::on_cursor_pos);
    glfwSetScrollCallback(handle_,      &WindowGLFW::on_scroll);
    glfwSetWindowSizeCallback(handle_, &WindowGLFW::on_resize);
    glfwSetCharCallback(handle_,        &WindowGLFW::on_char);

    all_windows_.push_back(this);
    return true;
}

bool WindowGLFW::should_close() const {
    return glfwWindowShouldClose(handle_);
}

void WindowGLFW::poll_events() {
    last_mouse_pos_ = mouse_pos_;
    glfwPollEvents();
}

void WindowGLFW::swap_buffers() {
    glfwSwapBuffers(handle_);
}

void WindowGLFW::make_current() {
    glfwMakeContextCurrent(handle_);
}

Vector2i WindowGLFW::size() const {
    int w, h; glfwGetWindowSize(handle_, &w, &h);
    return {w, h};
}

Vector2i WindowGLFW::framebuffer_size() const {
    int w, h; glfwGetFramebufferSize(handle_, &w, &h);
    return {w, h};
}

void WindowGLFW::set_size(int w, int h) { glfwSetWindowSize(handle_, w, h); }
void WindowGLFW::set_title(const std::string& t) { glfwSetWindowTitle(handle_, t.c_str()); }
void WindowGLFW::set_vsync(bool on) { glfwSwapInterval(on ? 1 : 0); }
void* WindowGLFW::native_handle() const { return (void*)handle_; }

bool WindowGLFW::is_key_down(Key k) const {
    int sc = static_cast<int>(k);
    return glfwGetKey(handle_, sc) == GLFW_PRESS;
}

bool WindowGLFW::is_mouse_button_down(MouseButton b) const {
    return glfwGetMouseButton(handle_, arx_button_to_glfw(b)) == GLFW_PRESS;
}

Vector2 WindowGLFW::mouse_position() const {
    return mouse_pos_;
}

std::vector<InputEvent> WindowGLFW::drain_events() {
    auto out = std::move(events_);
    events_.clear();
    return out;
}

void WindowGLFW::on_key(GLFWwindow* w, int key, int, int action, int) {
    auto* self = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(w));
    if (!self) return;
    InputEvent ev;
    ev.kind     = InputEvent::Kind::Key;
    ev.key      = glfw_key_to_arx(key);
    ev.pressed  = action == GLFW_PRESS;
    ev.released = action == GLFW_RELEASE;
    ev.held     = action == GLFW_REPEAT;
    self->events_.push_back(ev);
}

void WindowGLFW::on_mouse_button(GLFWwindow* w, int button, int action, int) {
    auto* self = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(w));
    if (!self) return;
    InputEvent ev;
    ev.kind      = InputEvent::Kind::MouseButton;
    ev.pressed   = action == GLFW_PRESS;
    ev.released  = action == GLFW_RELEASE;
    ev.mouse_pos = self->mouse_pos_;
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:   ev.mouse_button = MouseButton::Left;   break;
        case GLFW_MOUSE_BUTTON_RIGHT:  ev.mouse_button = MouseButton::Right;  break;
        case GLFW_MOUSE_BUTTON_MIDDLE: ev.mouse_button = MouseButton::Middle; break;
        case GLFW_MOUSE_BUTTON_4:      ev.mouse_button = MouseButton::X1;     break;
        case GLFW_MOUSE_BUTTON_5:      ev.mouse_button = MouseButton::X2;     break;
    }
    self->events_.push_back(ev);
}

void WindowGLFW::on_cursor_pos(GLFWwindow* w, double x, double y) {
    auto* self = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(w));
    if (!self) return;
    Vector2 prev = self->mouse_pos_;
    self->mouse_pos_ = Vector2(static_cast<float>(x), static_cast<float>(y));
    InputEvent ev;
    ev.kind       = InputEvent::Kind::MouseMotion;
    ev.mouse_pos  = self->mouse_pos_;
    ev.mouse_rel  = self->mouse_pos_ - prev;
    self->events_.push_back(ev);
}

void WindowGLFW::on_scroll(GLFWwindow*, double, double) {}
void WindowGLFW::on_char(GLFWwindow*, unsigned int) {}

void WindowGLFW::on_resize(GLFWwindow* w, int width, int height) {
    auto* self = static_cast<WindowGLFW*>(glfwGetWindowUserPointer(w));
    if (!self || !self->on_resize_) return;
    self->on_resize_(width, height);
}

} // namespace arx
