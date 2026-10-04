// ============================================================
// Zen Programming Language - Parser Implementation
// Parser recursivo descendente | Bloques por indentacion
// v1.2: while, for-each, repeat, break, continue, ternario,
//       asignacion aumentada, plataforma
// ============================================================

#include "parser.h"
#include <algorithm>

std::vector<std::unique_ptr<ASTNode>> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> program;
    skipNewlines();

    // Saltar #lang directive si existe
    if (check(TokenType::LANG_HASH)) {
        advance();
        skipNewlines();
    }

    while (!check(TokenType::EOF_TOKEN)) {
        auto stmt = parseStatement();
        if (stmt) program.push_back(std::move(stmt));
        // Consumir newlines y dedents entre sentencias
        while (check(TokenType::NEWLINE) || check(TokenType::DEDENT))
            advance();
    }
    return program;
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseBlock() {
    std::vector<std::unique_ptr<ASTNode>> stmts;
    skipNewlines();

    while (!isBlockEnd(peek().type)) {
        auto stmt = parseStatement();
        if (stmt) stmts.push_back(std::move(stmt));
        while (check(TokenType::NEWLINE)) advance();
    }

    if (check(TokenType::DEDENT)) advance();
    return stmts;
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    skipNewlines();
    auto tok = peek();

    // muestra / show / print
    if (tok.type == TokenType::MUESTRA || tok.type == TokenType::SHOW ||
        tok.type == TokenType::PRINT) {
        int ln = tok.line;
        advance();
        auto expr = parseExpression();
        return std::make_unique<PrintStmt>(std::move(expr), ln);
    }

    // si / if
    if (tok.type == TokenType::SI || tok.type == TokenType::IF) {
        return parseIf();
    }

    // para / for
    if (tok.type == TokenType::PARA || tok.type == TokenType::FOR) {
        // Mirar si es for-each o for-range
        // para cada / for each
        if (pos + 1 < tokens.size()) {
            TokenType next = tokens[pos + 1].type;
            if (next == TokenType::CADA || next == TokenType::EACH) {
                return parseForEach();
            }
        }
        return parseForRange();
    }

    // mientras / while
    if (tok.type == TokenType::MIENTRAS || tok.type == TokenType::WHILE) {
        return parseWhile();
    }

    // repetir / repeat
    if (tok.type == TokenType::REPETIR || tok.type == TokenType::REPEAT) {
        return parseRepeat();
    }

    // funcion / function
    if (tok.type == TokenType::FUNCION || tok.type == TokenType::FUNCTION) {
        return parseFuncDecl();
    }

    // retorna / return
    if (tok.type == TokenType::RETORNA || tok.type == TokenType::RETURN) {
        int ln = tok.line;
        advance();
        auto expr = parseExpression();
        return std::make_unique<ReturnStmt>(std::move(expr), ln);
    }

    // romper / break
    if (tok.type == TokenType::ROMPER || tok.type == TokenType::BREAK) {
        int ln = tok.line;
        advance();
        return std::make_unique<BreakStmt>(ln);
    }

    // continuar / continue
    if (tok.type == TokenType::CONTINUAR || tok.type == TokenType::CONTINUE) {
        int ln = tok.line;
        advance();
        return std::make_unique<ContinueStmt>(ln);
    }

    // plataforma / platform
    if (tok.type == TokenType::PLATAFORMA || tok.type == TokenType::PLATFORM) {
        return parsePlatformBlock();
    }

    // coincidir / match
    if (tok.type == TokenType::COINCIDIR || tok.type == TokenType::MATCH) {
        return parseMatch();
    }

    // extern "lib" nombre(args) -> tipo
    if (tok.type == TokenType::EXTERN) {
        return parseExternDecl();
    }

    // estructura/struct Nombre:        (sintaxis nueva, más legible)
    //     campo1
    //     campo2
    //     campo3
    if (tok.type == TokenType::ESTRUCTURA || tok.type == TokenType::STRUCT) {
        int ln = tok.line;
        advance();  // consumir estructura/struct
        // Nombre del struct
        if (!isIdentifierLike(peek().type)) {
            throw std::runtime_error(
                "Se esperaba nombre de struct despues de 'estructura' en la linea " +
                std::to_string(ln));
        }
        std::string structName = advance().value;
        // ':' opcional
        if (check(TokenType::COLON)) advance();
        // Esperar NEWLINE + INDENT
        if (!check(TokenType::NEWLINE)) {
            throw std::runtime_error(
                "Se esperaba nueva linea despues de 'estructura " + structName + ":' en la linea " +
                std::to_string(ln));
        }
        advance();  // NEWLINE
        if (!check(TokenType::INDENT)) {
            throw std::runtime_error(
                "Se esperaba indentacion con los campos del struct en la linea " +
                std::to_string(ln));
        }
        advance();  // INDENT
        // Parsear campos (uno por linea)
        std::vector<std::string> fields;
        skipNewlines();
        while (!check(TokenType::DEDENT) && !check(TokenType::EOF_TOKEN)) {
            if (!isIdentifierLike(peek().type)) {
                throw std::runtime_error(
                    "Se esperaba nombre de campo en struct '" + structName +
                    "' en la linea " + std::to_string(peek().line));
            }
            fields.push_back(advance().value);
            // Permitir coma opcional al final
            if (check(TokenType::COMMA)) advance();
            skipNewlines();
        }
        if (check(TokenType::DEDENT)) advance();
        return std::make_unique<StructDecl>(structName, std::move(fields), ln);
    }

    // Declaracion de struct:  Nombre {campo1, campo2, ...}
    // Se detecta con lookahead: IDENTIFIER-like seguido de '{' ... '}' NEWLINE
    // Y los elementos son solo nombres (sin ':') o strings.
    // Importante: debe estar al inicio de linea (statement-level).
    if (isIdentifierLike(tok.type) && pos + 1 < tokens.size()
        && tokens[pos + 1].type == TokenType::LBRACE) {
        // Guardar posicion para backtrack
        size_t savePos = pos;
        int ln = tok.line;
        std::string structName = tok.value;

        // Avanzar IDENTIFIER y '{'
        advance();  // IDENTIFIER
        advance();  // LBRACE

        // Saltar newlines
        skipNewlines();

        // Intentar parsear como struct DECL (solo nombres separados por coma)
        std::vector<std::string> fields;
        bool isDecl = true;
        if (!check(TokenType::RBRACE)) {
            // Debe haber al menos un nombre
            if (!isIdentifierLike(peek().type)) {
                isDecl = false;
            } else {
                fields.push_back(advance().value);
                while (match(TokenType::COMMA)) {
                    skipNewlines();
                    if (!isIdentifierLike(peek().type)) {
                        isDecl = false;
                        break;
                    }
                    fields.push_back(advance().value);
                }
            }
        }
        skipNewlines();

        if (isDecl && check(TokenType::RBRACE)) {
            advance();  // consumir '}'
            // Si despues del '}' hay NEWLINE o EOF, es declaracion de struct
            // NO hacer skipNewlines aqui porque consumiria el NEWLINE que necesitamos detectar
            if (check(TokenType::NEWLINE) || check(TokenType::INDENT)
                || check(TokenType::EOF_TOKEN)) {
                return std::make_unique<StructDecl>(structName, std::move(fields), ln);
            }
        }

        // No era struct decl: restaurar y dejar que parseExpression maneje
        // (es un struct literal: Punto {x: 10, y: 5})
        pos = savePos;
    }

    // Declaracion de funcion sin keyword:  nombre(args)
    //                                         <cuerpo indentado>
    // Se detecta con lookahead: IDENTIFIER-like seguido de '(' ... ')' NEWLINE.
    // Si despues del ')' hay un NEWLINE, es declaracion de funcion.
    // Si no, es una llamada a funcion usada como expresion (ej: factorial(10)).
    if (isIdentifierLike(tok.type) && pos + 1 < tokens.size()
        && tokens[pos + 1].type == TokenType::LPAREN) {
        // Guardar posicion para hacer backtrack si no es funcion
        size_t savePos = pos;
        int ln = tok.line;

        // Avanzar el IDENTIFIER y el '('
        advance(); // IDENTIFIER
        advance(); // LPAREN

        // Saltar newlines dentro de los parametros (por si acaso)
        skipNewlines();

        // Recoger parametros (identificadores separados por coma)
        std::vector<std::string> params;
        bool paramsOk = true;
        if (!check(TokenType::RPAREN)) {
            if (!isIdentifierLike(peek().type)) {
                paramsOk = false;
            } else {
                params.push_back(advance().value);
                while (match(TokenType::COMMA)) {
                    skipNewlines();
                    if (!isIdentifierLike(peek().type)) {
                        paramsOk = false;
                        break;
                    }
                    params.push_back(advance().value);
                }
            }
        }
        skipNewlines();

        // Verificar que cierre el parentesis
        bool isFuncDecl = false;
        if (paramsOk && check(TokenType::RPAREN)) {
            advance();  // consumir ')'
            // Solo es declaracion de funcion si despues del ')' hay NEWLINE + INDENT.
            // Si hay NEWLINE sin INDENT, o si hay cualquier otra cosa, es una llamada.
            if (check(TokenType::NEWLINE)) {
                advance();  // consumir NEWLINE
                if (check(TokenType::INDENT)) {
                    isFuncDecl = true;
                }
                // Si no hay INDENT, no es funcion - restaurar
            }
        }

        if (isFuncDecl) {
            // Ya consumimos el ')' y los newlines; ahora esperamos INDENT
            if (check(TokenType::NEWLINE)) advance();
            expect(TokenType::INDENT,
                "Se esperaba indentacion para el cuerpo de la funcion");
            auto body = parseBlock();
            return std::make_unique<FuncDecl>(tok.value, std::move(params),
                                              std::move(body), ln);
        }

        // No era declaracion de funcion: restaurar y dejar que parseExpression
        // haga su trabajo (es una llamada a funcion como expresion).
        pos = savePos;
    }

    // Expresion como sentencia (asignacion, asignacion aumentada, o expresion)
    auto expr = parseExpression();

    // Asignacion aumentada: x += 5, x -= 3, etc.
    if (expr->kind == NodeType::Identifier) {
        std::string name = static_cast<Identifier*>(expr.get())->name;
        std::map<TokenType, std::string> augOps = {
            {TokenType::PLUS_ASSIGN, "+"},
            {TokenType::MINUS_ASSIGN, "-"},
            {TokenType::STAR_ASSIGN, "*"},
            {TokenType::SLASH_ASSIGN, "/"},
            {TokenType::PERCENT_ASSIGN, "%"},
            {TokenType::POWER_ASSIGN, "^"},
        };
        auto it = augOps.find(peek().type);
        if (it != augOps.end()) {
            int ln = peek().line;
            advance(); // consumir el +=, -=, etc.
            auto value = parseExpression();
            return std::make_unique<AugAssignStmt>(name, it->second, std::move(value), ln);
        }

        // Asignacion normal: x = expr
        if (check(TokenType::ASSIGN)) {
            int ln = peek().line;
            advance();
            auto value = parseExpression();
            return std::make_unique<AssignStmt>(name, std::move(value), ln);
        }
    }

    // Asignacion por indice: lista[expr] = valor
    if (expr->kind == NodeType::IndexAccess && check(TokenType::ASSIGN)) {
        int ln = peek().line;
        advance();  // consumir =
        auto value = parseExpression();
        // Cast seguro: ya verificamos que es IndexAccess
        auto* idxAccess = static_cast<IndexAccess*>(expr.get());
        // Transferir ownership: hay que mover base e index fuera del expr
        auto base = std::move(idxAccess->base);
        auto idx = std::move(idxAccess->index);
        return std::make_unique<IndexAssign>(std::move(base), std::move(idx),
                                              std::move(value), ln);
    }

    // Asignacion por miembro: struct.member = valor
    if (expr->kind == NodeType::MemberAccess && check(TokenType::ASSIGN)) {
        int ln = peek().line;
        advance();  // consumir =
        auto value = parseExpression();
        auto* memAccess = static_cast<MemberAccess*>(expr.get());
        auto base = std::move(memAccess->base);
        std::string member = memAccess->member;
        return std::make_unique<MemberAssign>(std::move(base), member,
                                               std::move(value), ln);
    }

    return std::make_unique<ExprStmt>(std::move(expr), tok.line);
}

// ============================================================
// Bloques compuestos
// ============================================================

std::unique_ptr<ASTNode> Parser::parseIf() {
    int ln = peek().line;
    advance(); // consumir si/if

    auto cond = parseExpression();

    if (check(TokenType::NEWLINE)) advance();
    expect(TokenType::INDENT, "Se esperaba indentacion despues de la condicion");
    auto thenBody = parseBlock();

    // Bloque sino/else (opcional)
    std::vector<std::unique_ptr<ASTNode>> elseBody;
    skipNewlines();

    if (check(TokenType::SINO) || check(TokenType::ELSE)) {
        advance();

        // sino si / elif
        if (check(TokenType::SI) || check(TokenType::IF) ||
            check(TokenType::SINO_SI) || check(TokenType::ELIF)) {
            auto elifStmt = parseIf();
            elseBody.push_back(std::move(elifStmt));
        } else {
            if (check(TokenType::NEWLINE)) advance();
            expect(TokenType::INDENT, "Se esperaba indentacion despues de sino/else");
            elseBody = parseBlock();
        }
    }

    return std::make_unique<IfStmt>(std::move(cond), std::move(thenBody),
                                   std::move(elseBody), ln);
}

std::unique_ptr<ASTNode> Parser::parseForRange() {
    int ln = peek().line;
    advance(); // consumir para/for

    // nombre variable
    auto varTok = expectIdentifier(
        "Se esperaba nombre de variable despues de para/for");

    // desde / from
    if (!check(TokenType::DESDE) && !check(TokenType::FROM)) {
        throw std::runtime_error(
            "Se esperaba 'desde' o 'from' en la linea " + std::to_string(ln));
    }
    advance();

    auto fromExpr = parseExpression();

    // hasta / to
    if (!check(TokenType::HASTA) && !check(TokenType::TO)) {
        throw std::runtime_error(
            "Se esperaba 'hasta' o 'to' en la linea " + std::to_string(ln));
    }
    advance();

    auto toExpr = parseExpression();

    if (check(TokenType::NEWLINE)) advance();
    expect(TokenType::INDENT, "Se esperaba indentacion despues del para/for");
    auto body = parseBlock();

    return std::make_unique<ForRangeStmt>(varTok.value, std::move(fromExpr),
                                          std::move(toExpr), std::move(body), ln);
}

std::unique_ptr<ASTNode> Parser::parseForEach() {
    int ln = peek().line;
    advance(); // consumir para/for
    advance(); // consumir cada/each

    // nombre variable
    auto varTok = expectIdentifier(
        "Se esperaba nombre de variable despues de 'para cada'");

    // en / in
    if (!check(TokenType::EN) && !check(TokenType::IN)) {
        throw std::runtime_error(
            "Se esperaba 'en' o 'in' en la linea " + std::to_string(ln));
    }
    advance();

    // Expresion iterable (por ahora, soportamos rangos como expresion)
    auto iterable = parseExpression();

    if (check(TokenType::NEWLINE)) advance();
    expect(TokenType::INDENT, "Se esperaba indentacion despues del para cada");
    auto body = parseBlock();

    return std::make_unique<ForEachStmt>(varTok.value, std::move(iterable),
                                        std::move(body), ln);
}

std::unique_ptr<ASTNode> Parser::parseWhile() {
    int ln = peek().line;
    advance(); // consumir mientras/while

    auto cond = parseExpression();

    if (check(TokenType::NEWLINE)) advance();
    expect(TokenType::INDENT, "Se esperaba indentacion despues de mientras/while");
    auto body = parseBlock();

    return std::make_unique<WhileStmt>(std::move(cond), std::move(body), ln);
}

std::unique_ptr<ASTNode> Parser::parseRepeat() {
    int ln = peek().line;
    advance(); // consumir repetir/repeat

    auto count = parseExpression();

    // veces / times
    if (!check(TokenType::VECES) && !check(TokenType::TIMES)) {
        throw std::runtime_error(
            "Se esperaba 'veces' o 'times' en la linea " + std::to_string(ln));
    }
    advance();

    if (check(TokenType::NEWLINE)) advance();
    expect(TokenType::INDENT, "Se esperaba indentacion despues de repetir/repeat");
    auto body = parseBlock();

    return std::make_unique<RepeatStmt>(std::move(count), std::move(body), ln);
}

std::unique_ptr<ASTNode> Parser::parsePlatformBlock() {
    int ln = peek().line;
    advance(); // consumir plataforma/platform

    // Leer nombre de la plataforma: identificador o string
    std::string platformName;
    if (isIdentifierLike(peek().type)) {
        platformName = advance().value;
    } else if (check(TokenType::STRING)) {
        platformName = advance().value;
    } else {
        throw std::runtime_error(
            "Se esperaba nombre de plataforma (ej: windows, linux, macos, android, web) "
            "en la linea " + std::to_string(ln));
    }

    // Normalizar a minusculas
    std::transform(platformName.begin(), platformName.end(),
                   platformName.begin(), ::tolower);

    if (check(TokenType::NEWLINE)) advance();
    expect(TokenType::INDENT, "Se esperaba indentacion despues de plataforma");
    auto body = parseBlock();

    return std::make_unique<PlatformBlock>(platformName, std::move(body), ln);
}

std::unique_ptr<ASTNode> Parser::parseMatch() {
    int ln = peek().line;
    advance();  // consumir coincidir/match

    // La expr a comparar
    auto subject = parseExpression();
    if (check(TokenType::NEWLINE)) advance();

    // Esperamos INDENT
    expect(TokenType::INDENT, "Se esperaba indentacion despues de coincidir/match");

    // Parsear casos
    std::vector<MatchCase> cases;
    while (true) {
        skipNewlines();
        // caso / case
        if (check(TokenType::CASO) || check(TokenType::CASE)) {
            advance();
            MatchCase mc;
            mc.isDefault = false;
            // El valor del caso
            mc.value = parseExpression();
            if (check(TokenType::NEWLINE)) advance();
            expect(TokenType::INDENT, "Se esperaba indentacion despues de caso/valor");
            mc.body = parseBlock();
            cases.push_back(std::move(mc));
        }
        // por_defecto / default
        else if (check(TokenType::POR_DEFECTO) || check(TokenType::DEFAULT)) {
            advance();
            MatchCase mc;
            mc.isDefault = true;
            if (check(TokenType::NEWLINE)) advance();
            expect(TokenType::INDENT, "Se esperaba indentacion despues de por_defecto/default");
            mc.body = parseBlock();
            cases.push_back(std::move(mc));
        }
        else {
            break;
        }
        // Consumir newlines/dedents entre casos
        while (check(TokenType::NEWLINE) || check(TokenType::DEDENT)) advance();
    }

    // Consumir el DEDENT final
    if (check(TokenType::DEDENT)) advance();

    return std::make_unique<MatchStmt>(std::move(subject), std::move(cases), ln);
}

std::unique_ptr<ASTNode> Parser::parseFuncDecl() {
    int ln = peek().line;
    advance(); // consumir funcion/function

    // Para Zen sobre Zen: permitir que keywords se usen como nombres de funcion
    // (ej: funcion muestra(x), funcion longitud(s), etc.)
    // Si el token no es IDENTIFIER pero tampoco es un delimitador, lo aceptamos
    // como nombre de funcion.
    Token nameTok;
    auto t = peek();
    if (t.type == TokenType::IDENTIFIER ||
        t.type == TokenType::MUESTRA || t.type == TokenType::SHOW ||
        t.type == TokenType::PRINT ||
        t.type == TokenType::SI || t.type == TokenType::IF ||
        t.type == TokenType::PARA || t.type == TokenType::FOR ||
        t.type == TokenType::MIENTRAS || t.type == TokenType::WHILE) {
        nameTok = advance();
    } else {
        // Aceptar cualquier token que tenga un valor de texto como nombre
        // (esto permite que cualquier keyword sea nombre de funcion)
        if (!t.value.empty() && t.type != TokenType::LPAREN &&
            t.type != TokenType::RPAREN && t.type != TokenType::NEWLINE &&
            t.type != TokenType::INDENT && t.type != TokenType::DEDENT &&
            t.type != TokenType::EOF_TOKEN) {
            nameTok = advance();
        } else {
            throw std::runtime_error(
                "Se esperaba nombre de funcion en la linea " + std::to_string(t.line));
        }
    }

    expect(TokenType::LPAREN, "Se esperaba '(' despues del nombre de funcion");

    std::vector<std::string> params;
    if (!check(TokenType::RPAREN)) {
        params.push_back(
            expectIdentifier("Se esperaba nombre de parametro").value);
        while (match(TokenType::COMMA)) {
            params.push_back(
                expectIdentifier("Se esperaba nombre de parametro").value);
        }
    }
    expect(TokenType::RPAREN, "Se esperaba ')' al final de los parametros");

    if (check(TokenType::NEWLINE)) advance();
    expect(TokenType::INDENT, "Se esperaba indentacion para el cuerpo de la funcion");
    auto body = parseBlock();

    return std::make_unique<FuncDecl>(nameTok.value, std::move(params),
                                     std::move(body), ln);
}

// ============================================================
// Expresiones - Precedencia (menor a mayor):
//   ternario  <  or/o  <  and/y  <  not/no  <  comparacion
//   <  +/-  <  */%  <  ^  <  -unario  <  primario
// ============================================================

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseTernary();
}

std::unique_ptr<ASTNode> Parser::parseTernary() {
    auto expr = parseOr();
    if (check(TokenType::QUESTION)) {
        int ln = peek().line;
        advance(); // consumir ?
        auto thenExpr = parseExpression();
        if (!check(TokenType::COLON)) {
            throw std::runtime_error(
                "Se esperaba ':' en la expresion ternaria, linea " + std::to_string(ln));
        }
        advance(); // consumir :
        auto elseExpr = parseTernary(); // asociativo a la derecha
        return std::make_unique<TernaryOp>(std::move(expr), std::move(thenExpr),
                                           std::move(elseExpr), ln);
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseOr() {
    auto left = parseAnd();
    while (check(TokenType::O) || check(TokenType::OR)) {
        int ln = peek().line;
        advance();
        auto right = parseAnd();
        left = std::make_unique<BinaryOp>("or", std::move(left), std::move(right), ln);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseAnd() {
    auto left = parseNot();
    while (check(TokenType::Y) || check(TokenType::AND)) {
        int ln = peek().line;
        advance();
        auto right = parseNot();
        left = std::make_unique<BinaryOp>("and", std::move(left), std::move(right), ln);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseNot() {
    if (check(TokenType::NO) || check(TokenType::NOT)) {
        int ln = peek().line;
        advance();
        auto next = peek();
        if (next.type == TokenType::NUMBER || next.type == TokenType::STRING ||
            next.type == TokenType::IDENTIFIER || next.type == TokenType::LPAREN ||
            next.type == TokenType::NO || next.type == TokenType::NOT ||
            next.type == TokenType::MINUS || next.type == TokenType::YES ||
            next.type == TokenType::VERDAD || next.type == TokenType::FALSO ||
            next.type == TokenType::NADA || next.type == TokenType::NOTHING) {
            auto operand = parseNot();
            return std::make_unique<UnaryOp>("not", std::move(operand), ln);
        } else {
            return std::make_unique<BoolLit>(false, ln);
        }
    }
    return parseComparison();
}

std::unique_ptr<ASTNode> Parser::parseComparison() {
    auto left = parseAddSub();
    while (check(TokenType::EQEQ) || check(TokenType::NEQ) ||
           check(TokenType::LT) || check(TokenType::GT) ||
           check(TokenType::LTE) || check(TokenType::GTE)) {
        int ln = peek().line;
        auto tok = advance();
        auto right = parseAddSub();
        left = std::make_unique<BinaryOp>(tok.value, std::move(left), std::move(right), ln);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseAddSub() {
    auto left = parseMulDiv();
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        int ln = peek().line;
        auto tok = advance();
        auto right = parseMulDiv();
        left = std::make_unique<BinaryOp>(tok.value, std::move(left), std::move(right), ln);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseMulDiv() {
    auto left = parsePower();
    while (check(TokenType::STAR) || check(TokenType::SLASH) || check(TokenType::PERCENT)) {
        int ln = peek().line;
        auto tok = advance();
        auto right = parsePower();
        left = std::make_unique<BinaryOp>(tok.value, std::move(left), std::move(right), ln);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parsePower() {
    auto left = parseUnary();
    if (check(TokenType::POWER)) {
        int ln = peek().line;
        advance();
        auto right = parsePower();
        return std::make_unique<BinaryOp>("^", std::move(left), std::move(right), ln);
    }
    return left;
}

std::unique_ptr<ASTNode> Parser::parseUnary() {
    if (check(TokenType::MINUS)) {
        int ln = peek().line;
        advance();
        auto operand = parseUnary();
        return std::make_unique<UnaryOp>("-", std::move(operand), ln);
    }
    return parseCallOrPrimary();
}

std::unique_ptr<ASTNode> Parser::parseCallOrPrimary() {
    auto tok = peek();

    // Constantes booleanas
    if (tok.type == TokenType::YES || tok.type == TokenType::VERDAD) {
        advance();
        return std::make_unique<BoolLit>(true, tok.line);
    }
    if (tok.type == TokenType::FALSO) {
        advance();
        return std::make_unique<BoolLit>(false, tok.line);
    }
    // Nulo
    if (tok.type == TokenType::NADA || tok.type == TokenType::NOTHING) {
        advance();
        return std::make_unique<NullLit>(tok.line);
    }
    // Numero
    if (tok.type == TokenType::NUMBER) {
        advance();
        double val = std::stod(tok.value);
        return std::make_unique<NumberLit>(val, tok.line);
    }
    // Texto
    if (tok.type == TokenType::STRING) {
        advance();
        return std::make_unique<StringLit>(tok.value, tok.line);
    }
    // F-string: f"Hola {nombre}, tienes {edad}"
    if (tok.type == TokenType::FSTRING) {
        advance();
        return parseFString(tok.value, tok.line);
    }
    // Lista literal: [expr, expr, ...] o []
    if (tok.type == TokenType::LBRACKET) {
        int ln = tok.line;
        advance();  // consumir [
        std::vector<std::unique_ptr<ASTNode>> elements;
        skipNewlines();
        if (!check(TokenType::RBRACKET)) {
            elements.push_back(parseExpression());
            while (match(TokenType::COMMA)) {
                skipNewlines();
                if (check(TokenType::RBRACKET)) break;  // trailing comma
                elements.push_back(parseExpression());
            }
        }
        skipNewlines();
        expect(TokenType::RBRACKET, "Se esperaba ']' al final de la lista");
        return std::make_unique<ListLit>(std::move(elements), ln);
    }
    // Identificador o llamada a funcion
    // Aceptamos Y/O/ES/EN aqui para que se puedan usar como nombres
    // de variable cuando estan en posicion de expresion.
    // Ej:  y = 25  |  muestra o  |  saludo = es
    if (isIdentifierLike(tok.type)) {
        advance();

        // Struct literal: Nombre {campo: valor, ...}
        if (check(TokenType::LBRACE)) {
            int ln = tok.line;
            advance();  // consumir {
            std::vector<std::string> fieldNames;
            std::vector<std::unique_ptr<ASTNode>> fieldValues;
            // Saltar newlines y indents (struct literal puede ser multilinea)
            while (check(TokenType::NEWLINE) || check(TokenType::INDENT) || check(TokenType::DEDENT))
                advance();
            if (!check(TokenType::RBRACE)) {
                // Esperamos:  nombre: valor
                if (!isIdentifierLike(peek().type)) {
                    throw std::runtime_error(
                        "Se esperaba nombre de campo en struct literal linea " +
                        std::to_string(peek().line));
                }
                std::string fname = advance().value;
                expect(TokenType::COLON, "Se esperaba ':' despues del nombre del campo");
                auto fval = parseExpression();
                fieldNames.push_back(fname);
                fieldValues.push_back(std::move(fval));
                while (match(TokenType::COMMA)) {
                    while (check(TokenType::NEWLINE) || check(TokenType::INDENT) || check(TokenType::DEDENT))
                        advance();
                    if (check(TokenType::RBRACE)) break;  // trailing comma
                    if (!isIdentifierLike(peek().type)) {
                        throw std::runtime_error(
                            "Se esperaba nombre de campo en struct literal linea " +
                            std::to_string(peek().line));
                    }
                    std::string fname2 = advance().value;
                    expect(TokenType::COLON, "Se esperaba ':' despues del nombre del campo");
                    auto fval2 = parseExpression();
                    fieldNames.push_back(fname2);
                    fieldValues.push_back(std::move(fval2));
                }
            }
            // Saltar newlines y indents antes del }
            while (check(TokenType::NEWLINE) || check(TokenType::INDENT) || check(TokenType::DEDENT))
                advance();
            expect(TokenType::RBRACE, "Se esperaba '}' al final del struct literal");
            auto lit = std::make_unique<StructLit>(tok.value,
                std::move(fieldNames), std::move(fieldValues), ln);
            // Soporte para index/access despues del literal
            std::unique_ptr<ASTNode> result = std::move(lit);
            // Member access: structLit.x
            while (check(TokenType::DOT)) {
                int dln = peek().line;
                advance();
                if (!isIdentifierLike(peek().type)) {
                    throw std::runtime_error(
                        "Se esperaba nombre de miembro despues de '.' linea " +
                        std::to_string(dln));
                }
                std::string member = advance().value;
                result = std::make_unique<MemberAccess>(std::move(result), member, dln);
            }
            // Index access: structLit[0]
            while (check(TokenType::LBRACKET)) {
                int iln = peek().line;
                advance();
                auto idx = parseExpression();
                expect(TokenType::RBRACKET, "Se esperaba ']' despues del indice");
                result = std::make_unique<IndexAccess>(std::move(result), std::move(idx), iln);
            }
            return result;
        }

        if (check(TokenType::LPAREN)) {
            advance();
            std::vector<std::unique_ptr<ASTNode>> args;
            if (!check(TokenType::RPAREN)) {
                args.push_back(parseExpression());
                while (match(TokenType::COMMA)) {
                    args.push_back(parseExpression());
                }
            }
            expect(TokenType::RPAREN, "Se esperaba ')' al final de los argumentos");
            std::unique_ptr<ASTNode> call = std::make_unique<FuncCall>(tok.value, std::move(args), tok.line);
            // Soporte para llamada seguida de index: foo()[0]
            while (check(TokenType::LBRACKET)) {
                int iln = peek().line;
                advance();
                auto idx = parseExpression();
                expect(TokenType::RBRACKET, "Se esperaba ']' despues del indice");
                call = std::make_unique<IndexAccess>(std::move(call), std::move(idx), iln);
            }
            // Soporte para member access: foo().x
            while (check(TokenType::DOT)) {
                int dln = peek().line;
                advance();
                if (!isIdentifierLike(peek().type)) {
                    throw std::runtime_error(
                        "Se esperaba nombre de miembro despues de '.' linea " +
                        std::to_string(dln));
                }
                std::string member = advance().value;
                call = std::make_unique<MemberAccess>(std::move(call), member, dln);
            }
            return call;
        }
        std::unique_ptr<ASTNode> ident = std::make_unique<Identifier>(tok.value, tok.line);
        // Member access: ident.x (y chains: ident.x.y)
        while (check(TokenType::DOT)) {
            int dln = peek().line;
            advance();
            if (!isIdentifierLike(peek().type)) {
                throw std::runtime_error(
                    "Se esperaba nombre de miembro despues de '.' linea " +
                    std::to_string(dln));
            }
            std::string member = advance().value;
            ident = std::make_unique<MemberAccess>(std::move(ident), member, dln);
        }
        // Index access: ident[expr]
        while (check(TokenType::LBRACKET)) {
            int iln = peek().line;
            advance();
            auto idx = parseExpression();
            expect(TokenType::RBRACKET, "Se esperaba ']' despues del indice");
            ident = std::make_unique<IndexAccess>(std::move(ident), std::move(idx), iln);
        }
        // Member access despues de index: ident[0].x
        while (check(TokenType::DOT)) {
            int dln = peek().line;
            advance();
            if (!isIdentifierLike(peek().type)) {
                throw std::runtime_error(
                    "Se esperaba nombre de miembro despues de '.' linea " +
                    std::to_string(dln));
            }
            std::string member = advance().value;
            ident = std::make_unique<MemberAccess>(std::move(ident), member, dln);
        }
        return ident;
    }
    // Expresion parentizada
    if (tok.type == TokenType::LPAREN) {
        advance();
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Se esperaba ')'");
        return expr;
    }

    throw std::runtime_error(
        "No se esperaba '" + tok.value + "' en la linea " + std::to_string(tok.line));
}

// ============================================================
// FFI - parsing de declaraciones extern
// ============================================================

// Tipos de C para FFI (deben coincidir con ast.h)
//   int, uint, long, float, double, ptr, str, char, void
// El lexer trata estos como IDENTIFIER, asi que los reconocemos por valor.

bool Parser::checkCType() {
    if (!isIdentifierLike(peek().type)) return false;
    std::string v = peek().value;
    return v == "int" || v == "uint" || v == "long" || v == "float"
        || v == "double" || v == "ptr" || v == "str" || v == "char"
        || v == "void";
}

CType Parser::parseCType() {
    if (!checkCType()) {
        throw std::runtime_error(
            "Se esperaba tipo de C (int, double, str, etc.) en linea " +
            std::to_string(peek().line));
    }
    std::string v = advance().value;
    if (v == "int")     return CType::Int;
    if (v == "uint")    return CType::UInt;
    if (v == "long")    return CType::Long;
    if (v == "float")   return CType::Float;
    if (v == "double")  return CType::Double;
    if (v == "ptr")     return CType::Ptr;
    if (v == "str")     return CType::Str;
    if (v == "char")    return CType::Char;
    return CType::Void;
}

std::unique_ptr<ASTNode> Parser::parseExternDecl() {
    int ln = peek().line;
    advance();  // consumir 'extern'

    // Esperamos un string con el nombre de la libreria
    if (!check(TokenType::STRING)) {
        throw std::runtime_error(
            "Se esperaba nombre de libreria (string) despues de 'extern' en linea " +
            std::to_string(ln));
    }
    std::string libName = advance().value;

    // Nombre de la funcion
    auto nameTok = expectIdentifier("Se esperaba nombre de funcion externa");

    // Parentesis con tipos de parametros
    expect(TokenType::LPAREN, "Se esperaba '(' despues del nombre de funcion externa");

    std::vector<ExternParam> params;
    if (!check(TokenType::RPAREN)) {
        // ..., ...
        if (!checkCType()) {
            throw std::runtime_error(
                "Se esperaba tipo de C en parametros de funcion externa linea " +
                std::to_string(peek().line));
        }
        CType t = parseCType();
        bool isPtr = false;
        if (check(TokenType::STAR)) { advance(); isPtr = true; }
        params.push_back({t, isPtr});
        while (match(TokenType::COMMA)) {
            if (!checkCType()) {
                throw std::runtime_error(
                    "Se esperaba tipo de C en parametros de funcion externa linea " +
                    std::to_string(peek().line));
            }
            CType t2 = parseCType();
            bool isPtr2 = false;
            if (check(TokenType::STAR)) { advance(); isPtr2 = true; }
            params.push_back({t2, isPtr2});
        }
    }
    expect(TokenType::RPAREN, "Se esperaba ')' al final de los parametros");

    // Flecha -> tipo
    if (!check(TokenType::ARROW)) {
        throw std::runtime_error(
            "Se esperaba '->' despues de los parametros de funcion externa linea " +
            std::to_string(peek().line));
    }
    advance();
    if (!checkCType()) {
        throw std::runtime_error(
            "Se esperaba tipo de C para retorno en linea " +
            std::to_string(peek().line));
    }
    CType retType = parseCType();
    bool retIsPtr = false;
    if (check(TokenType::STAR)) { advance(); retIsPtr = true; }

    return std::make_unique<ExternDecl>(libName, nameTok.value,
        std::move(params), retType, retIsPtr, ln);
}

// ============================================================
// F-string parser: descompone f"Hola {nombre}, tienes {edad}" en partes
// ============================================================
std::unique_ptr<ASTNode> Parser::parseFString(const std::string& content, int line) {
    auto node = std::make_unique<FStringLit>(line);
    FStringPart current;
    current.isExpr = false;
    
    size_t i = 0;
    while (i < content.size()) {
        char ch = content[i];
        // Detectar { (inicio de expresión) — pero no si está escapada con \{
        if (ch == '{' && (i == 0 || content[i-1] != '\\')) {
            // Guardar la parte literal acumulada hasta ahora
            if (!current.text.empty()) {
                node->parts.push_back(std::move(current));
                current = FStringPart{};
                current.isExpr = false;
            }
            // Buscar el } que cierra la expresión
            i++;  // saltar {
            std::string exprText;
            int depth = 1;  // manejar {} anidados
            while (i < content.size() && depth > 0) {
                if (content[i] == '{' && (i == 0 || content[i-1] != '\\')) depth++;
                else if (content[i] == '}' && (i == 0 || content[i-1] != '\\')) {
                    depth--;
                    if (depth == 0) break;
                }
                exprText += content[i];
                i++;
            }
            if (depth != 0) {
                throw std::runtime_error(
                    "F-string: expresión sin cerrar con } en la linea " + std::to_string(line));
            }
            i++;  // saltar el } final
            
            // Tokenizar y parsear la expresión
            Lexer subLex(exprText);
            auto subResult = subLex.tokenize();
            // Agregar EOF si no lo tiene
            Parser subParser(subResult.tokens);
            auto expr = subParser.parseExpressionPublic();
            
            FStringPart exprPart;
            exprPart.isExpr = true;
            exprPart.expr = std::move(expr);
            node->parts.push_back(std::move(exprPart));
        } else if (ch == '\\' && i + 1 < content.size() && 
                   (content[i+1] == '{' || content[i+1] == '}')) {
            // \{ o \} → agregar el caracter literal
            current.text += content[i+1];
            i += 2;
        } else {
            current.text += ch;
            i++;
        }
    }
    // Guardar la última parte literal
    if (!current.text.empty()) {
        node->parts.push_back(std::move(current));
    }
    
    // Si no hay partes, agregar string vacío
    if (node->parts.empty()) {
        FStringPart empty;
        empty.isExpr = false;
        empty.text = "";
        node->parts.push_back(std::move(empty));
    }
    
    return node;
}
