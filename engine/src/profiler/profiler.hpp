// ==============================================================================
// src/profiler/profiler.hpp — Profiler interno del motor.
// Mide tiempos de cada sistema (render, physics, audio, scripts) por frame.
// ==============================================================================
#pragma once

#include "core/types.hpp"

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <mutex>
#include <algorithm>

namespace arx {

class Profiler {
public:
    static Profiler& instance();

    // Scope timer — usar el macro ARX_PROFILE_SCOPE.
    class Scope {
    public:
        Scope(const char* name) : name_(name) {
            start_ = std::chrono::high_resolution_clock::now();
        }
        ~Scope() {
            auto end = std::chrono::high_resolution_clock::now();
            double ms = std::chrono::duration<double, std::milli>(end - start_).count();
            Profiler::instance().record(name_, ms);
        }
    private:
        const char* name_;
        std::chrono::high_resolution_clock::time_point start_;
    };

    void record(const std::string& key, double ms);
    void begin_frame();
    void end_frame();

    // Estadísticas del último frame.
    struct Sample {
        std::string name;
        double ms_total = 0.0;
        int    calls    = 0;
        double ms_avg   = 0.0;
        double ms_max   = 0.0;
    };
    std::vector<Sample> get_last_frame() const;
    double get_frame_time() const { return frame_ms_; }
    double get_fps() const { return frame_ms_ > 0 ? 1000.0 / frame_ms_ : 0.0; }

    // Historial para graficar.
    std::vector<double> get_frame_history(int count) const;

    // Enable/disable.
    void set_enabled(bool e) { enabled_ = e; }
    bool is_enabled() const { return enabled_; }

private:
    Profiler() = default;
    bool enabled_ = true;
    double frame_ms_ = 0.0;
    std::chrono::high_resolution_clock::time_point frame_start_;

    std::unordered_map<std::string, Sample> current_;
    std::vector<Sample>                     last_frame_;
    std::vector<double>                     history_;
    mutable std::mutex                      mtx_;
};

#define ARX_PROFILE_SCOPE(name) ::arx::Profiler::Scope _prof_scope(name)
#define ARX_PROFILE_FUNCTION()  ARX_PROFILE_SCOPE(__func__)

} // namespace arx
