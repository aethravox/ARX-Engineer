// ==============================================================================
// zen_vm.cpp — Zen Virtual Machine (tree-walking interpreter)
// ==============================================================================
#include "zen_vm.hpp"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <fstream>

namespace zen {

// ===== Value conversions =====

double Value::toNum() const {
    switch (type) {
        case Type::Number: return num;
        case Type::String: {
            try { return std::stod(str); } catch (...) { return 0.0; }
        }
        case Type::Bool: return boolean ? 1.0 : 0.0;
        case Type::Null: return 0.0;
        default: return 0.0;
    }
}

std::string Value::toStr() const {
    switch (type) {
        case Type::Number: {
            // Si es entero, mostrar sin decimales
            if (num == std::floor(num) && std::abs(num) < 1e15) {
                return std::to_string((long long)num);
            }
            std::ostringstream ss;
            ss << num;
            return ss.str();
        }
        case Type::String: return str;
        case Type::Bool: return boolean ? "verdadero" : "falso";
        case Type::Null: return "nada";
        case Type::List: {
            std::string r = "[";
            for (size_t i = 0; i < list.size(); i++) {
                if (i > 0) r += ", ";
                if (list[i].type == Type::String) r += "\"" + list[i].toStr() + "\"";
                else r += list[i].toStr();
            }
            r += "]";
            return r;
        }
        case Type::Struct: {
            std::string r = struct_val.type_name + " {";
            bool first = true;
            for (auto& [k, v] : *struct_val.fields) {
                if (!first) r += ", ";
                first = false;
                r += k + ": ";
                if (v.type == Type::String) r += "\"" + v.toStr() + "\"";
                else r += v.toStr();
            }
            r += "}";
            return r;
        }
        case Type::Function: return "<funcion>";
    }
    return "";
}

bool Value::toBool() const {
    switch (type) {
        case Type::Bool: return boolean;
        case Type::Number: return num != 0.0;
        case Type::String: return !str.empty();
        case Type::Null: return false;
        case Type::List: return !list.empty();
        case Type::Struct: return true;
        case Type::Function: return true;
    }
    return false;
}

bool Value::isTruthy() const { return toBool(); }

std::string Value::typeName() const {
    switch (type) {
        case Type::Null: return "nada";
        case Type::Number: return "numero";
        case Type::String: return "texto";
        case Type::Bool: return "booleano";
        case Type::List: return "lista";
        case Type::Struct: return struct_val.type_name;
        case Type::Function: return "funcion";
    }
    return "desconocido";
}

// ===== VM =====

VM::VM() {
    // Registrar builtins
    register_builtin("muestra", [this](const std::vector<Value>& a) { return builtin_print(a); });
    register_builtin("show",    [this](const std::vector<Value>& a) { return builtin_print(a); });
    register_builtin("print",   [this](const std::vector<Value>& a) { return builtin_print(a); });
    register_builtin("longitud",  [this](const std::vector<Value>& a) { return builtin_longitud(a); });
    register_builtin("length",    [this](const std::vector<Value>& a) { return builtin_longitud(a); });
    register_builtin("contiene",  [this](const std::vector<Value>& a) { return builtin_contiene(a); });
    register_builtin("contains",  [this](const std::vector<Value>& a) { return builtin_contiene(a); });
    register_builtin("quitar",    [this](const std::vector<Value>& a) { return builtin_quitar(a); });
    register_builtin("pop",       [this](const std::vector<Value>& a) { return builtin_quitar(a); });
    register_builtin("numero",    [this](const std::vector<Value>& a) { return builtin_numero(a); });
    register_builtin("number",    [this](const std::vector<Value>& a) { return builtin_numero(a); });
    register_builtin("texto",     [this](const std::vector<Value>& a) { return builtin_texto(a); });
    register_builtin("text",      [this](const std::vector<Value>& a) { return builtin_texto(a); });
    register_builtin("azar",      [](const std::vector<Value>& a) {
        if (a.empty()) return Value::makeNum(0.0);
        int max = (int)a[0].toNum();
        return Value::makeNum((double)(rand() % max));
    });
    register_builtin("random",    [](const std::vector<Value>& a) {
        if (a.empty()) return Value::makeNum((double)rand() / RAND_MAX);
        int max = (int)a[0].toNum();
        return Value::makeNum((double)(rand() % max));
    });
    register_builtin("leer_linea", [this](const std::vector<Value>& a) { return builtin_leer_linea(a); });
    register_builtin("read_line",  [this](const std::vector<Value>& a) { return builtin_leer_linea(a); });
}

VM::~VM() = default;

bool VM::load_source(const std::string& source, const std::string& filename) {
    loaded_filename_ = filename;
    errors_.clear();
    
    try {
        Lexer lex(source);
        auto result = lex.tokenize();
        Parser parser(result.tokens);
        ast_ = parser.parse();
    } catch (const std::exception& e) {
        errors_.push_back(e.what());
        return false;
    }
    return true;
}

bool VM::load_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) {
        errors_.push_back("No se pudo abrir: " + path);
        return false;
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return load_source(ss.str(), path);
}

Value VM::run() {
    if (ast_.empty()) return Value::makeNull();
    
    // Scope global
    scopes_.clear();
    push_scope();
    
    // Primera pasada: registrar funciones y structs
    for (auto& node : ast_) {
        if (node->kind == NodeType::FuncDecl) {
            auto* fd = static_cast<FuncDecl*>(node.get());
            functions_[fd->name] = fd;
        } else if (node->kind == NodeType::StructDecl) {
            auto* sd = static_cast<StructDecl*>(node.get());
            structs_[sd->name] = sd->fields;
        }
    }
    
    // Segunda pasada: ejecutar statements top-level
    Value last = Value::makeNull();
    try {
        for (auto& node : ast_) {
            if (node->kind == NodeType::FuncDecl || node->kind == NodeType::StructDecl) {
                continue;  // ya registrados
            }
            exec_stmt(node.get());
        }
    } catch (const ReturnSignal& r) {
        last = r.value;
    } catch (const std::exception& e) {
        errors_.push_back(e.what());
    }
    
    return last;
}

bool VM::reload() {
    if (loaded_filename_.empty()) return false;
    // Re-cargar archivo, mantener variables globales
    auto saved_globals = std::move(scopes_);
    bool ok = load_file(loaded_filename_);
    if (ok) {
        // Restaurar globals (solo el scope global, no los locales)
        if (!saved_globals.empty()) {
            scopes_.clear();
            push_scope();
            for (auto& [k, v] : saved_globals[0].vars) {
                current_scope().vars[k] = v;
            }
        }
        run();  // re-ejecutar top-level (menos funciones/structs que ya están registrados)
    }
    return ok;
}

Value VM::call_function(const std::string& name, const std::vector<Value>& args) {
    auto it = functions_.find(name);
    if (it == functions_.end()) {
        // Probar builtin
        auto bit = builtins_.find(name);
        if (bit != builtins_.end()) {
            return bit->second(args);
        }
        throw std::runtime_error("Función no definida: " + name);
    }
    
    auto* fd = it->second;
    if (args.size() != fd->params.size()) {
        throw std::runtime_error("Función '" + name + "' espera " + 
            std::to_string(fd->params.size()) + " args, recibió " + std::to_string(args.size()));
    }
    
    push_scope();
    for (size_t i = 0; i < args.size(); i++) {
        current_scope().vars[fd->params[i]] = args[i];
    }
    
    Value result = Value::makeNull();
    try {
        for (auto& stmt : fd->body) {
            exec_stmt(stmt.get());
        }
    } catch (const ReturnSignal& r) {
        result = r.value;
    }
    
    pop_scope();
    return result;
}

void VM::set_global(const std::string& name, Value v) {
    if (scopes_.empty()) push_scope();
    scopes_[0].vars[name] = v;
}

Value VM::get_global(const std::string& name) const {
    if (scopes_.empty()) return Value::makeNull();
    auto it = scopes_[0].vars.find(name);
    if (it == scopes_[0].vars.end()) return Value::makeNull();
    return it->second;
}

bool VM::has_global(const std::string& name) const {
    if (scopes_.empty()) return false;
    return scopes_[0].vars.find(name) != scopes_[0].vars.end();
}

void VM::register_builtin(const std::string& name, BuiltinFn fn) {
    builtins_[name] = fn;
}

Value* VM::find_var(const std::string& name) {
    for (int i = (int)scopes_.size() - 1; i >= 0; i--) {
        auto it = scopes_[i].vars.find(name);
        if (it != scopes_[i].vars.end()) return &it->second;
    }
    return nullptr;
}

void VM::set_var(const std::string& name, Value v) {
    // Si existe en algún scope, actualizar ahí
    for (int i = (int)scopes_.size() - 1; i >= 0; i--) {
        auto it = scopes_[i].vars.find(name);
        if (it != scopes_[i].vars.end()) {
            it->second = v;
            return;
        }
    }
    // Sino, crear en scope actual
    current_scope().vars[name] = v;
}

// ===== Statement execution =====

void VM::exec_stmt(ASTNode* node) {
    if (!node) return;
    
    switch (node->kind) {
        case NodeType::PrintStmt: {
            auto* p = static_cast<PrintStmt*>(node);
            Value v = eval_expr(p->value.get());
            output_.push_back(v.toStr());
            break;
        }
        
        case NodeType::AssignStmt: {
            auto* a = static_cast<AssignStmt*>(node);
            Value v = eval_expr(a->value.get());
            set_var(a->name, v);
            break;
        }
        
        case NodeType::AugAssignStmt: {
            auto* a = static_cast<AugAssignStmt*>(node);
            Value* cur = find_var(a->name);
            Value lhs = cur ? *cur : Value::makeNull();
            Value rhs = eval_expr(a->value.get());
            
            Value result;
            switch (a->op[0]) {
                case '+': result = Value::makeNum(lhs.toNum() + rhs.toNum()); break;
                case '-': result = Value::makeNum(lhs.toNum() - rhs.toNum()); break;
                case '*': result = Value::makeNum(lhs.toNum() * rhs.toNum()); break;
                case '/': result = Value::makeNum(lhs.toNum() / (rhs.toNum() == 0 ? 1 : rhs.toNum())); break;
                case '%': result = Value::makeNum(std::fmod(lhs.toNum(), rhs.toNum())); break;
                case '^': result = Value::makeNum(std::pow(lhs.toNum(), rhs.toNum())); break;
            }
            set_var(a->name, result);
            break;
        }
        
        case NodeType::IfStmt: {
            auto* ifn = static_cast<IfStmt*>(node);
            Value cond = eval_expr(ifn->condition.get());
            if (cond.isTruthy()) {
                push_scope();
                for (auto& s : ifn->thenBody) exec_stmt(s.get());
                pop_scope();
            } else if (!ifn->elseBody.empty()) {
                push_scope();
                for (auto& s : ifn->elseBody) exec_stmt(s.get());
                pop_scope();
            }
            break;
        }
        
        case NodeType::ForRangeStmt: {
            auto* fr = static_cast<ForRangeStmt*>(node);
            double from = eval_expr(fr->from.get()).toNum();
            double to = eval_expr(fr->to.get()).toNum();
            push_scope();
            for (double i = from; i <= to; i++) {
                current_scope().vars[fr->varName] = Value::makeNum(i);
                try {
                    for (auto& s : fr->body) exec_stmt(s.get());
                } catch (const BreakSignal&) { break; }
                  catch (const ContinueSignal&) { continue; }
            }
            pop_scope();
            break;
        }
        
        case NodeType::ForEachStmt: {
            auto* fe = static_cast<ForEachStmt*>(node);
            Value iterable = eval_expr(fe->iterable.get());
            if (iterable.type != Value::Type::List) break;
            push_scope();
            for (auto& item : iterable.list) {
                current_scope().vars[fe->varName] = item;
                try {
                    for (auto& s : fe->body) exec_stmt(s.get());
                } catch (const BreakSignal&) { break; }
                  catch (const ContinueSignal&) { continue; }
            }
            pop_scope();
            break;
        }
        
        case NodeType::WhileStmt: {
            auto* w = static_cast<WhileStmt*>(node);
            push_scope();
            while (eval_expr(w->condition.get()).isTruthy()) {
                try {
                    for (auto& s : w->body) exec_stmt(s.get());
                } catch (const BreakSignal&) { break; }
                  catch (const ContinueSignal&) { continue; }
            }
            pop_scope();
            break;
        }
        
        case NodeType::RepeatStmt: {
            auto* r = static_cast<RepeatStmt*>(node);
            int count = (int)eval_expr(r->count.get()).toNum();
            push_scope();
            for (int i = 0; i < count; i++) {
                try {
                    for (auto& s : r->body) exec_stmt(s.get());
                } catch (const BreakSignal&) { break; }
                  catch (const ContinueSignal&) { continue; }
            }
            pop_scope();
            break;
        }
        
        case NodeType::BreakStmt:    { throw BreakSignal{}; }
        case NodeType::ContinueStmt: { throw ContinueSignal{}; }
        
        case NodeType::ReturnStmt: {
            auto* r = static_cast<ReturnStmt*>(node);
            ReturnSignal sig;
            sig.value = r->value ? eval_expr(r->value.get()) : Value::makeNull();
            throw sig;
        }
        
        case NodeType::FuncDecl: {
            auto* fd = static_cast<FuncDecl*>(node);
            functions_[fd->name] = fd;
            break;
        }
        
        case NodeType::StructDecl: {
            auto* sd = static_cast<StructDecl*>(node);
            structs_[sd->name] = sd->fields;
            break;
        }
        
        case NodeType::ExprStmt: {
            auto* e = static_cast<ExprStmt*>(node);
            eval_expr(e->expr.get());
            break;
        }
        
        case NodeType::IndexAssign: {
            auto* ia = static_cast<IndexAssign*>(node);
            // base es una expresión (ej: Identifier), la evaluamos para obtener la lista
            // Pero necesitamos una referencia para modificarla.
            // Simplificación: si base es un Identifier, buscar la variable.
            if (ia->base->kind == NodeType::Identifier) {
                auto* id = static_cast<Identifier*>(ia->base.get());
                Value* lst = find_var(id->name);
                if (lst && lst->type == Value::Type::List) {
                    int idx = (int)eval_expr(ia->index.get()).toNum();
                    Value val = eval_expr(ia->value.get());
                    if (idx >= 0 && idx < (int)lst->list.size()) {
                        lst->list[idx] = val;
                    }
                }
            }
            break;
        }
        
        case NodeType::MemberAssign: {
            auto* ma = static_cast<MemberAssign*>(node);
            // base es una expresión (ej: Identifier), buscar la variable struct
            if (ma->base->kind == NodeType::Identifier) {
                auto* id = static_cast<Identifier*>(ma->base.get());
                Value* sv = find_var(id->name);
                if (sv && sv->type == Value::Type::Struct) {
                    Value val = eval_expr(ma->value.get());
                    sv->struct_val.fields->operator[](ma->member) = val;
                }
            }
            break;
        }
        
        case NodeType::PlatformBlock: {
            // En la VM, ejecutar todos los platform blocks (no filtramos por plataforma)
            auto* pb = static_cast<PlatformBlock*>(node);
            for (auto& s : pb->body) exec_stmt(s.get());
            break;
        }
        
        case NodeType::MatchStmt: {
            // Simplificado: evaluar cada caso
            auto* m = static_cast<MatchStmt*>(node);
            Value match_val = eval_expr(m->subject.get());
            for (auto& c : m->cases) {
                if (c.isDefault) {
                    for (auto& s : c.body) exec_stmt(s.get());
                    break;
                }
                Value case_val = eval_expr(c.value.get());
                if (match_val.toStr() == case_val.toStr()) {
                    for (auto& s : c.body) exec_stmt(s.get());
                    break;
                }
            }
            break;
        }
        
        default:
            // Otros nodos (ExternDecl, etc.) — no-op en VM
            break;
    }
}

// ===== Expression evaluation =====

Value VM::eval_expr(ASTNode* node) {
    if (!node) return Value::makeNull();
    
    switch (node->kind) {
        case NodeType::NumberLit: {
            auto* n = static_cast<NumberLit*>(node);
            return Value::makeNum(n->value);
        }
        
        case NodeType::StringLit: {
            auto* s = static_cast<StringLit*>(node);
            return Value::makeStr(s->value);
        }
        
        case NodeType::FStringLit: {
            auto* fsl = static_cast<FStringLit*>(node);
            std::string result;
            for (auto& part : fsl->parts) {
                if (part.isExpr) {
                    result += eval_expr(part.expr.get()).toStr();
                } else {
                    result += part.text;
                }
            }
            return Value::makeStr(result);
        }
        
        case NodeType::BoolLit: {
            auto* b = static_cast<BoolLit*>(node);
            return Value::makeBool(b->value);
        }
        
        case NodeType::NullLit: {
            return Value::makeNull();
        }
        
        case NodeType::Identifier: {
            auto* id = static_cast<Identifier*>(node);
            Value* v = find_var(id->name);
            if (v) return *v;
            // Probar si es un builtin sin args
            auto bit = builtins_.find(id->name);
            if (bit != builtins_.end()) {
                return bit->second({});
            }
            // Probar si es "verdadero"/"falso"/"nada"
            if (id->name == "verdadero" || id->name == "yes" || id->name == "true") return Value::makeBool(true);
            if (id->name == "falso" || id->name == "no" || id->name == "false") return Value::makeBool(false);
            if (id->name == "nada" || id->name == "nothing" || id->name == "null") return Value::makeNull();
            throw std::runtime_error("Variable no definida: " + id->name);
        }
        
        case NodeType::BinaryOp: {
            auto* b = static_cast<BinaryOp*>(node);
            Value lhs = eval_expr(b->left.get());
            Value rhs = eval_expr(b->right.get());
            
            // Operadores lógicos (short-circuit)
            if (b->op == "y" || b->op == "and" || b->op == "&&") {
                return Value::makeBool(lhs.isTruthy() && rhs.isTruthy());
            }
            if (b->op == "o" || b->op == "or" || b->op == "||") {
                return Value::makeBool(lhs.isTruthy() || rhs.isTruthy());
            }
            
            // Comparaciones
            if (b->op == "==" || b->op == "es" || b->op == "is") {
                return Value::makeBool(lhs.toStr() == rhs.toStr());
            }
            if (b->op == "!=" || b->op == "no es" || b->op == "is not") {
                return Value::makeBool(lhs.toStr() != rhs.toStr());
            }
            if (b->op == "<")  return Value::makeBool(lhs.toNum() <  rhs.toNum());
            if (b->op == ">")  return Value::makeBool(lhs.toNum() >  rhs.toNum());
            if (b->op == "<=") return Value::makeBool(lhs.toNum() <= rhs.toNum());
            if (b->op == ">=") return Value::makeBool(lhs.toNum() >= rhs.toNum());
            
            // Aritmética
            if (b->op == "+") {
                // Si alguno es string, concatenar
                if (lhs.type == Value::Type::String || rhs.type == Value::Type::String) {
                    return Value::makeStr(lhs.toStr() + rhs.toStr());
                }
                return Value::makeNum(lhs.toNum() + rhs.toNum());
            }
            if (b->op == "-") return Value::makeNum(lhs.toNum() - rhs.toNum());
            if (b->op == "*") return Value::makeNum(lhs.toNum() * rhs.toNum());
            if (b->op == "/") {
                double r = rhs.toNum();
                return Value::makeNum(r == 0 ? 0 : lhs.toNum() / r);
            }
            if (b->op == "%") return Value::makeNum(std::fmod(lhs.toNum(), rhs.toNum()));
            if (b->op == "^") return Value::makeNum(std::pow(lhs.toNum(), rhs.toNum()));
            
            throw std::runtime_error("Operador desconocido: " + b->op);
        }
        
        case NodeType::UnaryOp: {
            auto* u = static_cast<UnaryOp*>(node);
            Value v = eval_expr(u->operand.get());
            if (u->op == "no" || u->op == "not" || u->op == "!") {
                return Value::makeBool(!v.isTruthy());
            }
            if (u->op == "-") return Value::makeNum(-v.toNum());
            return v;
        }
        
        case NodeType::TernaryOp: {
            auto* t = static_cast<TernaryOp*>(node);
            Value cond = eval_expr(t->condition.get());
            return cond.isTruthy() ? eval_expr(t->thenExpr.get()) : eval_expr(t->elseExpr.get());
        }
        
        case NodeType::FuncCall: {
            auto* fc = static_cast<FuncCall*>(node);
            std::vector<Value> args;
            for (auto& a : fc->args) args.push_back(eval_expr(a.get()));
            
            // Probar builtin primero
            auto bit = builtins_.find(fc->name);
            if (bit != builtins_.end()) {
                return bit->second(args);
            }
            
            // Probar función definida por el usuario
            auto fit = functions_.find(fc->name);
            if (fit != functions_.end()) {
                return call_function(fc->name, args);
            }
            
            throw std::runtime_error("Función no definida: " + fc->name);
        }
        
        case NodeType::ListLit: {
            auto* ll = static_cast<ListLit*>(node);
            ListValue items;
            for (auto& e : ll->elements) items.push_back(eval_expr(e.get()));
            return Value::makeList(std::move(items));
        }
        
        case NodeType::IndexAccess: {
            auto* ia = static_cast<IndexAccess*>(node);
            // base es una expresión, la evaluamos
            Value base = eval_expr(ia->base.get());
            if (base.type == Value::Type::List) {
                int idx = (int)eval_expr(ia->index.get()).toNum();
                if (idx >= 0 && idx < (int)base.list.size()) return base.list[idx];
            }
            return Value::makeNull();
        }
        
        case NodeType::MemberAccess: {
            auto* ma = static_cast<MemberAccess*>(node);
            // base es una expresión (Identifier u otro MemberAccess), la evaluamos
            Value base = eval_expr(ma->base.get());
            if (base.type != Value::Type::Struct) return Value::makeNull();
            auto it = base.struct_val.fields->find(ma->member);
            if (it != base.struct_val.fields->end()) return it->second;
            return Value::makeNull();
        }
        
        case NodeType::StructLit: {
            auto* sl = static_cast<StructLit*>(node);
            StructValue sv;
            sv.type_name = sl->structName;
            // fieldNames y fieldValues son vectores paralelos
            for (size_t i = 0; i < sl->fieldNames.size() && i < sl->fieldValues.size(); i++) {
                sv.fields->operator[](sl->fieldNames[i]) = eval_expr(sl->fieldValues[i].get());
            }
            Value v;
            v.type = Value::Type::Struct;
            v.struct_val = std::move(sv);
            return v;
        }
        
        default:
            return Value::makeNull();
    }
}

// ===== Builtins =====

Value VM::builtin_print(const std::vector<Value>& args) {
    if (args.empty()) {
        output_.push_back("");
    } else {
        output_.push_back(args[0].toStr());
    }
    return Value::makeNull();
}

Value VM::builtin_longitud(const std::vector<Value>& args) {
    if (args.empty()) return Value::makeNum(0);
    const auto& a = args[0];
    if (a.type == Value::Type::String) return Value::makeNum((double)a.str.size());
    if (a.type == Value::Type::List)   return Value::makeNum((double)a.list.size());
    return Value::makeNum(0);
}

Value VM::builtin_contiene(const std::vector<Value>& args) {
    if (args.size() < 2) return Value::makeBool(false);
    const auto& lst = args[0];
    const auto& item = args[1];
    if (lst.type == Value::Type::List) {
        for (auto& v : lst.list) {
            if (v.toStr() == item.toStr()) return Value::makeBool(true);
        }
    }
    if (lst.type == Value::Type::String) {
        return Value::makeBool(lst.str.find(item.toStr()) != std::string::npos);
    }
    return Value::makeBool(false);
}

Value VM::builtin_quitar(const std::vector<Value>& args) {
    // pop: quitar(lista) → devuelve y remueve el último
    // Requiere acceso a la variable por referencia, pero args es copia.
    // Simplificación: devolver el último elemento (no modifica la lista).
    if (args.empty() || args[0].type != Value::Type::List) return Value::makeNull();
    if (args[0].list.empty()) return Value::makeNull();
    return args[0].list.back();
}

Value VM::builtin_numero(const std::vector<Value>& args) {
    if (args.empty()) return Value::makeNum(0);
    return Value::makeNum(args[0].toNum());
}

Value VM::builtin_texto(const std::vector<Value>& args) {
    if (args.empty()) return Value::makeStr("");
    return Value::makeStr(args[0].toStr());
}

Value VM::builtin_leer_linea(const std::vector<Value>&) {
    std::string line;
    std::getline(std::cin, line);
    return Value::makeStr(line);
}

} // namespace zen
