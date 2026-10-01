// ==============================================================================
// src/platforms/windows/os_windows.cpp
// ==============================================================================
#include "os_windows.hpp"
#include "../linux/window_glfw.hpp"   // Reutilizamos la impl GLFW.
#include "scene/main_loop.hpp"
#include "core/logging.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <windows.h>
#include <shlobj.h>
#include <chrono>
#include <thread>
#include <filesystem>

namespace arx {

static uint64_t win_now_usec() {
    auto now = std::chrono::high_resolution_clock::now();
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()).count());
}

bool OSWindows::init() {
    if (!glfwInit()) {
        ARX_LOG_FATAL("No se pudo inicializar GLFW");
        return false;
    }
    start_ticks_ = win_now_usec();
    ARX_LOG_INFO("OSWindows inicializado (GLFW {})", glfwGetVersionString());
    return true;
}

void OSWindows::shutdown() {
    glfwTerminate();
    ARX_LOG_INFO("OSWindows apagado");
}

void OSWindows::run() {
    if (!main_loop_) {
        ARX_LOG_FATAL("OSWindows::run sin main_loop");
        return;
    }
    main_loop_->start();

    uint64_t last = win_now_usec();
    while (!exit_requested_) {
        uint64_t now = win_now_usec();
        float delta = static_cast<float>(now - last) / 1'000'000.0f;
        last = now;

        if (!main_loop_->process(delta)) break;

        if (auto* w = main_loop_->get_window()) {
            if (w->should_close()) break;
        }
    }
    main_loop_->finish();
}

std::unique_ptr<Window> OSWindows::create_window(const WindowCreateInfo& info) {
    auto w = std::make_unique<WindowGLFW>();
    if (!w->create(info)) return nullptr;
    w->make_current();

    static bool glad_loaded = false;
    if (!glad_loaded) {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            ARX_LOG_FATAL("No se pudo inicializar GLAD");
            return nullptr;
        }
        glad_loaded = true;
        ARX_LOG_INFO("OpenGL cargado: {}",
                     reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }
    return w;
}

std::vector<MonitorInfo> OSWindows::get_monitors() const {
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

std::string OSWindows::get_executable_path() const {
    char buf[MAX_PATH];
    DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    return n > 0 ? std::string(buf, n) : "";
}

std::string OSWindows::get_user_data_dir() const {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        return std::string(path) + "\\ARX";
    }
    return ".";
}

std::string OSWindows::get_cwd() const {
    return std::filesystem::current_path().string();
}

uint64_t OSWindows::get_ticks_usec() const {
    return win_now_usec() - start_ticks_;
}

void OSWindows::delay_usec(uint64_t us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

int OSWindows::get_processor_count() const {
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return static_cast<int>(si.dwNumberOfProcessors);
}

std::string OSWindows::get_locale() const {
    char buf[LOCALE_NAME_MAX_LENGTH];
    int n = GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SNAME, buf, sizeof(buf));
    return n > 0 ? std::string(buf) : "en-US";
}

void OSWindows::set_clipboard(const std::string& text) {
    glfwSetClipboardString(nullptr, text.c_str());
}

std::string OSWindows::get_clipboard() const {
    const char* s = glfwGetClipboardString(nullptr);
    return s ? std::string(s) : "";
}

} // namespace arx
