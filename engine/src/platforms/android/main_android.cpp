// ==============================================================================
// src/platforms/android/main_android.cpp — Entry point para Android.
//
// Usa android_native_app_glue. Crea OSAndroid, inicializa el motor,
// y corre el main loop.
// ==============================================================================
#include <android_native_app_glue.h>
#include <android/log.h>

#include "os_android.hpp"
#include "core/logging.hpp"
#include "render/renderer.hpp"
#include "scene/scene_tree.hpp"

#include <imgui.h>
#include <imgui_impl_opengl2.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ARX", __VA_ARGS__))

static arx::OSAndroid* g_os = nullptr;

static void handle_app_cmd(struct android_app* app, int32_t cmd) {
    if (g_os) g_os->on_app_cmd(cmd);
}

static int32_t handle_input_event(struct android_app* app, AInputEvent* event) {
    if (g_os) return g_os->on_input_event(event);
    return 0;
}

extern "C" void android_main(struct android_app* app) {
    LOGI("ARX Engine Android starting...");

    // Crear OS
    g_os = new arx::OSAndroid(app);
    g_os->init();

    app->onAppCmd = handle_app_cmd;
    app->onInputEvent = handle_input_event;

    // Esperar a que la ventana se inicialice
    LOGI("Esperando ventana...");
    while (!g_os->get_window_ready()) {
        int events;
        struct android_poll_source* source;
        while (ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(app, source);
            if (app->destroyRequested) break;
        }
    }
    LOGI("Ventana lista!");

    // Crear ventana + renderer
    arx::WindowCreateInfo wci;
    wci.title = "ARX Engine";
    wci.width = g_os->get_width();
    wci.height = g_os->get_height();
    wci.gl_major = 2;
    wci.gl_minor = 0;  // OpenGL ES 2.0
    auto window = g_os->create_window(wci);

    auto renderer = arx::Renderer::create(arx::Renderer::Backend::OpenGL);
    arx::RendererConfig rc;
    rc.viewport_w = wci.width;
    rc.viewport_h = wci.height;
    renderer->init(rc);

    LOGI("Renderer inicializado: %dx%d", wci.width, wci.height);

    // Main loop
    bool running = true;
    while (running) {
        // Poll events
        int events;
        struct android_poll_source* source;
        while (ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(app, source);
            if (app->destroyRequested) {
                running = false;
                break;
            }
        }

        // Render
        renderer->begin_frame();
        renderer->clear(0.1f, 0.1f, 0.12f, 1.0f);

        // TODO: Editor UI aquí

        renderer->end_frame();
        window->swap_buffers();
    }

    LOGI("ARX Engine Android shutting down...");
    delete g_os;
    g_os = nullptr;
}
