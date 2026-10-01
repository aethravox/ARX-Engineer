// ==============================================================================
#include "core/variant.hpp"
// src/editor/debugger/debugger.cpp
// ==============================================================================
#include "debugger.hpp"
#include "core/logging.hpp"

#include <algorithm>

namespace arx {

Debugger& Debugger::instance() {
    static Debugger d;
    return d;
}

void Debugger::add_breakpoint(const std::string& file, int line,
                                const std::string& condition) {
    if (find_breakpoint(file, line)) return;
    Breakpoint bp;
    bp.file = file;
    bp.line = line;
    bp.condition = condition;
    breakpoints_.push_back(std::move(bp));
}

void Debugger::remove_breakpoint(const std::string& file, int line) {
    breakpoints_.erase(
        std::remove_if(breakpoints_.begin(), breakpoints_.end(),
            [&](const Breakpoint& b) { return b.file == file && b.line == line; }),
        breakpoints_.end());
}

void Debugger::clear_breakpoints() { breakpoints_.clear(); }

void Debugger::set_breakpoint_enabled(const std::string& file, int line, bool e) {
    if (auto* bp = find_breakpoint(file, line)) bp->enabled = e;
}

void Debugger::set_breakpoint_condition(const std::string& file, int line,
                                          const std::string& cond) {
    if (auto* bp = find_breakpoint(file, line)) bp->condition = cond;
}

void Debugger::set_breakpoint_ignore_count(const std::string& file, int line, int n) {
    if (auto* bp = find_breakpoint(file, line)) bp->ignore_count = n;
}

std::vector<Breakpoint> Debugger::get_breakpoints() const { return breakpoints_; }

void Debugger::continue_execution() {
    paused_ = false;
    if (on_continue_) on_continue_();
}

void Debugger::step_over()    { paused_ = false; /* one step */ paused_ = true; }
void Debugger::step_into()    { paused_ = false; /* step into */ paused_ = true; }
void Debugger::step_out()     { paused_ = false; /* step out */ paused_ = true; }
void Debugger::pause() {
    paused_ = true;
    if (on_pause_) on_pause_();
}
void Debugger::stop() {
    paused_ = false;
    attached_ = false;
    vm_ = nullptr;
}

std::vector<StackFrameInfo> Debugger::get_call_stack() const {
    // Stub: en una impl completa se accede a la VM y se extraen los frames.
    return {};
}

std::vector<std::string> Debugger::get_local_variables(int frame_index) const {
    (void)frame_index;
    return {};
}

Variant Debugger::get_variable_value(const std::string& name, int frame_index) const {
    (void)name; (void)frame_index;
    return {};
}

void Debugger::add_watch(const std::string& expr) {
    for (auto& w : watches_) {
        if (w.expression == expr) return;
    }
    watches_.push_back({expr, {}, false});
}

void Debugger::remove_watch(const std::string& expr) {
    watches_.erase(
        std::remove_if(watches_.begin(), watches_.end(),
            [&](const WatchExpression& w) { return w.expression == expr; }),
        watches_.end());
}

std::vector<WatchExpression> Debugger::get_watches() const { return watches_; }

void Debugger::update_watches() {
    for (auto& w : watches_) {
        // Stub: evaluar expr y actualizar current_value.
        w.error = false;
    }
}

void Debugger::attach(arxscript::VM* vm) {
    vm_ = vm;
    attached_ = vm != nullptr;
    ARX_LOG_INFO("Debugger attached to VM");
}

void Debugger::detach() {
    vm_ = nullptr;
    attached_ = false;
}

void Debugger::notify_line(const std::string& file, int line) {
    if (!attached_) return;
    Breakpoint* bp = find_breakpoint(file, line);
    if (!bp || !bp->enabled) return;

    if (bp->ignore_count > 0) {
        --bp->ignore_count;
        return;
    }
    if (!bp->condition.empty() && !evaluate_condition(bp->condition)) {
        return;
    }

    bp->hit_count++;
    paused_ = true;
    if (on_breakpoint_) on_breakpoint_(*bp);
    // Bloquear hasta que el usuario continue/step.
    while (paused_ && attached_) {
        // En una impl completa se usaría un condition_variable.
    }
}

Breakpoint* Debugger::find_breakpoint(const std::string& file, int line) {
    for (auto& bp : breakpoints_) {
        if (bp.file == file && bp.line == line) return &bp;
    }
    return nullptr;
}

bool Debugger::evaluate_condition(const std::string& condition) {
    // Stub: evaluar expr con la VM.
    (void)condition;
    return true;
}

} // namespace arx
