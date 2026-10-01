// ==============================================================================
// src/core/logging.hpp — logging + macros de error.
// ==============================================================================
#pragma once

#include <fmt/format.h>  // Si no, usar std::format (C++20) o un fallback.

#include <string>
#include <string_view>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <source_location>

namespace arx {

enum class LogLevel : uint8_t {
    Trace, Debug, Info, Warn, Error, Fatal
};

class Logger {
public:
    static Logger& instance();

    void set_min_level(LogLevel lvl) { min_level_ = lvl; }
    void set_log_to_file(bool v)     { log_to_file_ = v; }
    void set_log_file_path(const std::string& p);

    void log(LogLevel lvl, std::string_view msg,
             const std::source_location& loc = std::source_location::current());

    template<typename... Args>
    void logf(LogLevel lvl, fmt::format_string<Args...> fmt, Args&&... args) {
        if (lvl < min_level_) return;
        log(lvl, fmt::format(fmt, std::forward<Args>(args)...));
    }

private:
    Logger() = default;
    LogLevel min_level_ = LogLevel::Info;
    bool     log_to_file_ = false;
    std::string log_path_;
};

// Macros públicas
#define ARX_LOG_TRACE(...) ::arx::Logger::instance().logf(::arx::LogLevel::Trace, __VA_ARGS__)
#define ARX_LOG_DEBUG(...) ::arx::Logger::instance().logf(::arx::LogLevel::Debug, __VA_ARGS__)
#define ARX_LOG_INFO(...)  ::arx::Logger::instance().logf(::arx::LogLevel::Info,  __VA_ARGS__)
#define ARX_LOG_WARN(...)  ::arx::Logger::instance().logf(::arx::LogLevel::Warn,  __VA_ARGS__)
#define ARX_LOG_ERROR(...) ::arx::Logger::instance().logf(::arx::LogLevel::Error, __VA_ARGS__)
#define ARX_LOG_FATAL(...) do { \
    ::arx::Logger::instance().logf(::arx::LogLevel::Fatal, __VA_ARGS__); \
    std::abort(); \
} while(0)

// Macros de checkeo estilo Godot.
#define ARX_ASSERT(cond, ...) do { \
    if (!(cond)) ARX_LOG_FATAL("Assertion failed: {} ({}:{})", \
        fmt::format(__VA_ARGS__), __FILE__, __LINE__); \
} while(0)

#define ARX_ASSERT_MSG(cond, msg) ARX_ASSERT(cond, "{}", msg)

// ARX_DEV_ASSERT solo activa en ARX_DEBUG. No se puede usar #ifdef dentro de
// #define, así que lo hacemos con dos ramas:
#ifdef ARX_DEBUG
    #define ARX_DEV_ASSERT(cond, ...) ARX_ASSERT(cond, __VA_ARGS__)
#else
    #define ARX_DEV_ASSERT(cond, ...) do { (void)sizeof(cond); } while(0)
#endif

#define ARX_FAIL(...) ARX_LOG_FATAL("FAIL: {}", fmt::format(__VA_ARGS__))

} // namespace arx
