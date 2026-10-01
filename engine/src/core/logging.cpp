// ==============================================================================
// src/core/logging.cpp
// ==============================================================================
#include "core/logging.hpp"

#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>

namespace arx {

static const char* level_names[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};
static const char* level_colors[] = {
    "\033[37m", "\033[36m", "\033[32m", "\033[33m", "\033[31m", "\033[41;37m"
};
static const char* color_reset = "\033[0m";

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::set_log_file_path(const std::string& p) {
    log_path_ = p;
    log_to_file_ = !p.empty();
}

void Logger::log(LogLevel lvl, std::string_view msg, const std::source_location& loc) {
    if (lvl < min_level_) return;

    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    // Timestamp
    std::time_t now = std::time(nullptr);
    std::tm tm_buf{};
#ifdef _WIN32
    localtime_s(&tm_buf, &now);
#else
    localtime_r(&now, &tm_buf);
#endif
    char ts[32];
    std::strftime(ts, sizeof(ts), "%H:%M:%S", &tm_buf);

    const char* lvl_name = level_names[static_cast<int>(lvl)];

    // Salida stdout/stderr con colores (en terminal)
    bool use_color = true;
#ifdef _WIN32
    use_color = false; // Simplificación: en Win32 desactivamos color ANSI
#endif

    FILE* out = (lvl >= LogLevel::Warn) ? stderr : stdout;
    if (use_color) {
        std::fprintf(out, "%s[%s] %s%s %s:%d\n",
                     level_colors[static_cast<int>(lvl)], lvl_name,
                     msg.data(), color_reset,
                     loc.file_name(), static_cast<int>(loc.line()));
    } else {
        std::fprintf(out, "[%s] %s %s:%d\n",
                     lvl_name, msg.data(),
                     loc.file_name(), static_cast<int>(loc.line()));
    }

    // Salida archivo
    if (log_to_file_) {
        std::ofstream f(log_path_, std::ios::app);
        if (f) {
            f << '[' << ts << "] [" << lvl_name << "] " << msg
              << " (" << loc.file_name() << ':' << loc.line() << ")\n";
        }
    }
}

} // namespace arx
