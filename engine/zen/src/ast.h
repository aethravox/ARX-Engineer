// ============================================================
// Zen Programming Language - AST Node Definitions
// Compilador nativo con LLVM backend
// v1.2: while, for-each, repeat, break, continue, ternario,
//       asignación aumentada, plataforma, #lang
// ============================================================

#pragma once
#include <string>
#include <vector>
#include <memory>

// Tipos de valor que Zen rastrea en compilacion
enum class ZenType {
    Number,  // double en LLVM
    String,  // i8* en LLVM
    Bool,    // i1 en LLVM
    Void,
    Null,
    List,    // i8* en LLVM (puntero a struct zen_list_t)
    Struct,  // i8* en LLVM (puntero a struct zen_struct_t)
};

// Tipos de nodos del AST
enum class NodeType {
    NumberLit,
    StringLit,
    FStringLit,    // f"Hola {nombre}" — string con interpolación
    BoolLit,
    NullLit,
    Identifier,
    BinaryOp,
    UnaryOp,
    TernaryOp,      // cond ? a : b
    FuncCall,
    AssignStmt,
    AugAssignStmt,  // x += 5, x -= 3, etc.
    PrintStmt,
    IfStmt,
    ForRangeStmt,
    ForEachStmt,    // para cada x en lista
    WhileStmt,      // mientras cond
    RepeatStmt,     // repetir N veces
    BreakStmt,      // romper / break
    ContinueStmt,   // continuar / continue
    FuncDecl,
    ReturnStmt,
    PlatformBlock,  // plataforma/windows ... fin_plataforma
    ExprStmt,
    // ---- Listas (Fase 1 - Feature #3) ----
    ListLit,        // [1, 2, 3]
    IndexAccess,    // lista[0]
    IndexAssign,    // lista[0] = valor
    // ---- Structs (Fase 1 - Feature #4) ----
    StructDecl,     // Punto {x, y}  → declara el tipo
    StructLit,      // Punto {x: 10, y: 5}  → crea instancia
    MemberAccess,   // p.x
    MemberAssign,   // p.x = 5
    // ---- Match (Fase 1 - Feature #5) ----
    MatchStmt,      // coincidir expr / caso v / por_defecto
    // ---- FFI (Fase 2) ----
    ExternDecl,     // extern "lib" nombre(args) -> tipo
    ExternCall,     // llamada a funcion externa
};

// Base AST node
struct ASTNode {
    NodeType kind;
    int line = 0;
    virtual ~ASTNode() = default;
};

// Numero: 42, 3.14, -7
struct NumberLit : ASTNode {
    double value;
    NumberLit(double v, int l) { kind = NodeType::NumberLit; value = v; line = l; }
};

// Texto: "hola", 'mundo'
struct StringLit : ASTNode {
    std::string value;
    StringLit(const std::string& v, int l) { kind = NodeType::StringLit; value = v; line = l; }
};

// F-string: f"Hola {nombre}, tienes {edad} años"
// Se descompone en partes:alternando string literal y expresión.
// parts[i].isExpr == false  → string literal (parts[i].text)
// parts[i].isExpr == true   → expresión      (parts[i].expr)
struct FStringPart {
    bool isExpr = false;
    std::string text;                      // si isExpr == false
    std::unique_ptr<ASTNode> expr;         // si isExpr == true
};

struct FStringLit : ASTNode {
    std::vector<FStringPart> parts;
    FStringLit(int l) { kind = NodeType::FStringLit; line = l; }
};

// Booleano: yes/verdad, no/falso
struct BoolLit : ASTNode {
    bool value;
    BoolLit(bool v, int l) { kind = NodeType::BoolLit; value = v; line = l; }
};

// Nulo: nada/nothing
struct NullLit : ASTNode {
    NullLit(int l) { kind = NodeType::NullLit; line = l; }
};

// Identificador de variable
struct Identifier : ASTNode {
    std::string name;
    Identifier(const std::string& n, int l) { kind = NodeType::Identifier; name = n; line = l; }
};

// Operacion binaria: a + b, x > 5, etc.
struct BinaryOp : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    BinaryOp(const std::string& o, std::unique_ptr<ASTNode> l,
             std::unique_ptr<ASTNode> r, int ln) {
        kind = NodeType::BinaryOp;
        op = o;
        left = std::move(l);
        right = std::move(r);
        line = ln;
    }
};

// Operacion unaria: -x, not x
struct UnaryOp : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> operand;
    UnaryOp(const std::string& o, std::unique_ptr<ASTNode> oper, int ln) {
        kind = NodeType::UnaryOp;
        op = o;
        operand = std::move(oper);
        line = ln;
    }
};

// Ternario: cond ? a : b
struct TernaryOp : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenExpr;
    std::unique_ptr<ASTNode> elseExpr;
    TernaryOp(std::unique_ptr<ASTNode> c, std::unique_ptr<ASTNode> t,
              std::unique_ptr<ASTNode> e, int ln) {
        kind = NodeType::TernaryOp;
        condition = std::move(c);
        thenExpr = std::move(t);
        elseExpr = std::move(e);
        line = ln;
    }
};

// Llamada a funcion: suma(3, 4)
struct FuncCall : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> args;
    FuncCall(const std::string& n, std::vector<std::unique_ptr<ASTNode>> a, int l) {
        kind = NodeType::FuncCall;
        name = n;
        args = std::move(a);
        line = l;
    }
};

// Asignacion: x = 5
struct AssignStmt : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    AssignStmt(const std::string& n, std::unique_ptr<ASTNode> v, int l) {
        kind = NodeType::AssignStmt;
        name = n;
        value = std::move(v);
        line = l;
    }
};

// Asignacion aumentada: x += 5, x -= 3, x *= 2, etc.
struct AugAssignStmt : ASTNode {
    std::string name;   // variable
    std::string op;     // +, -, *, /, %, ^
    std::unique_ptr<ASTNode> value;
    AugAssignStmt(const std::string& n, const std::string& o,
                  std::unique_ptr<ASTNode> v, int l) {
        kind = NodeType::AugAssignStmt;
        name = n;
        op = o;
        value = std::move(v);
        line = l;
    }
};

// Imprimir: muestra "hola"
struct PrintStmt : ASTNode {
    std::unique_ptr<ASTNode> value;
    PrintStmt(std::unique_ptr<ASTNode> v, int l) {
        kind = NodeType::PrintStmt;
        value = std::move(v);
        line = l;
    }
};

// Condicional: si ... sino ...
struct IfStmt : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> thenBody;
    std::vector<std::unique_ptr<ASTNode>> elseBody;
    IfStmt(std::unique_ptr<ASTNode> c,
           std::vector<std::unique_ptr<ASTNode>> t,
           std::vector<std::unique_ptr<ASTNode>> e, int l) {
        kind = NodeType::IfStmt;
        condition = std::move(c);
        thenBody = std::move(t);
        elseBody = std::move(e);
        line = l;
    }
};

// Bucle for con rango: para i desde 1 hasta 10
struct ForRangeStmt : ASTNode {
    std::string varName;
    std::unique_ptr<ASTNode> from;
    std::unique_ptr<ASTNode> to;
    std::vector<std::unique_ptr<ASTNode>> body;
    ForRangeStmt(const std::string& v, std::unique_ptr<ASTNode> f,
                 std::unique_ptr<ASTNode> t, std::vector<std::unique_ptr<ASTNode>> b, int l) {
        kind = NodeType::ForRangeStmt;
        varName = v;
        from = std::move(f);
        to = std::move(t);
        body = std::move(b);
        line = l;
    }
};

// Bucle for-each: para cada x en expr
struct ForEachStmt : ASTNode {
    std::string varName;
    std::unique_ptr<ASTNode> iterable;
    std::vector<std::unique_ptr<ASTNode>> body;
    ForEachStmt(const std::string& v, std::unique_ptr<ASTNode> iter,
                std::vector<std::unique_ptr<ASTNode>> b, int l) {
        kind = NodeType::ForEachStmt;
        varName = v;
        iterable = std::move(iter);
        body = std::move(b);
        line = l;
    }
};

// Bucle while: mientras cond
struct WhileStmt : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::vector<std::unique_ptr<ASTNode>> body;
    WhileStmt(std::unique_ptr<ASTNode> c,
               std::vector<std::unique_ptr<ASTNode>> b, int l) {
        kind = NodeType::WhileStmt;
        condition = std::move(c);
        body = std::move(b);
        line = l;
    }
};

// Bucle repeat: repetir 5 veces
struct RepeatStmt : ASTNode {
    std::unique_ptr<ASTNode> count;
    std::vector<std::unique_ptr<ASTNode>> body;
    RepeatStmt(std::unique_ptr<ASTNode> c,
               std::vector<std::unique_ptr<ASTNode>> b, int l) {
        kind = NodeType::RepeatStmt;
        count = std::move(c);
        body = std::move(b);
        line = l;
    }
};

// Romper bucle: romper / break
struct BreakStmt : ASTNode {
    BreakStmt(int l) { kind = NodeType::BreakStmt; line = l; }
};

// Continuar bucle: continuar / continue
struct ContinueStmt : ASTNode {
    ContinueStmt(int l) { kind = NodeType::ContinueStmt; line = l; }
};

// Bloque de plataforma: plataforma windows ... fin_plataforma
struct PlatformBlock : ASTNode {
    std::string platform;  // "windows", "linux", "macos", "android", "web", etc.
    std::vector<std::unique_ptr<ASTNode>> body;
    PlatformBlock(const std::string& p, std::vector<std::unique_ptr<ASTNode>> b, int l) {
        kind = NodeType::PlatformBlock;
        platform = p;
        body = std::move(b);
        line = l;
    }
};

// Declaracion de funcion: nombre(a, b)
struct FuncDecl : ASTNode {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::unique_ptr<ASTNode>> body;
    FuncDecl(const std::string& n, std::vector<std::string> p,
             std::vector<std::unique_ptr<ASTNode>> b, int l) {
        kind = NodeType::FuncDecl;
        name = n;
        params = std::move(p);
        body = std::move(b);
        line = l;
    }
};

// Retorno: retorna valor
struct ReturnStmt : ASTNode {
    std::unique_ptr<ASTNode> value;
    ReturnStmt(std::unique_ptr<ASTNode> v, int l) {
        kind = NodeType::ReturnStmt;
        value = std::move(v);
        line = l;
    }
};

// Expresion como statement (auto-print)
struct ExprStmt : ASTNode {
    std::unique_ptr<ASTNode> expr;
    ExprStmt(std::unique_ptr<ASTNode> e, int l) {
        kind = NodeType::ExprStmt;
        expr = std::move(e);
        line = l;
    }
};

// ---- Listas (Fase 1 - Feature #3) ----

// Literal de lista: [1, 2, 3], ["a", "b"], []
struct ListLit : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> elements;
    ListLit(std::vector<std::unique_ptr<ASTNode>> e, int l) {
        kind = NodeType::ListLit;
        elements = std::move(e);
        line = l;
    }
};

// Acceso por indice: lista[0]
struct IndexAccess : ASTNode {
    std::unique_ptr<ASTNode> base;  // la lista (Identifier u otra IndexAccess)
    std::unique_ptr<ASTNode> index;
    IndexAccess(std::unique_ptr<ASTNode> b, std::unique_ptr<ASTNode> i, int l) {
        kind = NodeType::IndexAccess;
        base = std::move(b);
        index = std::move(i);
        line = l;
    }
};

// Asignacion por indice: lista[0] = valor
struct IndexAssign : ASTNode {
    std::unique_ptr<ASTNode> base;  // la lista
    std::unique_ptr<ASTNode> index;
    std::unique_ptr<ASTNode> value;
    IndexAssign(std::unique_ptr<ASTNode> b, std::unique_ptr<ASTNode> i,
                std::unique_ptr<ASTNode> v, int l) {
        kind = NodeType::IndexAssign;
        base = std::move(b);
        index = std::move(i);
        value = std::move(v);
        line = l;
    }
};

// ---- Structs (Fase 1 - Feature #4) ----

// Declaracion de struct: Punto {x, y}
struct StructDecl : ASTNode {
    std::string name;
    std::vector<std::string> fields;  // nombres de los campos en orden
    StructDecl(const std::string& n, std::vector<std::string> f, int l) {
        kind = NodeType::StructDecl;
        name = n;
        fields = std::move(f);
        line = l;
    }
};

// Literal de struct (crear instancia): Punto {x: 10, y: 5}
struct StructLit : ASTNode {
    std::string structName;
    std::vector<std::string> fieldNames;
    std::vector<std::unique_ptr<ASTNode>> fieldValues;
    StructLit(const std::string& n,
              std::vector<std::string> fn,
              std::vector<std::unique_ptr<ASTNode>> fv, int l) {
        kind = NodeType::StructLit;
        structName = n;
        fieldNames = std::move(fn);
        fieldValues = std::move(fv);
        line = l;
    }
};

// Acceso a miembro: p.x
struct MemberAccess : ASTNode {
    std::unique_ptr<ASTNode> base;  // el struct (Identifier u otro MemberAccess)
    std::string member;
    MemberAccess(std::unique_ptr<ASTNode> b, const std::string& m, int l) {
        kind = NodeType::MemberAccess;
        base = std::move(b);
        member = m;
        line = l;
    }
};

// Asignacion a miembro: p.x = 5
struct MemberAssign : ASTNode {
    std::unique_ptr<ASTNode> base;  // el struct
    std::string member;
    std::unique_ptr<ASTNode> value;
    MemberAssign(std::unique_ptr<ASTNode> b, const std::string& m,
                 std::unique_ptr<ASTNode> v, int l) {
        kind = NodeType::MemberAssign;
        base = std::move(b);
        member = m;
        value = std::move(v);
        line = l;
    }
};

// ---- Match (Fase 1 - Feature #5) ----

// Un caso del match: caso valor <cuerpo>
struct MatchCase {
    std::unique_ptr<ASTNode> value;  // puede ser null si es por_defecto
    std::vector<std::unique_ptr<ASTNode>> body;
    bool isDefault = false;
};

// Match: coincidir expr / caso v1 / caso v2 / por_defecto
struct MatchStmt : ASTNode {
    std::unique_ptr<ASTNode> subject;  // la expr a comparar
    std::vector<MatchCase> cases;
    MatchStmt(std::unique_ptr<ASTNode> s, std::vector<MatchCase> c, int l) {
        kind = NodeType::MatchStmt;
        subject = std::move(s);
        cases = std::move(c);
        line = l;
    }
};

// ---- FFI (Fase 2) ----

// Tipo de C para FFI
enum class CType {
    Void,
    Int,
    UInt,
    Long,
    Float,
    Double,
    Ptr,        // void*
    Str,        // const char*
    Char,       // char
};

// Un parametro de funcion externa: (tipo, es puntero?)
struct ExternParam {
    CType type;
    bool isPointer = false;
};

// Declaracion extern: extern "libm" sqrt(double) -> double
struct ExternDecl : ASTNode {
    std::string libName;        // nombre de la libreria (ej: "libm", "libraylib")
    std::string funcName;       // nombre de la funcion C
    std::vector<ExternParam> params;
    CType returnType;
    bool returnIsPointer = false;
    ExternDecl(const std::string& lib, const std::string& fn,
               std::vector<ExternParam> p, CType rt, bool rip, int l) {
        kind = NodeType::ExternDecl;
        libName = lib;
        funcName = fn;
        params = std::move(p);
        returnType = rt;
        returnIsPointer = rip;
        line = l;
    }
};

// Llamada a funcion externa
struct ExternCall : ASTNode {
    std::string funcName;  // nombre de la funcion externa registrada
    std::vector<std::unique_ptr<ASTNode>> args;
    ExternCall(const std::string& n, std::vector<std::unique_ptr<ASTNode>> a, int l) {
        kind = NodeType::ExternCall;
        funcName = n;
        args = std::move(a);
        line = l;
    }
};
