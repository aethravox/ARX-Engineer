// ==============================================================================
// src/core/crash_handler.hpp — Handler de crashes (segfault, etc.)
//
// Captura señales del SO y muestra un mensaje legible antes de morir.
// ==============================================================================
#pragma once
#include <string>

namespace arx {

class CrashHandler {
public:
    static void install();
    static void set_crash_report_path(const std::string& path);

private:
    static std::string report_path_;
    static void signal_handler(int sig);
};

} // namespace arx
