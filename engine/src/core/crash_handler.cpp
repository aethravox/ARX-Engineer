// ==============================================================================
// src/core/crash_handler.cpp — Handler de crashes.
// ==============================================================================
#include "crash_handler.hpp"
#include "core/logging.hpp"
#include <csignal>
#include <cstdlib>
#include <cstdio>
#include <execinfo.h>
#include <unistd.h>

namespace arx {

std::string CrashHandler::report_path_ = "arx_crash.log";

void CrashHandler::signal_handler(int sig) {
    const char* sig_name = "Unknown";
    if (sig == SIGSEGV) sig_name = "Segmentation Fault";
    else if (sig == SIGABRT) sig_name = "Abort";
    else if (sig == SIGFPE) sig_name = "Floating Point Exception";
    else if (sig == SIGILL) sig_name = "Illegal Instruction";

    fprintf(stderr, "\n=== ARX ENGINE CRASH ===\n");
    fprintf(stderr, "Signal: %s (%d)\n", sig_name, sig);

    // Backtrace
    void* frames[64];
    int n = backtrace(frames, 64);
    fprintf(stderr, "\nBacktrace (%d frames):\n", n);
    backtrace_symbols_fd(frames, n, STDERR_FILENO);

    // Escribir crash log
    FILE* f = fopen(report_path_.c_str(), "w");
    if (f) {
        fprintf(f, "ARX Engine Crash Report\n");
        fprintf(f, "Signal: %s (%d)\n\n", sig_name, sig);
        fprintf(f, "Backtrace:\n");
        backtrace_symbols_fd(frames, n, fileno(f));
        fclose(f);
        fprintf(stderr, "\nCrash log written to: %s\n", report_path_.c_str());
    }

    fprintf(stderr, "========================\n");
    _exit(1);
}

void CrashHandler::install() {
    signal(SIGSEGV, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGFPE, signal_handler);
    signal(SIGILL, signal_handler);
    ARX_LOG_INFO("CrashHandler instalado");
}

void CrashHandler::set_crash_report_path(const std::string& path) {
    report_path_ = path;
}

} // namespace arx
