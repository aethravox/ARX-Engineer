// ==============================================================================
// src/editor/debugger/debugger.hpp — Debugger para ARXScript.
//
// Soporta:
//   - Breakpoints por archivo + línea.
//   - Step into / over / out.
//   - Inspección de variables locales y globales.
//   - Call stack.
//   - Watch expressions.
//   - Conditional breakpoints.
// ==============================================================================
#pragma once

#include "core/variant.hpp"

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

// Forward declarations: arxscript fue reemplazado por Zen y Luau, pero el
// debugger mantiene una interfaz genérica para attacharse a cualquier VM.
namespace arxscript {
    class VM;
}

namespace arx {

struct Breakpoint {
    std::string file;
    int         line;
    std::string condition;     // Vacío = breakpoint incondicional.
    bool        enabled = true;
    int         hit_count = 0;
    int         ignore_count = 0;   // Saltar N hits antes de parar.
};

struct StackFrameInfo {
    std::string function_name;
    std::string file;
    int         line;
    std::unordered_map<std::string, Variant> locals;
};

struct WatchExpression {
    std::string expression;
    Variant     current_value;
    bool        error = false;
};

class Debugger {
public:
    static Debugger& instance();

    // Breakpoints.
    void add_breakpoint(const std::string& file, int line,
                          const std::string& condition = "");
    void remove_breakpoint(const std::string& file, int line);
    void clear_breakpoints();
    void set_breakpoint_enabled(const std::string& file, int line, bool enabled);
    void set_breakpoint_condition(const std::string& file, int line,
                                    const std::string& condition);
    void set_breakpoint_ignore_count(const std::string& file, int line, int count);
    std::vector<Breakpoint> get_breakpoints() const;

    // Control de ejecución.
    void continue_execution();
    void step_over();
    void step_into();
    void step_out();
    void pause();
    void stop();

    bool is_paused() const { return paused_; }
    bool is_attached() const { return attached_; }

    // Inspección.
    std::vector<StackFrameInfo> get_call_stack() const;
    std::vector<std::string> get_local_variables(int frame_index = 0) const;
    Variant get_variable_value(const std::string& name, int frame_index = 0) const;

    // Watches.
    void add_watch(const std::string& expr);
    void remove_watch(const std::string& expr);
    std::vector<WatchExpression> get_watches() const;
    void update_watches();

    // Attach/detach a una VM.
    void attach(arxscript::VM* vm);
    void detach();

    // Callbacks.
    void on_breakpoint_hit(std::function<void(const Breakpoint&)> cb) {
        on_breakpoint_ = std::move(cb);
    }
    void on_pause(std::function<void()> cb) { on_pause_ = std::move(cb); }
    void on_continue(std::function<void()> cb) { on_continue_ = std::move(cb); }
    void on_log(std::function<void(const std::string&)> cb) { on_log_ = std::move(cb); }

    // Llamado desde la VM cuando llega a una línea con breakpoint.
    void notify_line(const std::string& file, int line);

private:
    Debugger() = default;
    arxscript::VM* vm_ = nullptr;
    bool attached_ = false;
    bool paused_   = false;

    std::vector<Breakpoint> breakpoints_;
    std::vector<WatchExpression> watches_;

    std::function<void(const Breakpoint&)> on_breakpoint_;
    std::function<void()>                  on_pause_;
    std::function<void()>                  on_continue_;
    std::function<void(const std::string&)> on_log_;

    Breakpoint* find_breakpoint(const std::string& file, int line);
    bool evaluate_condition(const std::string& condition);
};

} // namespace arx
