// ==============================================================================
// src/platforms/linux/window_glfw.hpp
// Implementación de Window con GLFW. Sirve para Windows, Linux y macOS.
// En Android y Web hay que usar window_emscripten / window_android.
// ==============================================================================
#pragma once

#include "os/os.hpp"

struct GLFWwindow;

namespace arx {

class WindowGLFW : public Window {
public:
    WindowGLFW() = default;
    ~WindowGLFW() override;

    bool create(const WindowCreateInfo& info);

    bool should_close() const override;
    void poll_events() override;
    void swap_buffers() override;
    void make_current() override;

    Vector2i size() const override;
    Vector2i framebuffer_size() const override;
    void set_size(int w, int h) override;
    void set_title(const std::string& t) override;
    void set_vsync(bool on) override;
    void* native_handle() const override;

    bool is_key_down(Key k) const override;
    bool is_mouse_button_down(MouseButton b) const override;
    Vector2 mouse_position() const override;
    std::vector<InputEvent> drain_events() override;

    GLFWwindow* glfw_handle() const { return handle_; }

private:
    static void on_key(GLFWwindow*, int, int, int, int);
    static void on_mouse_button(GLFWwindow*, int, int, int);
    static void on_cursor_pos(GLFWwindow*, double, double);
    static void on_scroll(GLFWwindow*, double, double);
    static void on_resize(GLFWwindow*, int, int);
    static void on_char(GLFWwindow*, unsigned int);

    GLFWwindow* handle_ = nullptr;
    std::vector<InputEvent> events_;
    Vector2 mouse_pos_;
    Vector2 last_mouse_pos_;

    static std::vector<WindowGLFW*> all_windows_;   // Para dispatch de callbacks
};

} // namespace arx
