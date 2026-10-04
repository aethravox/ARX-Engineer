// ============================================================
// Zen Programming Language - Lexer (Tokenizador)
// Bilingue ES/EN | Indentation-based | #lang directive
// ============================================================

#pragma once
#include <string>
#include <vector>
#include <map>

// Todos los tipos de token
enum class TokenType {
    // Directivas
    LANG_HASH,  // #lang

    // Literales
    NUMBER,
    STRING,
    FSTRING,  // f"Hola {nombre}" — string con interpolación

    // Identificador
    IDENTIFIER,

    // Condicionales
    SI, IF,
    SINO, ELSE,
    SINO_SI, ELIF,

    // Bucles
    PARA, FOR,
    DESDE, FROM,
    HASTA, TO,
    MIENTRAS, WHILE,
    EN, IN,
    CADA, EACH,
    REPETIR, REPEAT,
    VECES, TIMES,
    ROMPER, BREAK,
    CONTINUAR, CONTINUE,

    // Funciones
    FUNCION, FUNCTION,
    RETORNA, RETURN,

    // Imprimir
    MUESTRA, SHOW, PRINT,

    // Logica
    Y, AND, O, OR, NO, NOT, ES, IS,

    // Match
    COINCIDIR, MATCH,
    CASO, CASE,
    POR_DEFECTO, DEFAULT,

    // Manejo de errores
    INTENTAR, TRY,
    ATRAPAR, CATCH,
    FINALMENTE, FINALLY,
    LANZAR, THROW,

    // Structs (sintaxis nueva)
    ESTRUCTURA, STRUCT,

    // Memoria
    MEMORIA, MEMORY,
    AUTOMATICA, AUTO,
    MANUAL,
    MUT, REF,

    // Plataforma/Modulos
    PLATAFORMA, PLATFORM,
    IMPORTAR, IMPORT,
    INCLUIR, INCLUDE,
    FIN_PLATAFORMA,  // cierre de bloque plataforma

    // FFI
    EXTERN,

    // Constantes
    YES, VERDAD, FALSO, NADA, NOTHING,

    // Operadores
    PLUS, MINUS, STAR, SLASH, PERCENT, POWER,
    EQEQ, NEQ, LT, GT, LTE, GTE,
    ASSIGN,
    PLUS_ASSIGN, MINUS_ASSIGN, STAR_ASSIGN, SLASH_ASSIGN, PERCENT_ASSIGN, POWER_ASSIGN,
    ARROW,

    // Delimitadores
    LPAREN, RPAREN,
    LBRACKET, RBRACKET,
    LBRACE, RBRACE,
    COMMA, DOT, COLON, QUESTION,

    // Estructura (indentacion)
    NEWLINE,
    INDENT,
    DEDENT,
    EOF_TOKEN,
};

// Un token con su tipo, valor y posicion
struct Token {
    TokenType type;
    std::string value;
    int line;
    int col;
    std::string filename;  // archivo donde aparece el token (para errores claros)
};

// Resultado del lexer: tokens + idioma detectado
struct LexResult {
    std::vector<Token> tokens;
    std::string lang;  // "es", "en", o "" (bilingue)
};

// Tokenizador de Zen
class Lexer {
    std::string source;
    std::string currentLine;
    int pos = 0;
    int line = 1;
    int col = 1;
    std::vector<Token> tokens;
    std::vector<int> indentStack = {0};
    std::string detectedLang;  // idioma del archivo
    std::string currentFile;  // archivo que se esta procesando (trackeado via # Import: markers)

    void handleIndent(int indent, int lineNum);
    void readString(char quote, int lineNum, int startCol);
    void readFString(char quote, int lineNum, int startCol);
    void readNumber(int lineNum, int startCol);
    void readIdentifier(int lineNum, int startCol);
    void readOperator(int lineNum, int startCol);
    int countIndent(const std::string& ln);
    void processLangDirective(int lineNum, int startCol);

    bool isDigit(char c) { return c >= '0' && c <= '9'; }
    bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                                  || (unsigned char)c >= 0xC0; }
    bool isAlphaNum(char c) { return isAlpha(c) || isDigit(c) || c == '_'; }

    static std::map<std::string, TokenType> buildKeywords();
    static const std::map<std::string, TokenType> KEYWORDS;

public:
    Lexer(const std::string& source) : source(source) {}
    LexResult tokenize();
    std::string getLang() const { return detectedLang; }
    void setFilename(const std::string& f) { currentFile = f; }
};
