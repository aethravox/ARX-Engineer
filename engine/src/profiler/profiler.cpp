// ==============================================================================
// src/profiler/profiler.cpp
// ==============================================================================
#include "profiler.hpp"

namespace arx {

Profiler& Profiler::instance() {
    static Profiler p;
    return p;
}

void Profiler::record(const std::string& key, double ms) {
    if (!enabled_) return;
    std::lock_guard<std::mutex> lock(mtx_);
    auto& s = current_[key];
    s.name = key;
    s.ms_total += ms;
    s.calls += 1;
    s.ms_avg = s.ms_total / s.calls;
    if (ms > s.ms_max) s.ms_max = ms;
}

void Profiler::begin_frame() {
    if (!enabled_) return;
    frame_start_ = std::chrono::high_resolution_clock::now();
    current_.clear();
}

void Profiler::end_frame() {
    if (!enabled_) return;
    auto end = std::chrono::high_resolution_clock::now();
    frame_ms_ = std::chrono::duration<double, std::milli>(end - frame_start_).count();

    std::lock_guard<std::mutex> lock(mtx_);
    last_frame_.clear();
    for (const auto& [_, s] : current_) last_frame_.push_back(s);
    std::sort(last_frame_.begin(), last_frame_.end(),
              [](const Sample& a, const Sample& b) { return a.ms_total > b.ms_total; });

    history_.push_back(frame_ms_);
    if (history_.size() > 300) history_.erase(history_.begin());
}

std::vector<Profiler::Sample> Profiler::get_last_frame() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return last_frame_;
}

std::vector<double> Profiler::get_frame_history(int count) const {
    std::lock_guard<std::mutex> lock(mtx_);
    int start = std::max(0, (int)history_.size() - count);
    return std::vector<double>(history_.begin() + start, history_.end());
}

} // namespace arx
