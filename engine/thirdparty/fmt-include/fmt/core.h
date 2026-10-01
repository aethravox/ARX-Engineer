#pragma once
#include <cstdio>
#include <string>
namespace fmt { template<typename... Args> void print(const char* f, Args&&... a) { printf(f, a...); } }
