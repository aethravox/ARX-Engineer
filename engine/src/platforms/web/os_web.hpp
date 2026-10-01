// ==============================================================================
// src/platforms/web/os_web.hpp / cpp — Implementación para Emscripten.
// Usa emscripten_* APIs para el bucle y eventos. Sin hilos.
// ==============================================================================
#pragma once

#include "os/os.hpp"

namespace arx {

class OSWeb : public OS {
public:
    bool init() override;
    void shutdown() override;
    void run() override;   // emscripten_set_main_loop

    std::unique_ptr<Window> create_window(const WindowCreateInfo& info) override;
    std::vector<MonitorInfo> get_monitors() const override {
        return MonitorInfo{0, "Web", 0, 0, 1280, 720, true};
    }

    std::string get_executable_path() const override { return "/arx.html"; }
    std::string get_user_data_dir() const override;
    std::string get_cwd() const override;

    uint64_t get_ticks_usec() const override;
    void delay_usec(uint64_t) override {}  // No-op en web.

    int get_processor_count() const override { return 1; }
    std::string get_locale() const override;
    void set_clipboard(const std::string&) override {}
    std::string get_clipboard() const override { return ""; }

    void set_main_loop(MainLoop* loop) { main_loop_ = loop; }

private:
    MainLoop* main_loop_ = nullptr;
};

} // namespace arx
