#pragma once
#include <cstdio>
#include <string>
#include <sstream>
#include "fmt/core.h"
namespace fmt {
template<typename... Args> std::string format(const char* fmt_str, Args&&... args) {
    char buf[8192]; snprintf(buf, sizeof(buf), "%s", fmt_str); return std::string(buf);
}
template<typename... Args> void print(const char* fmt_str, Args&&... args) {
    printf("%s", fmt_str);
}
}
