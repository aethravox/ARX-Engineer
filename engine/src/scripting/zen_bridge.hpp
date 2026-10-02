// ==============================================================================
// src/scripting/zen_bridge.hpp — Bridge entre el engine y el compilador Zen.
//
// Permite al editor y al exporter de ARX invocar el compilador Zen sin pasar
// por la línea de comandos. Expone una API C++ simple sobre `libzen.a`.
//
// Casos de uso:
//   1. Editor (preview en tiempo real):
//        - Compila el .zen actual → genera .ll
//        - Para preview habría que hacer una VM (pendiente), pero al menos
//          podemos validar sintácticamente y mostrar el IR.
//
//   2. Exporter AOT (al empaquetar .aex):
//        - Compila cada .zen del proyecto → .o nativo
//        - Linkea los .o en un .so para la plataforma destino
//        - Empaqueta el .so dentro del .aex
//
//   3. CLI tools (arxscript_lint, arxscript_format):
//        - Lint: parsea y reporta errores sin generar código
//        - Format: parsea → pretty-print del AST
// ==============================================================================
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>

namespace arx {

namespace fs = std::filesystem;

// Resultado de compilar un archivo .zen
struct ZenCompileResult {
    bool        success        = false;
    std::string error_message;                // primer error encontrado
    std::vector<std::string> warnings;
    std::vector<std::string> logs;

    std::string ir_text;                      // LLVM IR (vacío si no se generó)
    std::string object_file_path;             // .o si se emitió con --emit-obj
    std::vector<std::string> required_libs;   // FFI libs (ej: "raylib")
};

// API principal del compilador Zen.
class ZenCompiler {
public:
    ZenCompiler();

    // Compila un archivo .zen a LLVM IR + (opcional) código objeto nativo.
    // - input_path: archivo .zen
    // - emit_object: si true, genera un .o junto al input (mismo nombre)
    // - target_platform: "linux", "windows", "android", "web" — para #plataforma
    // Devuelve ZenCompileResult con todo lo que se generó.
    ZenCompileResult compile_file(const fs::path& input_path,
                                    bool emit_object = false,
                                    const std::string& target_platform = "");

    // Compila un string (código Zen directo) sin tocar el FS.
    // Útil para preview en el editor o para tests.
    ZenCompileResult compile_source(const std::string& source,
                                      const std::string& filename = "<memory>",
                                      bool emit_object = false,
                                      const std::string& target_platform = "");

    // Solo valida sintaxis (lex + parse, sin codegen).
    // Devuelve lista de errores, vacía si OK.
    std::vector<std::string> lint(const std::string& source,
                                    const std::string& filename = "<memory>");

    // Devuelve la versión de Zen/LLVM disponible.
    static std::string version();
};

} // namespace arx
