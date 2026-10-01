#pragma once
#include <cstdio>
#include <string>
#include <sstream>
namespace fmt {
template<typename T> std::string to_string(T v) { std::stringstream ss; ss << v; return ss.str(); }
template<typename... Args> std::string format(const char* f, Args&&... a) { char buf[4096]; snprintf(buf, sizeof(buf), f, a...); return buf; }
template<typename... Args> void print(const char* f, Args&&... a) { printf(f, a...); }
}
