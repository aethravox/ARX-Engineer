// ==============================================================================
// tools/arxscript_lint/main.cpp — Linter de ARXScript.
// Uso: arxscript_lint <archivo.arx> [<archivo2.arx> ...]
// ==============================================================================
#include "arxscript/lexer.hpp"
#include "arxscript/parser.hpp"
#include "core/logging.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace arxscript = arx::arxscript;

struct LintReport {
    std::string file;
    int errors   = 0;
    int warnings = 0;
    std::vector<std::string> messages;
};

static std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) return "";
    std::stringstream ss; ss << f.rdbuf();
    return ss.str();
}

static LintReport lint_file(const std::string& path) {
    LintReport r;
    r.file = path;
    std::string src = read_file(path);
    if (src.empty()) {
        r.errors++;
        r.messages.push_back("[ERROR] No se pudo leer el archivo");
        return r;
    }

    arxscript::Lexer lex;
    auto tokens = lex.tokenize(src, path);

    bool has_class = false;
    for (const auto& t : tokens) {
        if (t.type == arxscript::TokenType::Class) { has_class = true; break; }
    }
    if (!has_class) {
        r.warnings++;
        r.messages.push_back("[WARN] No se encontró declaración 'class'.");
    }

    try {
        arxscript::Parser parser(tokens);
        auto mod = parser.parse_module(path);
        r.messages.push_back("[OK] Parsing exitoso (" +
                              std::to_string(mod->top_level.size()) + " top-level nodes)");
    } catch (const arxscript::ParseError& e) {
        r.errors++;
        r.messages.push_back(std::string("[ERROR] ") + e.what());
    }

    return r;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: arxscript_lint <archivo.arx> [<archivo2.arx> ...]\n";
        return 1;
    }

    int total_errors = 0, total_warnings = 0;
    for (int i = 1; i < argc; ++i) {
        auto report = lint_file(argv[i]);
        std::cout << "=== " << report.file << " ===\n";
        for (const auto& m : report.messages) std::cout << "  " << m << "\n";
        std::cout << "  " << report.errors << " errors, "
                  << report.warnings << " warnings\n\n";
        total_errors   += report.errors;
        total_warnings += report.warnings;
    }

    std::cout << "Total: " << total_errors << " errors, "
              << total_warnings << " warnings\n";
    return total_errors > 0 ? 1 : 0;
}
