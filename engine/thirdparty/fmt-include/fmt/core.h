#pragma once
#include <cstdio>
#include <string>
namespace fmt {
template<typename... Args> struct basic_format_string { const char* str; basic_format_string(const char* s) : str(s) {} };
using format_string = basic_format_string<>;
}
