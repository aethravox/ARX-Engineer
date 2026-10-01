#include <glad.h>
// ==============================================================================
// src/platforms/linux/os_linux.cpp
// ==============================================================================
#include "os_linux.hpp"
#include "window_glfw.hpp"
#include "scene/main_loop.hpp"
#include "core/logging.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <thread>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fstream>
#include <filesystem>

namespace arx {

static uint64_t linux_now_usec() {
    auto now = std::chrono::high_resolution_clock::now();
    auto us  = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();
    return static_cast<uint64_t>(us);
}

bool OSLinux::init() {
    // Set GLFW error callback para ver el error real
    glfwSetErrorCallback([](int code, const char* desc) {
        ARX_LOG_FATAL("GLFW error {}: {}", code, desc ? desc : "(null)");
        fprintf(stderr, "GLFW error %d: %s\n", code, desc ? desc : "(null)");
    });

    if (!glfwInit()) {
        ARX_LOG_FATAL("No se pudo inicializar GLFW");
        return false;
    }
    start_ticks_ = linux_now_usec();
    ARX_LOG_INFO("OSLinux inicializado (GLFW {})", glfwGetVersionString());
    return true;
}

void OSLinux::shutdown() {
    glfwTerminate();
    ARX_LOG_INFO("OSLinux apagado");
}

void OSLinux::run() {
    if (!main_loop_) {
        ARX_LOG_FATAL("OSLinux::run sin main_loop");
        return;
    }
    main_loop_->start();

    uint64_t last = linux_now_usec();
    while (!exit_requested_) {
        uint64_t now = linux_now_usec();
        float delta = static_cast<float>(now - last) / 1'000'000.0f;
        last = now;

        if (!main_loop_->process(delta)) break;

        if (auto* w = main_loop_->get_window()) {
            if (w->should_close()) break;
        }
    }
    main_loop_->finish();
}

std::unique_ptr<Window> OSLinux::create_window(const WindowCreateInfo& info) {
    auto w = std::make_unique<WindowGLFW>();
    if (!w->create(info)) return nullptr;
    w->make_current();

    // Cargar GLAD con el contexto activo.
    static bool glad_loaded = false;
    if (!glad_loaded) {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            ARX_LOG_FATAL("No se pudo inicializar GLAD");
            return nullptr;
        }
        glad_loaded = true;
        ARX_LOG_INFO("OpenGL cargado: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }
    return w;
}

std::vector<MonitorInfo> OSLinux::get_monitors() const {
    std::vector<MonitorInfo> out;
    int count = 0;
    GLFWmonitor** mons = glfwGetMonitors(&count);
    for (int i = 0; i < count; ++i) {
        MonitorInfo m;
        m.id = i;
        m.name = glfwGetMonitorName(mons[i]);
        glfwGetMonitorPos(mons[i], &m.x, &m.y);
        const GLFWvidmode* vm = glfwGetVideoMode(mons[i]);
        m.width   = vm->width;
        m.height  = vm->height;
        m.primary = (mons[i] == glfwGetPrimaryMonitor());
        out.push_back(m);
    }
    return out;
}

std::string OSLinux::get_executable_path() const {
    char buf[4096];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (n > 0) { buf[n] = 0; return std::string(buf); }
    return {};
}

std::string OSLinux::get_user_data_dir() const {
    const char* home = getenv("XDG_CONFIG_HOME");
    if (!home || !*home) {
        home = getenv("HOME");
        if (!home || !*home) {
            auto* pw = getpwuid(getuid());
            home = pw ? pw->pw_dir : ".";
        }
        return std::string(home) + "/.config/arx";
    }
    return std::string(home) + "/arx";
}

std::string OSLinux::get_cwd() const {
    return std::filesystem::current_path().string();
}

uint64_t OSLinux::get_ticks_usec() const {
    return linux_now_usec() - start_ticks_;
}

void OSLinux::delay_usec(uint64_t us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

int OSLinux::get_processor_count() const {
    return sysconf(_SC_NPROCESSORS_ONLN);
}

std::string OSLinux::get_locale() const {
    const char* l = getenv("LANG");
    return l ? std::string(l) : "en_US.UTF-8";
}

void OSLinux::set_clipboard(const std::string& text) {
    glfwSetClipboardString(nullptr, text.c_str());
}

std::string OSLinux::get_clipboard() const {
    const char* s = glfwGetClipboardString(nullptr);
    return s ? std::string(s) : "";
}

} // namespace arx
