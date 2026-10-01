// ==============================================================================
// src/editor/gui/output_log.hpp / .cpp — Consola de output del editor.
// ==============================================================================
#pragma once

#include <string>
#include <vector>
#include <mutex>

namespace arx {

class OutputLog {
public:
    void render();

    void log_info(const std::string& s)    { push("[INFO]  " + s, 0xFFFFFFFF); }
    void log_warn(const std::string& s)    { push("[WARN]  " + s, 0xFFFFFF30); }
    void log_error(const std::string& s)   { push("[ERROR] " + s, 0xFFFF6060); }
    void log_success(const std::string& s) { push("[OK]    " + s, 0xFF60FF60); }

private:
    void push(const std::string& line, uint32_t color) {
        std::lock_guard<std::mutex> lock(mtx_);
        lines_.push_back({line, color});
        if (lines_.size() > 4096) lines_.erase(lines_.begin());
    }

    struct Entry { std::string text; uint32_t color; };
    std::vector<Entry> lines_;
    std::mutex         mtx_;
    bool               auto_scroll_ = true;
};

} // namespace arx
