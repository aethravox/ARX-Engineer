#include "scene/main_loop.hpp"
// ==============================================================================
// src/platforms/linux/os_linux.hpp / cpp
// Implementación de OS para Linux (X11/Wayland via GLFW).
// ==============================================================================
#pragma once

#include "os/os.hpp"

namespace arx {

class OSLinux : public OS {
public:
    bool init() override;
    void shutdown() override;
    void run() override;

    std::unique_ptr<Window> create_window(const WindowCreateInfo& info) override;
    std::vector<MonitorInfo> get_monitors() const override;

    std::string get_executable_path() const override;
    std::string get_user_data_dir() const override;
    std::string get_cwd() const override;

    uint64_t get_ticks_usec() const override;
    void delay_usec(uint64_t us) override;

    int get_processor_count() const override;
    std::string get_locale() const override;

    void set_clipboard(const std::string& text) override;
    std::string get_clipboard() const override;

    void set_main_loop(MainLoop* loop) { main_loop_ = loop; }

private:
    MainLoop* main_loop_ = nullptr;
    uint64_t start_ticks_ = 0;
};

} // namespace arx
