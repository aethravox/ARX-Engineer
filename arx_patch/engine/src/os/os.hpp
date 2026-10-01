// ==============================================================================
// src/os/os.hpp — Abstracción del sistema operativo.
// Una sola interfaz para Windows, Linux, Android y Web (Emscripten).
// ==============================================================================
#pragma once

#include "core/types.hpp"

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

namespace arx {

// ---- Input ------------------------------------------------------------------
enum class MouseButton : uint8_t { Left, Right, Middle, X1, X2 };
enum class Key : uint16_t {
    Unknown=0,
    A=65,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
    Num0=48,Num1,Num2,Num3,Num4,Num5,Num6,Num7,Num8,Num9,
    F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,
    Up=0x114, Down, Left, Right,
    Space=0x20, Enter=0x0D, Escape=0x1B, Tab=0x09, Backspace=0x08,
    Shift=0x130, Ctrl, Alt, Super,
    Plus=0xBB, Minus=0xBD, Comma=0xBC, Period=0xBE
};

struct InputEvent {
    enum class Kind { Key, Mouse, MouseButton, MouseMotion, Touch, Joystick } kind;
    bool  pressed   = false;
    bool  released  = false;
    bool  held      = false;
    Key   key       = Key::Unknown;
    Vector2 mouse_pos;
    Vector2 mouse_rel;
    MouseButton mouse_button = MouseButton::Left;
    float touch_pressure = 0.0f;
    int   touch_id       = -1;
};

// ---- Display ----------------------------------------------------------------
struct DisplayMode {
    int width;
    int height;
    int refresh_rate;
    int bits_per_pixel = 32;
};

struct MonitorInfo {
    int id;
    std::string name;
    int x, y;             // Posición virtual
    int width, height;    // Resolución
    bool primary;
};

// ---- Window -----------------------------------------------------------------
struct WindowCreateInfo {
    std::string title = "ARX Engine";
    int width  = 1280;
    int height = 720;
    bool fullscreen = false;
    bool resizable  = true;
    bool decorated  = true;
    bool visible    = true;
    int  samples    = 0;            // MSAA
    int  gl_major   = 3;
    int  gl_minor   = 3;
    bool gl_es      = false;        // GLES para web/android
};

class Window {
public:
    virtual ~Window() = default;
    virtual bool should_close() const = 0;
    virtual void poll_events()       = 0;
    virtual void swap_buffers()      = 0;
    virtual void make_current()      = 0;

    virtual Vector2i size() const    = 0;
    virtual Vector2i framebuffer_size() const = 0;
    virtual void set_size(int w, int h) = 0;
    virtual void set_title(const std::string& t) = 0;
    virtual void set_vsync(bool on) = 0;
    virtual void* native_handle() const = 0;

    // Input state (polling).
    virtual bool is_key_down(Key k) const = 0;
    virtual bool is_mouse_button_down(MouseButton b) const = 0;
    virtual Vector2 mouse_position() const = 0;

    // Eventos en cola (consumir antes de process del frame).
    virtual std::vector<InputEvent> drain_events() = 0;

    // Callback de resize.
    using ResizeCallback = std::function<void(int,int)>;
    void set_resize_callback(ResizeCallback cb) { on_resize_ = std::move(cb); }

protected:
    ResizeCallback on_resize_;
};

// ---- OS ---------------------------------------------------------------------
class MainLoop;  // forward-declare

class OS {
public:
    static OS* create();   // Factory según plataforma.

    virtual ~OS() = default;

    // Lifecycle
    virtual bool init() = 0;
    virtual void shutdown() = 0;
    virtual void run() = 0;   // Bloquea hasta que el main loop termine.

    // Main loop (el runtime/editor lo setea para que OS::run lo llame cada frame)
    virtual void set_main_loop(MainLoop* loop) { main_loop_ = loop; }
    MainLoop* get_main_loop() const { return main_loop_; }

    // Ventanas
    virtual std::unique_ptr<Window> create_window(const WindowCreateInfo& info) = 0;
    virtual std::vector<MonitorInfo> get_monitors() const = 0;

    // Sistema de archivos (rutas relativas al proyecto / al binario).
    virtual std::string get_executable_path() const = 0;
    virtual std::string get_user_data_dir() const = 0;     // Config/save
    virtual std::string get_cwd() const = 0;

    // Tiempo
    virtual uint64_t get_ticks_usec() const = 0;
    virtual uint64_t get_ticks_msec() const { return get_ticks_usec() / 1000; }
    virtual void delay_usec(uint64_t us) = 0;

    // Threads y cores
    virtual int get_processor_count() const = 0;

    // Locale
    virtual std::string get_locale() const = 0;

    // Clipboard
    virtual void set_clipboard(const std::string& text) = 0;
    virtual std::string get_clipboard() const = 0;

    //Salir
    void request_exit() { exit_requested_ = true; }
    bool is_exit_requested() const { return exit_requested_; }

protected:
    bool exit_requested_ = false;
    MainLoop* main_loop_ = nullptr;
};

} // namespace arx
