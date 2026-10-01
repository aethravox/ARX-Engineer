// ==============================================================================
// src/scripting/zen_bridge.cpp — Implementación del bridge Zen.
// ==============================================================================
#include "scripting/zen_bridge.hpp"
#include "core/logging.hpp"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>

#include <fstream>
#include <sstream>

// Headers de Zen (incluidos vía add_subdirectory(zen) + target_include_directories)
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

namespace arx {

ZenCompiler::ZenCompiler() {
    // Inicializar targets LLVM una sola vez.
    static bool targets_initialized = false;
    if (!targets_initialized) {
        CodeGen::initializeTargets();
        targets_initialized = true;
    }
}

ZenCompileResult ZenCompiler::compile_source(const std::string& source,
                                                const std::string& filename,
                                                bool emit_object,
                                                const std::string& target_platform) {
    ZenCompileResult r;

    try {
        // 1. Lex (los errores de lexer se lanzan como excepción)
        Lexer lex(source);
        auto lex_result = lex.tokenize();

        // 2. Parse (los errores de parser también se lanzan como excepción)
        Parser parser(lex_result.tokens);
        auto ast = parser.parse();

        // 3. Codegen
        CodeGen codegen(target_platform);
        codegen.generate(ast);
        if (!codegen.verify()) {
            r.error_message = "Verificación LLVM IR fallida";
            return r;
        }

        // IR text
        r.ir_text = codegen.getIR();

        // Libs requeridas (FFI)
        r.required_libs = codegen.getRequiredLibs();

        // Emit object file si piden
        if (emit_object) {
            // Generar path temporal
            std::string obj_path = filename + ".o";
            if (obj_path == "<memory>.o") obj_path = "/tmp/zen_compile.o";
            if (codegen.emitObjectFile(obj_path)) {
                r.object_file_path = obj_path;
                r.logs.push_back("Object file emitido: " + obj_path);
            } else {
                r.warnings.push_back("No se pudo emitir el .o");
            }
        }

        r.success = true;
    } catch (const std::exception& e) {
        r.error_message = e.what();
    }

    return r;
}

ZenCompileResult ZenCompiler::compile_file(const fs::path& input_path,
                                             bool emit_object,
                                             const std::string& target_platform) {
    ZenCompileResult r;
    if (!fs::exists(input_path)) {
        r.error_message = "Archivo no encontrado: " + input_path.string();
        return r;
    }
    std::ifstream in(input_path);
    if (!in) {
        r.error_message = "No se pudo abrir: " + input_path.string();
        return r;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    auto source = ss.str();

    r = compile_source(source, input_path.string(), emit_object, target_platform);
    return r;
}

std::vector<std::string> ZenCompiler::lint(const std::string& source,
                                              const std::string& filename) {
    std::vector<std::string> errors;
    try {
        Lexer lex(source);
        auto lex_result = lex.tokenize();
        Parser parser(lex_result.tokens);
        auto ast = parser.parse();  // lanza excepción en error de sintaxis
    } catch (const std::exception& e) {
        errors.push_back(std::string("[parse] ") + e.what());
    }
    return errors;
}

std::string ZenCompiler::version() {
    return "Zen 1.2 (LLVM " + std::string(LLVM_VERSION_STRING) + ")";
}

} // namespace arx
