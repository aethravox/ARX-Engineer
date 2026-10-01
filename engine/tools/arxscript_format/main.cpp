// ==============================================================================
// tools/arxscript_format/main.cpp — Formatter de ARXScript.
// Uso: arxscript_format <archivo.arx> [--output <out.arx>] [--inplace]
// ==============================================================================
#include "arxscript/lexer.hpp"
#include "arxscript/parser.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace arxscript = arx::arxscript;

static std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) return "";
    std::stringstream ss; ss << f.rdbuf();
    return ss.str();
}

// Pretty-printer simple del AST. Convierte tokens → texto con indentación
// consistente (4 espacios).
static std::string format_tokens(const std::vector<arxscript::Token>& tokens) {
    std::ostringstream out;
    int indent = 0;
    bool at_line_start = true;
    bool last_was_newline = false;

    auto push_indent = [&]() {
        for (int i = 0; i < indent; ++i) out << "    ";
    };

    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& t = tokens[i];

        if (t.type == arxscript::TokenType::Indent)   { indent++; continue; }
        if (t.type == arxscript::TokenType::Dedent)   { indent--; continue; }
        if (t.type == arxscript::TokenType::Eof)      break;

        if (t.type == arxscript::TokenType::Newline) {
            if (!last_was_newline) out << "\n";
            at_line_start = true;
            last_was_newline = true;
            continue;
        }

        if (at_line_start) { push_indent(); at_line_start = false; }

        // Insertar espacio entre tokens si hace falta.
        if (!last_was_newline && i > 0) {
            const auto& prev = tokens[i-1];
            bool need_space =
                (prev.type == arxscript::TokenType::Identifier ||
                 prev.type == arxscript::TokenType::IntLit ||
                 prev.type == arxscript::TokenType::FloatLit ||
                 prev.type == arxscript::TokenType::StringLit ||
                 prev.type == arxscript::TokenType::True ||
                 prev.type == arxscript::TokenType::False ||
                 prev.type == arxscript::TokenType::Null) &&
                (t.type == arxscript::TokenType::Identifier ||
                 t.type == arxscript::TokenType::IntLit ||
                 t.type == arxscript::TokenType::FloatLit ||
                 t.type == arxscript::TokenType::StringLit ||
                 t.type == arxscript::TokenType::True ||
                 t.type == arxscript::TokenType::False ||
                 t.type == arxscript::TokenType::Null);
            if (need_space) out << " ";
        }

        out << t.text;
        last_was_newline = false;
    }
    out << "\n";
    return out.str();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Uso: arxscript_format <archivo.arx> [--output out.arx] [--inplace]\n";
        return 1;
    }

    std::string input_path = argv[1];
    std::string output_path;
    bool inplace = false;

    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--output" && i + 1 < argc) output_path = argv[++i];
        else if (a == "--inplace") { inplace = true; output_path = input_path; }
    }
    if (output_path.empty()) output_path = input_path + ".formatted";

    std::string src = read_file(input_path);
    if (src.empty()) {
        std::cerr << "Error: no se pudo leer " << input_path << "\n";
        return 1;
    }

    arxscript::Lexer lex;
    auto tokens = lex.tokenize(src, input_path);

    std::string formatted = format_tokens(tokens);

    std::ofstream f(output_path);
    if (!f) {
        std::cerr << "Error: no se pudo escribir " << output_path << "\n";
        return 1;
    }
    f << formatted;
    std::cout << "Formateado: " << output_path << "\n";
    return 0;
}
