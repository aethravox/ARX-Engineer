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
    Token expect(TokenType type, const std::string& msg) {
        if (check(type)) return advance();
        throw std::runtime_error(
            msg + " en la linea " + std::to_string(peek().line));
    }
    // Como expect() pero acepta cualquier token tipo-identificador
    // (IDENTIFIER + las keywords ambiguas Y/O/ES/EN).
    Token expectIdentifier(const std::string& msg) {
        if (isIdentifierLike(peek().type)) return advance();
        throw std::runtime_error(
            msg + " en la linea " + std::to_string(peek().line));
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
