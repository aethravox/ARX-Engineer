// ==============================================================================
// zen/src/zen_api.cpp — API pública de Zen (implementación).
// ==============================================================================
#include "zen_api.hpp"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "zen_linker.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace zen {

static bool s_initialized = false;

void initialize() {
    if (!s_initialized) {
        CodeGen::initializeTargets();
        s_initialized = true;
    }
}

// Helper: leer archivo
static std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return "";
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

bool compile_to_obj(const std::string& zen_file,
                    const std::string& obj_path,
                    const std::string& platform,
                    std::string& error_msg) {
    initialize();

    // 1. Leer archivo .zen
    std::string source = read_file(zen_file);
    if (source.empty()) {
        error_msg = "No se pudo leer: " + zen_file;
        return false;
    }

    // 2. Lex
    Lexer lexer(source);
    auto lexResult = lexer.tokenize();
    if (lexResult.tokens.empty()) {
        error_msg = "Error lex: no se generaron tokens";
        return false;
    }

    // 3. Parse
    Parser parser(lexResult.tokens);
    auto ast = parser.parse();
    if (ast.empty()) {
        error_msg = "Error parse: AST vacío";
        return false;
    }

    // 4. Codegen (LLVM IR → .o)
    CodeGen codegen(platform);
    codegen.generate(ast);

    if (!codegen.verify()) {
        error_msg = "Error verificación LLVM IR";
        return false;
    }

    if (!codegen.emitObjectFile(obj_path)) {
        error_msg = "Error generando .o";
        return false;
    }

    return true;
}

bool compile_and_link(const std::string& zen_file,
                      const std::string& output,
                      const std::string& platform,
                      std::string& error_msg) {
    // 1. Compilar a .o
    std::string obj_path = output + ".o";
    if (!compile_to_obj(zen_file, obj_path, platform, error_msg)) {
        return false;
    }

    // 2. Linkar
    auto req_libs = std::vector<std::string>();
    if (!link_object(obj_path, output, platform, req_libs, error_msg)) {
        return false;
    }

    // 3. Limpiar .o temporal
    std::filesystem::remove(obj_path);

    return true;
}

} // namespace zen
