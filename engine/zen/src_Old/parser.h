// ============================================================
// Zen Programming Language - Parser
// Parser recursivo descendente con bloques por indentacion
// v1.2: while, for-each, repeat, break, continue, ternario,
//       asignacion aumentada, plataforma
// ============================================================

#pragma once
#include "ast.h"
#include "lexer.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <sstream>

// ============================================================
// Helpers para mensajes de error claros
// ============================================================

inline std::string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::STRING: return "STRING";
        case TokenType::FSTRING: return "FSTRING";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::INDENT: return "INDENT";
        case TokenType::DEDENT: return "DEDENT";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::LBRACKET: return "[";
        case TokenType::RBRACKET: return "]";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::COMMA: return ",";
        case TokenType::DOT: return ".";
        case TokenType::COLON: return ":";
        case TokenType::QUESTION: return "?";
        case TokenType::ARROW: return "->";
        case TokenType::ASSIGN: return "=";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::STAR: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::FUNCION: return "funcion";
        case TokenType::RETORNA: return "retorna";
        case TokenType::MUESTRA: return "muestra";
        case TokenType::SI: return "si";
        case TokenType::PARA: return "para";
        case TokenType::MIENTRAS: return "mientras";
        case TokenType::EXTERN: return "extern";
        case TokenType::VERDAD: return "verdad";
        case TokenType::FALSO: return "falso";
        case TokenType::EOF_TOKEN: return "EOF";
        default: return "TOKEN_" + std::to_string((int)t);
    }
}

inline int levenshtein(const std::string& a, const std::string& b) {
    int n = a.size(), m = b.size();
    if (n == 0) return m;
    if (m == 0) return n;
    std::vector<int> prev(m + 1), curr(m + 1);
    for (int j = 0; j <= m; j++) prev[j] = j;
    for (int i = 1; i <= n; i++) {
        curr[0] = i;
        for (int j = 1; j <= m; j++) {
            int cost = (tolower(a[i-1]) == tolower(b[j-1])) ? 0 : 1;
            curr[j] = std::min({prev[j] + 1, curr[j-1] + 1, prev[j-1] + cost});
        }
        std::swap(prev, curr);
    }
    return prev[m];
}

inline std::vector<std::string> zenKeywords() {
    return {"funcion", "retorna", "muestra", "si", "sino", "para", "mientras",
            "repetir", "extern", "estructura", "platforma", "importar",
            "verdad", "falso", "nada", "romper", "continuar", "y", "o", "no",
            "desde", "hasta", "cada", "en", "veces"};
}

inline std::string suggestKeyword(const std::string& value) {
    if (value.empty() || value.size() > 20) return "";
    auto kws = zenKeywords();
    std::string best;
    int bestDist = 4;
    for (const auto& kw : kws) {
        int d = levenshtein(value, kw);
        if (d < bestDist && d < (int)kw.size() / 2 + 1) {
            bestDist = d;
            best = kw;
        }
    }
    if (best.empty()) return "";
    return " (Sugerencia: quizas quisiste decir '" + best + "'?)";
}

// Palabras clave que terminan un bloque (para parseBlock)
// NOTA: romper/break y continuar/continue NO son terminadores de bloque.
// Son statements normales que aparecen DENTRO del body. Si los ponemos
// como block-end, el parser se confunde y nunca los parsea como statement.
inline bool isBlockEnd(TokenType t) {
    return t == TokenType::DEDENT || t == TokenType::EOF_TOKEN ||
           t == TokenType::SINO || t == TokenType::ELSE ||
           t == TokenType::SINO_SI || t == TokenType::ELIF ||
           t == TokenType::CASO || t == TokenType::CASE ||
           t == TokenType::POR_DEFECTO || t == TokenType::DEFAULT;
}

// Palabras clave en español que tambien pueden usarse como identificadores
// cuando aparecen en posicion de expresion (no entre dos operandos).
// Esto permite escribir:  y = 25   |   muestra o   |   saludo = es
// sin perder:             5 y 3    |   1 o 2       |   x es 5
inline bool isIdentifierLike(TokenType t) {
    return t == TokenType::IDENTIFIER ||
           t == TokenType::Y  ||  // "y"  (AND en español)
           t == TokenType::O  ||  // "o"  (OR en español)
           t == TokenType::ES ||  // "es" (IS en español)
           t == TokenType::EN;    // "en" (IN en español)
}

class Parser {
    std::vector<Token> tokens;
    size_t pos = 0;

    Token peek() { return pos < tokens.size() ? tokens[pos] : tokens.back(); }
    Token advance() {
        if (pos < tokens.size()) return tokens[pos++];
        return tokens.back();
    }
    bool check(TokenType type) { return peek().type == type; }
    bool match(TokenType type) {
        if (check(type)) { advance(); return true; }
        return false;
    }
    // Genera mensaje de error con contexto completo
    [[noreturn]] void throwErrorWithContext(const std::string& msg) {
        Token cur = peek();
        std::ostringstream err;
        err << "\n========== ERROR DE PARSING ==========\n";
        err << "  " << msg << "\n";
        if (!cur.filename.empty()) {
            err << "  Archivo:  " << cur.filename << "\n";
        }
        err << "  Linea:    " << cur.line << "\n";
        err << "  Columna:  " << cur.col << "\n";
        err << "  Token:    " << tokenTypeName(cur.type);
        if (!cur.value.empty() && cur.value != tokenTypeName(cur.type)) {
            err << " (valor: '" << cur.value << "')";
        }
        err << "\n";
        std::string sug = suggestKeyword(cur.value);
        if (!sug.empty()) err << sug << "\n";
        err << "  Contexto (ultimos 5 tokens):\n";
        size_t start = (pos >= 5) ? pos - 5 : 0;
        for (size_t i = start; i <= pos && i < tokens.size(); i++) {
            const Token& t = tokens[i];
            err << "    ";
            if (i == pos) err << ">>> ";
            else err << "    ";
            err << "[" << t.line << ":" << t.col << "] "
                << tokenTypeName(t.type);
            if (!t.value.empty() && t.value != tokenTypeName(t.type)) {
                err << " '" << t.value << "'";
            }
            if (!t.filename.empty()) {
                err << "  (" << t.filename << ")";
            }
            err << "\n";
        }
        err << "======================================\n";
        throw std::runtime_error(err.str());
    }

    Token expect(TokenType type, const std::string& msg) {
        if (check(type)) return advance();
        throwErrorWithContext(msg);
    }
    Token expectIdentifier(const std::string& msg) {
        if (isIdentifierLike(peek().type)) return advance();
        throwErrorWithContext(msg);
    }
    void skipNewlines() { while (check(TokenType::NEWLINE)) advance(); }

    // Expresiones - precedencia de menor a mayor
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTernary();
    std::unique_ptr<ASTNode> parseOr();
    std::unique_ptr<ASTNode> parseAnd();
    std::unique_ptr<ASTNode> parseNot();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseAddSub();
    std::unique_ptr<ASTNode> parseMulDiv();
    std::unique_ptr<ASTNode> parsePower();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parseCallOrPrimary();
    std::unique_ptr<ASTNode> parsePrimary();
    // F-string: descompone f"Hola {x}" en partes (string + expresión)
    std::unique_ptr<ASTNode> parseFString(const std::string& content, int line);

    // Sentencias
    std::vector<std::unique_ptr<ASTNode>> parseBlock();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseForRange();
    std::unique_ptr<ASTNode> parseForEach();
    std::unique_ptr<ASTNode> parseWhile();
    std::unique_ptr<ASTNode> parseRepeat();
    std::unique_ptr<ASTNode> parseFuncDecl();
    std::unique_ptr<ASTNode> parsePlatformBlock();
    std::unique_ptr<ASTNode> parseMatch();
    std::unique_ptr<ASTNode> parseExternDecl();
    // Parsea un tipo de C (int, double, str, ptr, etc.) para FFI
    // Retorna {CType, isPointer}. Si no es un tipo valido, retorna {Void, false}.
    CType parseCType();
    bool checkCType();

public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens) {}
    std::vector<std::unique_ptr<ASTNode>> parse();
    // Wrapper público para parsear una expresión (usado por parseFString)
    std::unique_ptr<ASTNode> parseExpressionPublic() { return parseExpression(); }
};
