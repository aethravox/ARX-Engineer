// ==============================================================================
// src/platforms/android/os_android.cpp — OS layer para Android.
// ==============================================================================
#include "os_android.hpp"
#include "os/touch_mouse.hpp"
#include "core/logging.hpp"

#include <android/log.h>
#include <android/native_app_glue.h>
#include <unistd.h>
#include <sys/time.h>
#include <cstring>
#include <cmath>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ARX", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "ARX", __VA_ARGS__))

namespace arx {

OSAndroid::OSAndroid(android_app* app) : app_(app) {}

OSAndroid::~OSAndroid() {
    destroy_egl();
}

bool OSAndroid::init() {
    ARX_LOG_INFO("OSAndroid::init");
    return true;
}

void OSAndroid::shutdown() {
    destroy_egl();
    ARX_LOG_INFO("OSAndroid::shutdown");
}

bool OSAndroid::init_egl() {
    display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display_ == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }

    EGLint major, minor;
    if (!eglInitialize(display_, &major, &minor)) {
        LOGE("eglInitialize failed");
        return false;
    }
    LOGI("EGL %d.%d", major, minor);

    // Config: OpenGL ES 2.0
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display_, attribs, &config, 1, &numConfigs) || numConfigs < 1) {
        LOGE("eglChooseConfig failed");
        return false;
    }

    // Format del window
    EGLint format;
    eglGetConfigAttrib(display_, config, EGL_NATIVE_VISUAL_ID, &format);
    if (window_) {
        ANativeWindow_setBuffersGeometry(window_, 0, 0, format);
    }

    surface_ = eglCreateWindowSurface(display_, config, window_, nullptr);
    if (surface_ == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return false;
    }

    // Context OpenGL ES 2.0
    EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    context_ = eglCreateContext(display_, config, EGL_NO_CONTEXT, contextAttribs);
    if (context_ == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return false;
    }

    if (!eglMakeCurrent(display_, surface_, surface_, context_)) {
        LOGE("eglMakeCurrent failed");
        return false;
    }

    eglQuerySurface(display_, surface_, EGL_WIDTH, &width_);
    eglQuerySurface(display_, surface_, EGL_HEIGHT, &height_);
    LOGI("EGL surface: %dx%d", width_, height_);

    // Init touch mouse emulator
    TouchMouseEmulator::instance().init(width_, height_);
    // Activar por defecto en Android
    TouchMouseEmulator::instance().set_active(true);

    return true;
}

void OSAndroid::destroy_egl() {
    if (display_ != EGL_NO_DISPLAY) {
        eglMakeCurrent(display_, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context_ != EGL_NO_CONTEXT) eglDestroyContext(display_, context_);
        if (surface_ != EGL_NO_SURFACE) eglDestroySurface(display_, surface_);
        eglTerminate(display_);
    }
    display_ = EGL_NO_DISPLAY;
    context_ = EGL_NO_CONTEXT;
    surface_ = EGL_NO_SURFACE;
}

void OSAndroid::on_app_cmd(int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app_->window) {
                window_ = app_->window;
                init_egl();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            destroy_egl();
            window_ = nullptr;
            break;
        case APP_CMD_GAINED_FOCUS:
        case APP_CMD_LOST_FOCUS:
            break;
    }
}

int32_t OSAndroid::on_input_event(AInputEvent* event) {
    int type = AInputEvent_getType(event);
    if (type == AINPUT_EVENT_TYPE_MOTION) {
        process_touch(event);
        return 1;
    } else if (type == AINPUT_EVENT_TYPE_KEY) {
        process_key(event);
        return 1;
    }
    return 0;
}

void OSAndroid::process_touch(AInputEvent* event) {
    float x = AMotionEvent_getX(event, 0);
    float y = AMotionEvent_getY(event, 0);
    int action = AMotionEvent_getActionMasked(event);

    int touch_action = 2; // up
    if (action == AMOTION_EVENT_ACTION_DOWN) touch_action = 0;
    else if (action == AMOTION_EVENT_ACTION_MOVE) touch_action = 1;

    // Enviar al emulador de mouse
    TouchMouseEmulator::instance().on_touch(touch_action, x, y);

    // Actualizar posición del cursor emulado
    auto& tm = TouchMouseEmulator::instance();
    mouse_x_ = tm.get_cursor_pos().x;
    mouse_y_ = tm.get_cursor_pos().y;
}

void OSAndroid::process_key(AInputEvent* event) {
    int keycode = AKeyEvent_getKeyCode(event);
    int action = AKeyEvent_getAction(event);

    bool pressed = (action == AKEY_EVENT_ACTION_DOWN);
    bool released = (action == AKEY_EVENT_ACTION_UP);

    // Volumen → TouchMouseEmulator
    if (keycode == AKEYCODE_VOLUME_UP) {
        TouchMouseEmulator::instance().on_volume_button(0, pressed);
        // Actualizar mouse state desde el emulador
        auto& tm = TouchMouseEmulator::instance();
        mouse_down_[0] = tm.is_left_down();
        mouse_pressed_[0] = tm.is_left_pressed();
        return;
    }
    if (keycode == AKEYCODE_VOLUME_DOWN) {
        TouchMouseEmulator::instance().on_volume_button(1, pressed);
        auto& tm = TouchMouseEmulator::instance();
        mouse_down_[1] = tm.is_right_down();
        mouse_pressed_[1] = tm.is_right_pressed();
        return;
    }

    // Otras teclas → key state
    int arx_key = map_android_keycode(keycode);
    if (arx_key >= 0 && arx_key < 512) {
        if (pressed) {
            key_down_[arx_key] = true;
            key_pressed_[arx_key] = true;
        } else if (released) {
            key_down_[arx_key] = false;
            key_released_[arx_key] = true;
        }
    }
}

int OSAndroid::map_android_keycode(int keycode) {
    // Mapeo básico: A-Z, 0-9, espacios, etc.
    if (keycode >= AKEYCODE_A && keycode <= AKEYCODE_Z) {
        return 65 + (keycode - AKEYCODE_A); // ASCII A=65
    }
    if (keycode >= AKEYCODE_0 && keycode <= AKEYCODE_9) {
        return 48 + (keycode - AKEYCODE_0); // ASCII 0=48
    }
    switch (keycode) {
        case AKEYCODE_SPACE: return 32;
        case AKEYCODE_ENTER: return 257;
        case AKEYCODE_ESCAPE: return 256;
        case AKEYCODE_BACK: return 256;
        case AKEYCODE_DPAD_LEFT: return 263;
        case AKEYCODE_DPAD_RIGHT: return 262;
        case AKEYCODE_DPAD_UP: return 265;
        case AKEYCODE_DPAD_DOWN: return 264;
        case AKEYCODE_TAB: return 258;
        case AKEYCODE_DEL: return 259; // backspace
        default: return -1;
    }
}

// === OS interface ===

std::unique_ptr<Window> OSAndroid::create_window(const WindowCreateInfo& info) {
    // En Android, el window ya está creado por EGL
    // Retornar un dummy Window que wrappea el ANativeWindow
    class AndroidWindow : public Window {
        ANativeWindow* nw_;
        EGLDisplay display_;
        EGLSurface surface_;
        int w_, h_;
    public:
        AndroidWindow(ANativeWindow* nw, EGLDisplay d, EGLSurface s, int w, int h)
            : nw_(nw), display_(d), surface_(s), w_(w), h_(h) {}

        void poll_events() override {}
        std::vector<InputEvent> drain_events() override { return {}; }
        bool should_close() const override { return false; }
        void swap_buffers() override {
            if (surface_ != EGL_NO_SURFACE) {
                eglSwapBuffers(display_, surface_);
            }
        }
        void set_vsync(bool) override {}
        void set_title(const std::string&) override {}
        void* native_handle() override { return nw_; }
        int get_width() const override { return w_; }
        int get_height() const override { return h_; }
    };

    return std::make_unique<AndroidWindow>(window_, display_, surface_, width_, height_);
}

void OSAndroid::poll_events() {
    // Android events se procesan en on_input_event (callback)
    // Actualizar touch mouse
    TouchMouseEmulator::instance().update(0.016f);
}

std::vector<InputEvent> OSAndroid::drain_events() {
    auto ev = std::move(events_);
    events_.clear();
    return ev;
}

bool OSAndroid::is_key_down(Key k) const {
    int idx = static_cast<int>(k);
    return idx >= 0 && idx < 512 ? key_down_[idx] : false;
}

bool OSAndroid::is_key_pressed(Key k) const {
    int idx = static_cast<int>(k);
    return idx >= 0 && idx < 512 ? key_pressed_[idx] : false;
}

bool OSAndroid::is_key_released(Key k) const {
    int idx = static_cast<int>(k);
    return idx >= 0 && idx < 512 ? key_released_[idx] : false;
}

bool OSAndroid::is_mouse_button_down(MouseButton b) const {
    int idx = static_cast<int>(b);
    return idx >= 0 && idx < 3 ? mouse_down_[idx] : false;
}

bool OSAndroid::is_mouse_button_pressed(MouseButton b) const {
    int idx = static_cast<int>(b);
    return idx >= 0 && idx < 3 ? mouse_pressed_[idx] : false;
}

bool OSAndroid::is_mouse_button_released(MouseButton b) const {
    int idx = static_cast<int>(b);
    return idx >= 0 && idx < 3 ? mouse_released_[idx] : false;
}

std::pair<float, float> OSAndroid::get_mouse_position() const {
    return {mouse_x_, mouse_y_};
}

float OSAndroid::get_mouse_wheel() const { return mouse_wheel_; }

bool OSAndroid::is_gamepad_connected(int pad) const { return false; }
float OSAndroid::is_gamepad_axis(int pad, int axis) const { return 0; }
bool OSAndroid::is_gamepad_button_down(int pad, int button) const { return false; }

double OSAndroid::get_time() const {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec * 0.000001;
}

void OSAndroid::sleep(double seconds) const {
    usleep(seconds * 1000000);
}

std::string OSAndroid::get_user_data_dir() const {
    return "/data/data/com.aethravox.arxengine/files";
}

std::string OSAndroid::get_executable_path() const {
    return "/data/app/com.aethravox.arxengine";
}

} // namespace arx
