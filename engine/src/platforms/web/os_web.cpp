// ==============================================================================
// src/platforms/web/os_web.cpp
// ==============================================================================
#include "os_web.hpp"
#include "scene/main_loop.hpp"
#include "core/logging.hpp"

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/key_codes.h>

#include <GLES3/gl3.h>
#include <chrono>
#include <string>

namespace arx {

static MainLoop* g_loop = nullptr;

EM_BOOL web_iter(double, void*) {
    if (!g_loop) return EM_FALSE;
    static double last = emscripten_get_now();
    double now = emscripten_get_now();
    float delta = static_cast<float>((now - last) / 1000.0);
    last = now;
    if (!g_loop->process(delta)) {
        emscripten_cancel_main_loop();
        return EM_FALSE;
    }
    return EM_TRUE;
}

bool OSWeb::init() {
    ARX_LOG_INFO("OSWeb inicializado (Emscripten)");
    return true;
}

void OSWeb::shutdown() {
    ARX_LOG_INFO("OSWeb apagado");
}

void OSWeb::run() {
    if (!main_loop_) return;
    g_loop = main_loop_;
    main_loop_->start();
    emscripten_set_main_loop_arg([](void*){ web_iter(0, nullptr); }, nullptr,
                                  0 /*fps=vsync*/, 1 /*simulate_infinite_loop*/);
    main_loop_->finish();
}

std::unique_ptr<Window> OSWeb::create_window(const WindowCreateInfo& info) {
    // En web, la ventana la da el canvas HTML. No creamos nada aquí.
    // El renderer debe configurarse contra #canvas via emscripten_webgl_*.
    return nullptr;
}

std::string OSWeb::get_user_data_dir() const {
    return "/arx_user";
}

std::string OSWeb::get_cwd() const {
    return "/";
}

uint64_t OSWeb::get_ticks_usec() const {
    return static_cast<uint64_t>(emscripten_get_now() * 1000.0);
}

std::string OSWeb::get_locale() const {
    const char* l = emscripten_get_system_locale();
    return l ? std::string(l) : "en-US";
}

} // namespace arx
