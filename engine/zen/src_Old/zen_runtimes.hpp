// ==============================================================================
// zen/src/zen_runtimes.hpp — Runtimes embebidos (AUTO-GENERADO)
// ==============================================================================
#pragma once
#include <string>
#include <vector>

namespace zen {

struct EmbeddedRuntime {
    const char* platform;
    const char* filename;
    const unsigned char* data;
    unsigned long size;
};

const std::vector<EmbeddedRuntime>& get_embedded_runtimes();

std::string extract_runtimes();

std::string get_runtime_path(const std::string& platform, const std::string& filename);

} // namespace zen
