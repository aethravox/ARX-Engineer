// ==============================================================================
// zen_vm.hpp — Zen Virtual Machine (tree-walking interpreter)
//
// Ejecuta código Zen directamente desde el AST, sin pasar por LLVM.
// Reusa el Lexer y Parser existentes. Esto permite:
//   - Preview en el editor (sin esperar a LLVM compile)
//   - Hot reload (re-parsear y re-ejecutar al guardar)
//   - REPL interactivo
//   - Debugging más fácil
//
// La VM soporta los mismos nodos que el compilador LLVM:
//   - Literales (Number, String, FString, Bool, Null)
//   - Identifiers, BinaryOp, UnaryOp, TernaryOp
//   - FuncCall, AssignStmt, AugAssignStmt
//   - PrintStmt, IfStmt, ForRangeStmt, ForEachStmt, WhileStmt, RepeatStmt
//   - BreakStmt, ContinueStmt, ReturnStmt
//   - FuncDecl, ListLit, IndexAccess, IndexAssign
//   - StructDecl, StructLit, MemberAccess, MemberAssign
//   - MatchStmt
// ==============================================================================
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include "ast.h"  // para ASTNode, FuncDecl, etc.

namespace zen {

// ===== Values =====

// Un valor en la VM puede ser:
// - number (double)
// - string (std::string)
// - bool
// - null
// - list (vector de Values)
// - struct (map de nombre → Value + nombre del tipo)
// - function (puntero a FuncDecl + closure)

struct Value;
using ListValue = std::vector<Value>;
using StructFields = std::unordered_map<std::string, Value>;
struct StructValue {
    std::string type_name;
    std::shared_ptr<StructFields> fields;  // shared_ptr para evitar recursión infinita
    StructValue() : fields(std::make_shared<StructFields>()) {}
};
struct FuncValue {
    FuncDecl* decl = nullptr;
    StructFields* closure = nullptr;  // captura variables
};

struct Value {
    enum class Type { Null, Number, String, Bool, List, Struct, Function } type = Type::Null;
    
    double num = 0.0;
    std::string str;
    bool boolean = false;
    ListValue list;
    StructValue struct_val;
    FuncValue func;
    
    // Constructors
    Value() {}  // Null
    static Value makeNum(double n) { Value v; v.type = Type::Number; v.num = n; return v; }
    static Value makeStr(std::string s) { Value v; v.type = Type::String; v.str = std::move(s); return v; }
    static Value makeBool(bool b) { Value v; v.type = Type::Bool; v.boolean = b; return v; }
    static Value makeNull() { Value v; v.type = Type::Null; return v; }
    static Value makeList(ListValue l) { Value v; v.type = Type::List; v.list = std::move(l); return v; }
    
    // Conversions
    double toNum() const;
    std::string toStr() const;
    bool toBool() const;
    bool isTruthy() const;
    
    // Type name for error messages
    std::string typeName() const;
};

// ===== VM =====

class VM {
public:
    VM();
    ~VM();
    
    // Cargar código desde un string
    bool load_source(const std::string& source, const std::string& filename = "<memory>");
    
    // Cargar código desde un archivo
    bool load_file(const std::string& path);
    
    // Ejecutar el código cargado (corre top-level statements)
    // Retorna el valor del último statement (o null)
    Value run();
    
    // Llamar una función por nombre
    Value call_function(const std::string& name, const std::vector<Value>& args);
    
    // Obtener errores
    const std::vector<std::string>& errors() const { return errors_; }
    bool has_errors() const { return !errors_.empty(); }
    
    // Output capturado (lo que se imprime con muestra/show)
    const std::vector<std::string>& output() const { return output_; }
    void clear_output() { output_.clear(); }
    
    // Hot reload: re-cargar el archivo (re-parse, mantener variables globales)
    bool reload();
    
    // Set/get variable global (para interacción con el editor)
    void set_global(const std::string& name, Value v);
    Value get_global(const std::string& name) const;
    bool has_global(const std::string& name) const;
    
    // Registrar builtin functions (para que el código Zen pueda llamar APIs del motor)
    using BuiltinFn = std::function<Value(const std::vector<Value>&)>;
    void register_builtin(const std::string& name, BuiltinFn fn);
    
private:
    // Ejecutar un statement (retorna via exception para break/continue/return)
    struct BreakSignal {};
    struct ContinueSignal {};
    struct ReturnSignal { Value value; };
    
    void exec_stmt(ASTNode* node);
    Value eval_expr(ASTNode* node);
    
    // Scope management
    struct Scope {
        std::unordered_map<std::string, Value> vars;
    };
    std::vector<Scope> scopes_;
    
    Scope& current_scope() { return scopes_.back(); }
    void push_scope() { scopes_.emplace_back(); }
    void pop_scope() { scopes_.pop_back(); }
    
    Value* find_var(const std::string& name);
    void set_var(const std::string& name, Value v);
    
    // Function table (nombre → FuncDecl*)
    std::unordered_map<std::string, FuncDecl*> functions_;
    
    // Struct definitions (nombre → lista de campos)
    std::unordered_map<std::string, std::vector<std::string>> structs_;
    
    // Builtins
    std::unordered_map<std::string, BuiltinFn> builtins_;
    
    // AST
    std::vector<std::unique_ptr<ASTNode>> ast_;
    std::string loaded_filename_;
    
    // Estado
    std::vector<std::string> errors_;
    std::vector<std::string> output_;
    
    // Builtin: print (muestra/show)
    Value builtin_print(const std::vector<Value>& args);
    Value builtin_longitud(const std::vector<Value>& args);
    Value builtin_contiene(const std::vector<Value>& args);
    Value builtin_quitar(const std::vector<Value>& args);
    Value builtin_numero(const std::vector<Value>& args);
    Value builtin_texto(const std::vector<Value>& args);
    Value builtin_azarr(const std::vector<Value>& args);
    Value builtin_leer_linea(const std::vector<Value>& args);
};

} // namespace zen
