// ==============================================================================
// src/platforms/android/os_android.hpp — OS layer para Android.
//
// Usa EGL para crear contexto OpenGL ES 2.0 (compatible con ImGui backend OpenGL2).
// Usa android_native_app_glue para el entry point.
// Touch input → TouchMouseEmulator.
// Volumen → TouchMouseEmulator (click izquierdo/derecho + toggle).
// ==============================================================================
#pragma once

#include "os/os.hpp"
#include <EGL/egl.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>

struct android_app;

namespace arx {

class OSAndroid : public OS {
public:
    explicit OSAndroid(android_app* app);
    ~OSAndroid() override;

    bool init() override;
    void shutdown() override;

    // Window
    std::unique_ptr<Window> create_window(const WindowCreateInfo& info) override;

    // Events
    void poll_events() override;
    std::vector<InputEvent> drain_events() override;

    // Input
    bool is_key_down(Key k) const override;
    bool is_key_pressed(Key k) const override;
    bool is_key_released(Key k) const override;
    bool is_mouse_button_down(MouseButton b) const override;
    bool is_mouse_button_pressed(MouseButton b) const override;
    bool is_mouse_button_released(MouseButton b) const override;
    std::pair<float, float> get_mouse_position() const override;
    float get_mouse_wheel() const override;
    bool is_gamepad_connected(int pad) const override;
    float get_gamepad_axis(int pad, int axis) const override;
    bool is_gamepad_button_down(int pad, int button) const override;

    // Time
    double get_time() const override;
    void sleep(double seconds) const override;

    // System info
    std::string get_name() const override { return "Android"; }
    std::string get_user_data_dir() const override;
    std::string get_executable_path() const override;

    // Android-specific
    void on_app_cmd(int32_t cmd);
    int32_t on_input_event(AInputEvent* event);

private:
    android_app* app_ = nullptr;
    EGLDisplay display_ = EGL_NO_DISPLAY;
    EGLSurface surface_ = EGL_NO_SURFACE;
    EGLContext context_ = EGL_NO_CONTEXT;
    ANativeWindow* window_ = nullptr;
    int width_ = 0;
    int height_ = 0;

    // Event queue
    std::vector<InputEvent> events_;

    // Key state
    bool key_down_[512] = {};
    bool key_pressed_[512] = {};
    bool key_released_[512] = {};

    // Mouse state (from TouchMouseEmulator)
    bool mouse_down_[3] = {};
    bool mouse_pressed_[3] = {};
    bool mouse_released_[3] = {};
    float mouse_x_ = 0;
    float mouse_y_ = 0;
    float mouse_wheel_ = 0;

    bool init_egl();
    void destroy_egl();
    void process_touch(AInputEvent* event);
    void process_key(AInputEvent* event);

    // Mapear keycode Android → ARX Key
    int map_android_keycode(int keycode);
};

} // namespace arx
