// ==============================================================================
// tools/scene_compiler/main.cpp — Compila .scene a formato binario.
// ==============================================================================
#include "scene/resources/packed_scene.hpp"
#include "core/logging.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Uso: scene_compiler <input.scene> <output.scene.bin>\n";
        return 1;
    }
    arx::PackedScene ps;
    if (!ps.load_from_file(argv[1])) {
        std::cerr << "Error: no se pudo cargar " << argv[1] << "\n";
        return 1;
    }
    // Stub: guardar binario (en una impl completa se serializaría).
    std::ofstream out(argv[2], std::ios::binary);
    if (!out) { std::cerr << "Error escribiendo " << argv[2] << "\n"; return 1; }
    out << "# ARX Scene Binary v1\n";
    std::cout << "OK: " << argv[1] << " -> " << argv[2] << "\n";
    return 0;
}
