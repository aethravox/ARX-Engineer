// ============================================================
// Zen Programming Language - LLVM Code Generator
// Genera LLVM IR a partir del AST → ejecutable nativo
// v1.2: while, for-each, repeat, break, continue, ternario,
//       asignacion aumentada, plataforma condicional
// Compilacion 100% nativa sin depender de Clang
// ============================================================

#pragma once
#include "ast.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

// Info de una variable en el scope actual
struct VarInfo {
    llvm::Value* alloca = nullptr;
    ZenType type = ZenType::Void;
};

// Info de una funcion Zen
struct FuncInfo {
    llvm::Function* func = nullptr;
    ZenType returnType = ZenType::Number;
    std::vector<ZenType> paramTypes;
};

class CodeGen {
    llvm::LLVMContext context;
    std::unique_ptr<llvm::Module> module;
    llvm::IRBuilder<> builder;

    // Scope de variables
    std::map<std::string, VarInfo> variables;
    // Funciones registradas
    std::map<std::string, FuncInfo> functions;
    // Structs registrados: nombre → lista de nombres de campos
    struct StructInfo {
        std::vector<std::string> fieldNames;
    };
    std::map<std::string, StructInfo> structs;
    // Funciones externas registradas (FFI)
    struct ExternInfo {
        llvm::Function* func = nullptr;
        CType returnType;
        bool returnIsPointer = false;
        std::vector<ExternParam> params;
    };
    std::map<std::string, ExternInfo> externs;
    // Nombres de librerias usadas en declaraciones extern (para pasar al linker)
    std::vector<std::string> requiredLibs;

    // Funcion actual (para ret)
    llvm::Function* currentFunction = nullptr;

    // Target de plataforma (para compilacion condicional)
    std::string targetPlatform;

    // Bloques de control de flujo para break/continue
    // Pila de (breakBB, continueBB) para bucles anidados
    std::vector<std::pair<llvm::BasicBlock*, llvm::BasicBlock*>> loopStack;

    // Funciones externas (printf, pow, etc.)
    llvm::Function* printfFunc = nullptr;
    llvm::Function* powFunc = nullptr;
    llvm::Function* concatFunc = nullptr;
    llvm::Function* numToStrFunc = nullptr;  // runtime: double -> char*
    // Funciones runtime de listas (embebidas en LLVM IR)
    llvm::Function* listCreateFunc = nullptr;     // (i64 capacity) -> zen_list_t*
    llvm::Function* listPushFunc = nullptr;       // (zen_list_t*, i8* value) -> void
    llvm::Function* listGetFunc = nullptr;        // (zen_list_t*, i64 idx) -> i8* (or null)
    llvm::Function* listSetFunc = nullptr;        // (zen_list_t*, i64 idx, i8* value) -> void
    llvm::Function* listLenFunc = nullptr;        // (zen_list_t*) -> i64
    llvm::Function* listContainsFunc = nullptr;   // (zen_list_t*, i8* value) -> i1
    llvm::Function* listPopFunc = nullptr;        // (zen_list_t*) -> i8*
    // Funciones runtime de structs (embebidas en LLVM IR)
    llvm::Function* structCreateFunc = nullptr;   // (i64 numFields) -> i8* (struct opaco)
    llvm::Function* structGetFunc = nullptr;      // (i8*, i64 idx) -> i8*
    llvm::Function* structSetFunc = nullptr;      // (i8*, i64 idx, i8* value) -> void
    // Externs de libc para builtins de strings
    llvm::Function* strcmpFunc = nullptr;
    llvm::Function* strncmpFunc = nullptr;
    llvm::Function* strlenFunc = nullptr;
    llvm::Function* strstrFunc = nullptr;
    llvm::Function* strcpyFunc = nullptr;
    llvm::Function* strncpyFunc = nullptr;
    llvm::Function* mallocFunc = nullptr;
    llvm::Function* memcpyFunc = nullptr;   // para reemplazar() y otros
    llvm::Function* toupperFunc = nullptr;
    llvm::Function* tolowerFunc = nullptr;
    llvm::Function* strtodFunc = nullptr;
    // Externs de libc para I/O archivos y sistema
    llvm::Function* fopenFunc = nullptr;
    llvm::Function* fcloseFunc = nullptr;
    llvm::Function* freadFunc = nullptr;
    llvm::Function* fwriteFunc = nullptr;
    llvm::Function* fseekFunc = nullptr;
    llvm::Function* ftellFunc = nullptr;
    llvm::Function* exitFunc = nullptr;
    // Para leer_linea() desde stdin
    llvm::Function* getlineFunc = nullptr;
    llvm::Function* freeFunc = nullptr;
    // Para azar(n) - numero aleatorio
    llvm::Function* randFunc = nullptr;
    llvm::Function* srandFunc = nullptr;
    bool srandCalled = false;  // true si ya sembramos rand() con time(NULL)
    // Para reloj() - timestamp
    llvm::Function* timeFunc = nullptr;
    // Para dormir(ms)
    llvm::Function* usleepFunc = nullptr;

    // Helpers
    void declareExternals();
    void buildRuntimeConcat();
    void buildRuntimePlatform();  // genera __zen_platform() que retorna un string
    void buildRuntimeNumToStr();  // genera __zen_num_to_str(double) -> char*
    void buildRuntimeList();      // genera funciones runtime de listas
    void buildRuntimeStruct();    // genera funciones runtime de structs
    llvm::Value* toDouble(llvm::Value* val, ZenType type);
    llvm::Value* toBool(llvm::Value* val, ZenType type);
    llvm::Value* toString(llvm::Value* val, ZenType type);  // convierte cualquier valor a i8*
    // Maneja llamadas a funciones builtin (longitud, subtexto, etc.)
    // Retorna {value, type} o lanzar excepcion si no es builtin.
    // Si el nombre no coincide con ningun builtin, retorna {nullptr, Void}.
    std::pair<llvm::Value*, ZenType> tryBuiltinCall(FuncCall* node);
    // Convierte cualquier valor a un "elemento de lista" (i8* box).
    // Los numeros se boxed como string, los strings quedan igual, etc.
    // Esto simplifica el almacenamiento: todo es i8* adentro de la lista.
    llvm::Value* boxValue(llvm::Value* val, ZenType type);
    // Reversa de boxValue: dado un i8*, devuelve el tipo pedido.
    // (por ahora todo se guarda como string; lectura devuelve string)
    llvm::Value* unboxToNumber(llvm::Value* boxed);
    // Generadores de nodos lista
    std::pair<llvm::Value*, ZenType> generateListLit(ListLit* node);
    std::pair<llvm::Value*, ZenType> generateIndexAccess(IndexAccess* node);
    void generateIndexAssign(IndexAssign* node);
    // Generadores de nodos struct
    void registerStructDecl(StructDecl* node);
    std::pair<llvm::Value*, ZenType> generateStructLit(StructLit* node);
    std::pair<llvm::Value*, ZenType> generateMemberAccess(MemberAccess* node);
    void generateMemberAssign(MemberAssign* node);
    // Generador de match/caso
    void generateMatch(MatchStmt* node);
    // FFI
    void registerExternDecl(ExternDecl* node);
    std::pair<llvm::Value*, ZenType> generateExternCall(ExternCall* node);
    llvm::Function* getOrInsertExtern(const std::string& name,
                                      llvm::Type* retType,
                                      std::vector<llvm::Type*> paramTypes,
                                      bool variadic = false);
    void registerFunction(FuncDecl* node);
    void generateFunctionBody(FuncDecl* node);
    void generateNode(ASTNode* node);
    void generateNodeInLoop(ASTNode* node);  // genera nodo + maneja break/continue
    void generatePrint(PrintStmt* node);
    void generateAssign(AssignStmt* node);
    void generateAugAssign(AugAssignStmt* node);
    void generateIf(IfStmt* node);
    void generateForRange(ForRangeStmt* node);
    void generateForEach(ForEachStmt* node);
    void generateWhile(WhileStmt* node);
    void generateRepeat(RepeatStmt* node);
    void generateReturn(ReturnStmt* node);
    void generatePlatformBlock(PlatformBlock* node);
    std::pair<llvm::Value*, ZenType> generateExpr(ASTNode* node);
    std::pair<llvm::Value*, ZenType> generateBinaryOp(BinaryOp* node);
    std::pair<llvm::Value*, ZenType> generateUnaryOp(UnaryOp* node);
    std::pair<llvm::Value*, ZenType> generateFuncCallExpr(FuncCall* node);
    std::pair<llvm::Value*, ZenType> generateTernary(TernaryOp* node);

    std::string typeToString(ZenType t) {
        switch (t) {
            case ZenType::Number: return "numero";
            case ZenType::String: return "texto";
            case ZenType::Bool: return "booleano";
            case ZenType::Null: return "nada";
            default: return "void";
        }
    }

public:
    CodeGen(const std::string& targetPlatform = "");
    void generate(const std::vector<std::unique_ptr<ASTNode>>& ast);
    std::string getIR();
    bool verify();

    // --- Compilacion nativa (sin Clang) ---
    static void initializeTargets();
    bool emitObjectFile(const std::string& outputPath);

    // --- FFI: librerias requeridas ---
    // Devuelve los nombres de librerias usadas en declaraciones extern.
    // Ej: si el usuario hizo extern "raylib" ..., devuelve ["raylib"].
    std::vector<std::string> getRequiredLibs() const;
};
