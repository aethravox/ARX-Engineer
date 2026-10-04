// ============================================================
// Zen Programming Language - Lexer Implementation
// Tokenizador bilingue con soporte de indentacion y #lang
// ============================================================

#include "lexer.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>

// Tabla de palabras clave bilingues
std::map<std::string, TokenType> Lexer::buildKeywords() {
    std::map<std::string, TokenType> kw;
    // Condicionales
    kw["si"] = TokenType::SI;              kw["if"] = TokenType::IF;
    kw["sino"] = TokenType::SINO;          kw["else"] = TokenType::ELSE;
    kw["sino_si"] = TokenType::SINO_SI;    kw["elif"] = TokenType::ELIF;
    // Bucles
    kw["para"] = TokenType::PARA;          kw["for"] = TokenType::FOR;
    kw["desde"] = TokenType::DESDE;        kw["from"] = TokenType::FROM;
    kw["hasta"] = TokenType::HASTA;        kw["to"] = TokenType::TO;
    kw["mientras"] = TokenType::MIENTRAS;  kw["while"] = TokenType::WHILE;
    kw["en"] = TokenType::EN;              kw["in"] = TokenType::IN;
    kw["cada"] = TokenType::CADA;          kw["each"] = TokenType::EACH;
    kw["repetir"] = TokenType::REPETIR;    kw["repeat"] = TokenType::REPEAT;
    kw["veces"] = TokenType::VECES;        kw["times"] = TokenType::TIMES;
    kw["romper"] = TokenType::ROMPER;      kw["break"] = TokenType::BREAK;
    kw["continuar"] = TokenType::CONTINUAR; kw["continue"] = TokenType::CONTINUE;
    kw["siguiente"] = TokenType::CONTINUAR;  // sinonimo en español
    // Funciones
    kw["funcion"] = TokenType::FUNCION;    kw["function"] = TokenType::FUNCTION;
    kw["retorna"] = TokenType::RETORNA;    kw["return"] = TokenType::RETURN;
    // Imprimir
    kw["muestra"] = TokenType::MUESTRA;    kw["show"] = TokenType::SHOW;
    kw["print"] = TokenType::PRINT;        kw["mostrar"] = TokenType::MUESTRA;
    kw["imprime"] = TokenType::MUESTRA;
    // Logica
    kw["y"] = TokenType::Y;                kw["and"] = TokenType::AND;
    kw["o"] = TokenType::O;                kw["or"] = TokenType::OR;
    kw["no"] = TokenType::NO;              kw["not"] = TokenType::NOT;
    kw["es"] = TokenType::ES;              kw["is"] = TokenType::IS;
    // Match
    kw["coincidir"] = TokenType::COINCIDIR; kw["match"] = TokenType::MATCH;
    kw["caso"] = TokenType::CASO;          kw["case"] = TokenType::CASE;
    kw["por_defecto"] = TokenType::POR_DEFECTO; kw["default"] = TokenType::DEFAULT;
    // Errores
    kw["intentar"] = TokenType::INTENTAR;  kw["try"] = TokenType::TRY;
    kw["atrapar"] = TokenType::ATRAPAR;    kw["catch"] = TokenType::CATCH;
    kw["finalmente"] = TokenType::FINALMENTE; kw["finally"] = TokenType::FINALLY;
    kw["lanzar"] = TokenType::LANZAR;      kw["throw"] = TokenType::THROW;
    // Structs (sintaxis nueva)
    kw["estructura"] = TokenType::ESTRUCTURA; kw["struct"] = TokenType::STRUCT;
    // Memoria
    kw["memoria"] = TokenType::MEMORIA;    kw["memory"] = TokenType::MEMORY;
    kw["automatica"] = TokenType::AUTOMATICA; kw["auto"] = TokenType::AUTO;
    kw["manual"] = TokenType::MANUAL;
    kw["mut"] = TokenType::MUT;            kw["ref"] = TokenType::REF;
    // Plataforma / Modulos
    kw["plataforma"] = TokenType::PLATAFORMA; kw["platform"] = TokenType::PLATFORM;
    kw["importar"] = TokenType::IMPORTAR;  kw["import"] = TokenType::IMPORT;
    kw["incluir"] = TokenType::INCLUIR;    kw["include"] = TokenType::INCLUDE;
    // FFI
    kw["extern"] = TokenType::EXTERN;
    // Constantes
    kw["yes"] = TokenType::YES;            kw["verdad"] = TokenType::VERDAD;
    kw["falso"] = TokenType::FALSO;        kw["nada"] = TokenType::NADA;
    kw["nothing"] = TokenType::NOTHING;
    return kw;
}

const std::map<std::string, TokenType> Lexer::KEYWORDS = Lexer::buildKeywords();

// Procesar #lang es / #lang en
void Lexer::processLangDirective(int lineNum, int startCol) {
    // Pos está en 'l' de 'lang', avanzar
    pos += 4; col += 4;

    // Saltar espacios
    while (pos < (int)currentLine.size() && currentLine[pos] == ' ') {
        pos++; col++;
    }

    // Leer el idioma
    std::string langValue;
    while (pos < (int)currentLine.size() && isAlphaNum(currentLine[pos])) {
        langValue += currentLine[pos];
        pos++; col++;
    }

    // Normalizar a minusculas
    std::string langLower = langValue;
    std::transform(langLower.begin(), langLower.end(), langLower.begin(), ::tolower);

    if (langLower == "es" || langLower == "espanol" || langLower == "español") {
        detectedLang = "es";
    } else if (langLower == "en" || langLower == "english" || langLower == "ingles" || langLower == "inglés") {
        detectedLang = "en";
    } else {
        throw std::runtime_error(
            "Idioma desconocido '" + langValue + "' en #lang. Usa 'es' o 'en'. "
            "Linea " + std::to_string(lineNum));
    }

    tokens.push_back({TokenType::LANG_HASH, "#lang " + detectedLang, lineNum, startCol});
}

LexResult Lexer::tokenize() {
    // Dividir source en lineas
    std::vector<std::string> lines;
    std::string ln;
    for (char c : source) {
        if (c == '\n') {
            lines.push_back(ln);
            ln.clear();
        } else {
            ln += c;
        }
    }
    lines.push_back(ln); // ultima linea (puede estar vacia)

    for (size_t i = 0; i < lines.size(); i++) {
        currentLine = lines[i];
        this->line = (int)(i + 1);
        col = 1;
        pos = 0;

        // Calcular indentacion
        int indent = countIndent(currentLine);
        col = indent + 1;
        pos = indent;

        // VERIFICAR si la linea es vacia ANTES de handleIndent
        bool isEmpty = true;
        for (int j = indent; j < (int)currentLine.size(); j++) {
            if (currentLine[j] != ' ' && currentLine[j] != '\t') {
                isEmpty = false;
                break;
            }
        }
        if (isEmpty) {
            if (i < lines.size() - 1)
                tokens.push_back({TokenType::NEWLINE, "\\n", this->line, col});
            continue;
        }

        // Solo manejar indentacion si la linea NO es vacia
        handleIndent(indent, this->line);

        // Verificar si es #lang directive (debe ser el primer token)
        if (pos < (int)currentLine.size() && currentLine[pos] == '#') {
            // Comprobar si es #lang
            if (pos + 1 < (int)currentLine.size() &&
                currentLine[pos + 1] == 'l' &&
                pos + 2 < (int)currentLine.size() &&
                currentLine[pos + 2] == 'a' &&
                pos + 3 < (int)currentLine.size() &&
                currentLine[pos + 3] == 'g') {
                processLangDirective(this->line, col);
                if (i < lines.size() - 1)
                    tokens.push_back({TokenType::NEWLINE, "\\n", this->line, col});
                continue;
            }
            // Es un comentario normal
            if (i < lines.size() - 1)
                tokens.push_back({TokenType::NEWLINE, "\\n", this->line, col});
            continue;
        }

        // Tokenizar contenido de la linea
        while (pos < (int)currentLine.size()) {
            // Saltar espacios entre tokens
            while (pos < (int)currentLine.size() &&
                   (currentLine[pos] == ' ' || currentLine[pos] == '\t')) {
                pos++; col++;
            }
            if (pos >= (int)currentLine.size()) break;

            char ch = currentLine[pos];
            if (ch == '#') break;

            // F-string: f"..." o f'...'
            // Detectar f o F seguido de comilla (pero no como parte de un identificador más largo)
            if ((ch == 'f' || ch == 'F') && pos + 1 < (int)currentLine.size() &&
                (currentLine[pos + 1] == '"' || currentLine[pos + 1] == '\'') &&
                (pos == 0 || !isAlphaNum(currentLine[pos - 1]))) {
                char quote = currentLine[pos + 1];
                // readFString espera pos en la 'f', avanza internamente
                readFString(quote, this->line, col);
            } else if (ch == '"' || ch == '\'') {
                readString(ch, this->line, col);
            } else if (isDigit(ch) || (ch == '.' && pos + 1 < (int)currentLine.size()
                       && isDigit(currentLine[pos + 1]))) {
                readNumber(this->line, col);
            } else if (isAlpha(ch) || ch == '_') {
                readIdentifier(this->line, col);
            } else {
                readOperator(this->line, col);
            }
        }

        if (i < lines.size() - 1) {
            tokens.push_back({TokenType::NEWLINE, "\\n", this->line, col});
        }
    }

    // Cerrar indentacion pendiente
    while ((int)indentStack.size() > 1) {
        indentStack.pop_back();
        tokens.push_back({TokenType::DEDENT, "DEDENT", this->line, 1});
    }

    tokens.push_back({TokenType::EOF_TOKEN, "EOF", this->line, col});
    return {tokens, detectedLang};
}

int Lexer::countIndent(const std::string& ln) {
    int count = 0;
    for (char c : ln) {
        if (c == ' ') count++;
        else if (c == '\t') count += 4;
        else break;
    }
    return count;
}

void Lexer::handleIndent(int indent, int lineNum) {
    int current = indentStack.back();
    if (indent > current) {
        indentStack.push_back(indent);
        tokens.push_back({TokenType::INDENT, "INDENT", lineNum, 1});
    } else if (indent < current) {
        while ((int)indentStack.size() > 1 && indentStack.back() > indent) {
            indentStack.pop_back();
            tokens.push_back({TokenType::DEDENT, "DEDENT", lineNum, 1});
        }
        if (indentStack.back() != indent) {
            throw std::runtime_error(
                "Error de indentacion inesperada en la linea " + std::to_string(lineNum));
        }
    }
}

void Lexer::readString(char quote, int lineNum, int startCol) {
    pos++; col++;
    std::string value;
    while (pos < (int)currentLine.size()) {
        char ch = currentLine[pos];
        if (ch == '\\' && pos + 1 < (int)currentLine.size()) {
            pos++; col++;
            char next = currentLine[pos];
            switch (next) {
                case 'n':  value += '\n'; break;
                case 't':  value += '\t'; break;
                case '\\': value += '\\'; break;
                case '{':  value += '{';  break;
                case '}':  value += '}';  break;
                default:   value += next;   break;
            }
        } else if (ch == quote) {
            pos++; col++;
            tokens.push_back({TokenType::STRING, value, lineNum, startCol});
            return;
        } else {
            value += ch;
        }
        pos++; col++;
    }
    throw std::runtime_error(
        "Cadena sin cerrar en la linea " + std::to_string(lineNum));
}

// F-string: f"Hola {nombre}, tienes {edad} años"
// Se tokeniza como FSTRING con el contenido crudo (incluye las {}).
// El parser se encarga de separar las partes literales de las expresiones.
void Lexer::readFString(char quote, int lineNum, int startCol) {
    pos += 2; col += 2;  // saltar la 'f' y la comilla inicial
    std::string value;
    while (pos < (int)currentLine.size()) {
        char ch = currentLine[pos];
        if (ch == '\\' && pos + 1 < (int)currentLine.size()) {
            pos++; col++;
            char next = currentLine[pos];
            switch (next) {
                case 'n':  value += '\n'; break;
                case 't':  value += '\t'; break;
                case '\\': value += '\\'; break;
                case '{':  value += "\\{";  break;  // escapar literal {
                case '}':  value += "\\}";  break;  // escapar literal }
                default:   value += next;   break;
            }
        } else if (ch == quote) {
            pos++; col++;
            tokens.push_back({TokenType::FSTRING, value, lineNum, startCol});
            return;
        } else {
            value += ch;
        }
        pos++; col++;
    }
    throw std::runtime_error(
        "F-string sin cerrar en la linea " + std::to_string(lineNum));
}

void Lexer::readNumber(int lineNum, int startCol) {
    std::string value;
    bool hasDot = false;
    while (pos < (int)currentLine.size()) {
        char ch = currentLine[pos];
        if (ch == '.' && !hasDot) {
            hasDot = true;
            value += ch;
        } else if (isDigit(ch)) {
            value += ch;
        } else {
            break;
        }
        pos++; col++;
    }
    tokens.push_back({TokenType::NUMBER, value, lineNum, startCol});
}

void Lexer::readIdentifier(int lineNum, int startCol) {
    std::string value;
    while (pos < (int)currentLine.size() && isAlphaNum(currentLine[pos])) {
        value += currentLine[pos];
        pos++; col++;
    }
    // Buscar en palabras clave
    auto it = KEYWORDS.find(value);
    TokenType type = (it != KEYWORDS.end()) ? it->second : TokenType::IDENTIFIER;
    tokens.push_back({type, value, lineNum, startCol});
}

void Lexer::readOperator(int lineNum, int startCol) {
    char ch = currentLine[pos];

    // Operadores de 2 caracteres
    if (pos + 1 < (int)currentLine.size()) {
        std::string two = std::string(1, ch) + currentLine[pos + 1];
        std::map<std::string, TokenType> twoCharOps = {
            {"==", TokenType::EQEQ}, {"!=", TokenType::NEQ},
            {"<=", TokenType::LTE},  {">=", TokenType::GTE},
            {"->", TokenType::ARROW},
            {"+=", TokenType::PLUS_ASSIGN},   {"-=", TokenType::MINUS_ASSIGN},
            {"*=", TokenType::STAR_ASSIGN},   {"/=", TokenType::SLASH_ASSIGN},
            {"%=", TokenType::PERCENT_ASSIGN}, {"^=", TokenType::POWER_ASSIGN},
        };
        auto it = twoCharOps.find(two);
        if (it != twoCharOps.end()) {
            tokens.push_back({it->second, two, lineNum, startCol});
            pos += 2; col += 2;
            return;
        }
    }

    // Operadores de 1 caracter
    std::map<char, TokenType> oneCharOps = {
        {'+', TokenType::PLUS}, {'-', TokenType::MINUS},
        {'*', TokenType::STAR}, {'/', TokenType::SLASH},
        {'%', TokenType::PERCENT}, {'^', TokenType::POWER},
        {'=', TokenType::ASSIGN},
        {'(', TokenType::LPAREN}, {')', TokenType::RPAREN},
        {'[', TokenType::LBRACKET}, {']', TokenType::RBRACKET},
        {'{', TokenType::LBRACE}, {'}', TokenType::RBRACE},
        {',', TokenType::COMMA}, {'.', TokenType::DOT},
        {'<', TokenType::LT}, {'>', TokenType::GT},
        {':', TokenType::COLON}, {'?', TokenType::QUESTION},
    };

    auto it = oneCharOps.find(ch);
    if (it != oneCharOps.end()) {
        tokens.push_back({it->second, std::string(1, ch), lineNum, startCol});
        pos++; col++;
    } else {
        throw std::runtime_error(
            "Caracter desconocido '" + std::string(1, ch) +
            "' en la linea " + std::to_string(lineNum) +
            ". Revisa si hay un simbolo extra o mal escrito.");
    }
}