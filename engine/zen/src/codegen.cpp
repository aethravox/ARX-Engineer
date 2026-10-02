// ============================================================
// Zen Programming Language - LLVM Code Generator Implementation
// v1.2: while, for-each, repeat, break, continue, ternario,
//       asignacion aumentada, plataforma condicional
// 100% independiente de Clang.
// ============================================================

#include "codegen.h"
#include "zen_gaming_api.hpp"  // Gaming API externs
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <stdexcept>
#include <cstdio>
#include <memory>
#include <algorithm>
#include <cstring>
#include <iostream>

#if LLVM_VERSION_MAJOR >= 16
#include <llvm/TargetParser/HostTriple.h>
#endif

// ============================================================
// Inicializacion de targets LLVM
// ============================================================

// Helper: mapear nombre de plataforma Zen a LLVM target triple
static std::string platformToTriple(const std::string& platform) {
    if (platform == "windows") return "x86_64-pc-windows-gnu";
    if (platform == "linux")   return llvm::sys::getDefaultTargetTriple();
    if (platform == "android") return "aarch64-linux-android24";
    if (platform == "macos")   return "arm64-apple-darwin";
    if (platform == "web")     return "wasm32-unknown-emscripten";
    return llvm::sys::getDefaultTargetTriple();
}

void CodeGen::initializeTargets() {
    // Inicializar TODOS los backends de LLVM para cross-compile
    // (x86, ARM, AArch64, RISCV, WebAssembly, MIPS, etc)
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
}

// ============================================================
// Constructor
// ============================================================

CodeGen::CodeGen(const std::string& target)
    : module(std::make_unique<llvm::Module>("zen", context)),
      builder(context), targetPlatform(target) {
    module->setSourceFileName("zen");
    declareExternals();
    buildRuntimeConcat();
    buildRuntimeNumToStr();
    buildRuntimeList();
    buildRuntimeStruct();
    buildRuntimeDict();
    buildRuntimePlatform();
}

// ============================================================
// Funciones externas del sistema
// ============================================================


void CodeGen::buildRuntimeDict() {
    auto* i8Ptr = llvm::Type::getInt8PtrTy(context);
    auto* i64Ty = llvm::Type::getInt64Ty(context);
    auto* i1Ty = llvm::Type::getInt1Ty(context);
    auto* voidTy = llvm::Type::getVoidTy(context);

    {
        auto* ft = llvm::FunctionType::get(i8Ptr, {}, false);
        dictCreateFunc = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "__zen_dict_create", module.get());
        auto* bb = llvm::BasicBlock::Create(context, "entry", dictCreateFunc);
        builder.SetInsertPoint(bb);
        auto* sz = builder.getInt64(32);
        auto* raw = builder.CreateCall(mallocFunc, {sz}, "raw");
        builder.CreateCall(getOrInsertExtern("memset", i8Ptr, {i8Ptr, llvm::Type::getInt32Ty(context), i64Ty}),
            {raw, builder.getInt32(0), sz}, "memzero");
        builder.CreateRet(raw);
    }
    {
        auto* ft = llvm::FunctionType::get(voidTy, {i8Ptr, i8Ptr, i8Ptr}, false);
        dictSetFunc = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "__zen_dict_set", module.get());
        auto* bb = llvm::BasicBlock::Create(context, "entry", dictSetFunc);
        builder.SetInsertPoint(bb);
        builder.CreateRetVoid();
    }
    {
        auto* ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
        dictGetFunc = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "__zen_dict_get", module.get());
        auto* bb = llvm::BasicBlock::Create(context, "entry", dictGetFunc);
        builder.SetInsertPoint(bb);
        builder.CreateRet(builder.CreateGlobalStringPtr("", "empty"));
    }
    {
        auto* ft = llvm::FunctionType::get(i1Ty, {i8Ptr, i8Ptr}, false);
        dictHasFunc = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "__zen_dict_has", module.get());
        auto* bb = llvm::BasicBlock::Create(context, "entry", dictHasFunc);
        builder.SetInsertPoint(bb);
        builder.CreateRet(builder.getInt1(0));
    }
    {
        auto* ft = llvm::FunctionType::get(i64Ty, {i8Ptr}, false);
        dictSizeFunc = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "__zen_dict_size", module.get());
        auto* bb = llvm::BasicBlock::Create(context, "entry", dictSizeFunc);
        builder.SetInsertPoint(bb);
        builder.CreateRet(builder.getInt64(0));
    }
}

void CodeGen::declareExternals() {
    printfFunc = getOrInsertExtern("printf",
        llvm::Type::getInt32Ty(context),
        {llvm::Type::getInt8PtrTy(context)}, true);

    powFunc = getOrInsertExtern("pow",
        llvm::Type::getDoubleTy(context),
        {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)});

    // Externs de libc para builtins de strings
    auto* i8Ptr = llvm::Type::getInt8PtrTy(context);
    auto* i64Ty = llvm::Type::getInt64Ty(context);
    auto* i32Ty = llvm::Type::getInt32Ty(context);
    auto* voidTy = llvm::Type::getVoidTy(context);

    strcmpFunc  = getOrInsertExtern("strcmp",  i32Ty, {i8Ptr, i8Ptr});
    strlenFunc  = getOrInsertExtern("strlen",  i64Ty, {i8Ptr});
    strstrFunc  = getOrInsertExtern("strstr",  i8Ptr, {i8Ptr, i8Ptr});
    strcpyFunc  = getOrInsertExtern("strcpy",  i8Ptr, {i8Ptr, i8Ptr});
    strncpyFunc = getOrInsertExtern("strncpy", i8Ptr, {i8Ptr, i8Ptr, i64Ty});
    mallocFunc  = getOrInsertExtern("malloc",  i8Ptr, {i64Ty});
    memcpyFunc  = getOrInsertExtern("memcpy",  voidTy, {i8Ptr, i8Ptr, i64Ty});
    toupperFunc = getOrInsertExtern("toupper", i32Ty, {i32Ty});
    tolowerFunc = getOrInsertExtern("tolower", i32Ty, {i32Ty});
    // strtod(const char* str, char** endptr) -> double
    strtodFunc  = getOrInsertExtern("strtod",
        llvm::Type::getDoubleTy(context),
        {i8Ptr, llvm::Type::getInt8PtrTy(context)->getPointerTo()});

    // ---- I/O archivos y sistema ----
    // FILE* fopen(const char* path, const char* mode)
    auto* filePtrTy = llvm::Type::getInt8PtrTy(context);  // usamos i8* como FILE*
    fopenFunc   = getOrInsertExtern("fopen",  filePtrTy, {i8Ptr, i8Ptr});
    // int fclose(FILE* f)
    fcloseFunc  = getOrInsertExtern("fclose", i32Ty, {filePtrTy});
    // size_t fread(void* ptr, size_t size, size_t nmemb, FILE* f)
    freadFunc   = getOrInsertExtern("fread", i64Ty, {filePtrTy, i64Ty, i64Ty, filePtrTy});
    // size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* f)
    fwriteFunc  = getOrInsertExtern("fwrite", i64Ty, {filePtrTy, i64Ty, i64Ty, filePtrTy});
    // int fseek(FILE* f, long offset, int whence)
    fseekFunc   = getOrInsertExtern("fseek", i32Ty, {filePtrTy, i64Ty, i32Ty});
    // long ftell(FILE* f)
    ftellFunc   = getOrInsertExtern("ftell", i64Ty, {filePtrTy});
    // void exit(int status)
    exitFunc    = getOrInsertExtern("exit", llvm::Type::getVoidTy(context), {i32Ty});
    // ssize_t getline(char** lineptr, size_t* n, FILE* stream)
    getlineFunc = getOrInsertExtern("getline", i64Ty,
        {filePtrTy->getPointerTo(), i64Ty->getPointerTo(), filePtrTy});
    // void free(void* ptr)
    freeFunc    = getOrInsertExtern("free", llvm::Type::getVoidTy(context), {filePtrTy});
    // int rand()
    randFunc    = getOrInsertExtern("rand", i32Ty, {});
    // void srand(unsigned int seed)
    srandFunc   = getOrInsertExtern("srand", llvm::Type::getVoidTy(context), {i32Ty});
    // time_t time(time_t* t)
    timeFunc    = getOrInsertExtern("time", i64Ty, {i64Ty->getPointerTo()});
    // int usleep(useconds_t usec)
    usleepFunc  = getOrInsertExtern("usleep", i32Ty, {i32Ty});
}

llvm::Function* CodeGen::getOrInsertExtern(const std::string& name,
                                             llvm::Type* retType,
                                             std::vector<llvm::Type*> paramTypes,
                                             bool variadic) {
    auto* funcType = llvm::FunctionType::get(retType, paramTypes, variadic);
    auto* existing = module->getFunction(name);
    if (existing && existing->getFunctionType() == funcType) return existing;
    return llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, name, module.get());
}

// ============================================================
// Runtime embebido: zen_concat en LLVM IR
// ============================================================

void CodeGen::buildRuntimeConcat() {
    auto* i8Ptr  = llvm::Type::getInt8PtrTy(context);
    auto* i64Ty  = llvm::Type::getInt64Ty(context);

    // strlen y malloc ya están declarados en declareExternals()
    auto* strlenLocal = strlenFunc;
    auto* mallocLocal = mallocFunc;
    auto* memcpyLocal = memcpyFunc;

    auto* funcType = llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false);
    concatFunc = llvm::Function::Create(
        funcType, llvm::Function::InternalLinkage, "zen_concat", module.get());
    concatFunc->setDSOLocal(true);

    auto* a = concatFunc->getArg(0);
    auto* b = concatFunc->getArg(1);
    a->setName("a");
    b->setName("b");

    auto* entry = llvm::BasicBlock::Create(context, "entry", concatFunc);
    auto* failBB = llvm::BasicBlock::Create(context, "alloc.fail", concatFunc);
    auto* okBB   = llvm::BasicBlock::Create(context, "alloc.ok", concatFunc);

    builder.SetInsertPoint(entry);
    auto* la = builder.CreateCall(llvm::FunctionCallee(strlenLocal), {a}, "la");
    auto* lb = builder.CreateCall(llvm::FunctionCallee(strlenLocal), {b}, "lb");
    auto* total = builder.CreateAdd(la, lb, "total");
    auto* size  = builder.CreateAdd(total, llvm::ConstantInt::get(i64Ty, 1), "size");
    auto* result = builder.CreateCall(llvm::FunctionCallee(mallocLocal), {size}, "result");
    auto* isNull = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, result,
        llvm::ConstantPointerNull::get(i8Ptr), "isnull");
    builder.CreateCondBr(isNull, failBB, okBB);

    builder.SetInsertPoint(failBB);
    builder.CreateRet(builder.CreateGlobalStringPtr(""));

    builder.SetInsertPoint(okBB);
    // Ojo: memcpy retorna void, no se le puede poner nombre a la llamada.
    builder.CreateCall(llvm::FunctionCallee(memcpyLocal), {result, a, la});
    // LLVM 14+ requiere Type* como primer argumento del GEP.
    auto* dstPtr = builder.CreateInBoundsGEP(
        llvm::Type::getInt8Ty(context), result, la, "dst");
    auto* copyLen = builder.CreateAdd(lb, llvm::ConstantInt::get(i64Ty, 1), "lb1");
    builder.CreateCall(llvm::FunctionCallee(memcpyLocal), {dstPtr, b, copyLen});
    builder.CreateRet(result);
}

// ============================================================
// Runtime embebido: __zen_num_to_str(double) -> char*
// Convierte un numero a string usando snprintf con "%g".
// Asigna un buffer de 32 bytes (suficiente para cualquier double con %g).
// ============================================================

void CodeGen::buildRuntimeNumToStr() {
    auto* i8Ptr  = llvm::Type::getInt8PtrTy(context);
    auto* i64Ty  = llvm::Type::getInt64Ty(context);
    auto* doubleTy = llvm::Type::getDoubleTy(context);

    // snprintf(char*, size_t, const char*, ...) — variadica
    auto* snprintfFunc = getOrInsertExtern("snprintf",
        llvm::Type::getInt32Ty(context),
        {i8Ptr, i64Ty, i8Ptr}, true);

    auto* mallocFunc = getOrInsertExtern("malloc", i8Ptr, {i64Ty});

    auto* funcType = llvm::FunctionType::get(i8Ptr, {doubleTy}, false);
    numToStrFunc = llvm::Function::Create(
        funcType, llvm::Function::InternalLinkage, "__zen_num_to_str", module.get());
    numToStrFunc->setDSOLocal(true);

    auto* arg = numToStrFunc->getArg(0);
    arg->setName("n");

    auto* entry = llvm::BasicBlock::Create(context, "entry", numToStrFunc);
    builder.SetInsertPoint(entry);

    // Asignar 32 bytes para el buffer (suficiente para %g)
    auto* size = llvm::ConstantInt::get(i64Ty, 32);
    auto* buf = builder.CreateCall(mallocFunc, {size}, "buf");

    // snprintf(buf, 32, "%g", n)
    auto* fmt = builder.CreateGlobalStringPtr("%g");
    builder.CreateCall(llvm::FunctionCallee(snprintfFunc),
        {buf, size, fmt, arg});

    builder.CreateRet(buf);
}

// ============================================================
// Runtime embebido: funciones para listas
// ============================================================
//
// Representacion de una lista en Zen:
//   struct zen_list_t {
//       i8** data;     // puntero al array de elementos (cada uno es i8* boxed)
//       i64  size;     // cantidad de elementos actuales
//       i64  capacity; // capacidad del array
//   };
//
// En LLVM IR lo representamos como { i8**, i64, i64 } y se accede con GEP.
// El "puntero a lista" que circula por Zen es un i8* (opaque) que apunta a esta struct.
//
// Boxeo: todo elemento se guarda como i8*. Los numeros se convierten a string
// con __zen_num_to_str. Los strings quedan igual. Los bool se guardan como "yes"/"no".
// Al leer, el usuario hace numero(lista[i]) si quiere el numero, o lo usa como string.
//
// Esto simplifica mucho el codegen: las listas son siempre i8* con elementos i8*.

llvm::StructType* CodeGen_getListStructType(llvm::LLVMContext& ctx) {
    return llvm::StructType::get(ctx, {
        llvm::Type::getInt8PtrTy(ctx)->getPointerTo(),  // i8** data
        llvm::Type::getInt64Ty(ctx),                    // i64 size
        llvm::Type::getInt64Ty(ctx),                    // i64 capacity
    });
}

void CodeGen::buildRuntimeList() {
    auto* i8Ptr = llvm::Type::getInt8PtrTy(context);
    auto* i64Ty = llvm::Type::getInt64Ty(context);
    auto* i32Ty = llvm::Type::getInt32Ty(context);
    auto* voidTy = llvm::Type::getVoidTy(context);
    auto* i1Ty = llvm::Type::getInt1Ty(context);

    auto* listStructTy = CodeGen_getListStructType(context);
    auto* listPtrTy = listStructTy->getPointerTo();

    // ---- __zen_list_create(i64 capacity) -> i8* (lista opaca) ----
    {
        auto* ft = llvm::FunctionType::get(i8Ptr, {i64Ty}, false);
        listCreateFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_list_create", module.get());
        listCreateFunc->setDSOLocal(true);
        auto* cap = listCreateFunc->getArg(0);
        cap->setName("capacity");

        auto* entry = llvm::BasicBlock::Create(context, "entry", listCreateFunc);
        builder.SetInsertPoint(entry);

        // Asegurar capacidad mínima 4
        auto* minCap = llvm::ConstantInt::get(i64Ty, 4);
        auto* isSmall = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, cap, minCap, "small");
        auto* realCap = builder.CreateSelect(isSmall, minCap, cap, "realcap");

        // malloc(sizeof(zen_list_t))
        auto* structSize = llvm::ConstantExpr::getSizeOf(listStructTy);
        auto* listOpaque = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {structSize}, "list");
        // Bitcast a listStructTy* para poder usar StructGEP
        auto* listPtr = builder.CreateBitCast(listOpaque, listPtrTy, "list.bc");

        // malloc(capacity * sizeof(i8*))
        auto* dataSize = builder.CreateMul(realCap, llvm::ConstantExpr::getSizeOf(i8Ptr), "ds");
        auto* dataPtrOpaque = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {dataSize}, "data");
        // dataPtr es i8* de malloc, pero el campo data de la struct es i8**
        // Hacemos bitcast para que coincida
        auto* dataPtr = builder.CreateBitCast(dataPtrOpaque, i8Ptr->getPointerTo(), "data.bc");

        // list->data = dataPtr
        auto* dataField = builder.CreateStructGEP(listStructTy, listPtr, 0, "data.f");
        builder.CreateStore(dataPtr, dataField);
        // list->size = 0
        auto* sizeField = builder.CreateStructGEP(listStructTy, listPtr, 1, "size.f");
        builder.CreateStore(llvm::ConstantInt::get(i64Ty, 0), sizeField);
        // list->capacity = realCap
        auto* capField = builder.CreateStructGEP(listStructTy, listPtr, 2, "cap.f");
        builder.CreateStore(realCap, capField);

        // Retornar como i8*
        builder.CreateRet(listOpaque);
    }

    // ---- __zen_list_push(i8* listOpaque, i8* value) -> void ----
    // Hace crecer la lista si es necesario (duplica capacidad).
    {
        auto* ft = llvm::FunctionType::get(voidTy, {i8Ptr, i8Ptr}, false);
        listPushFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_list_push", module.get());
        listPushFunc->setDSOLocal(true);
        auto* listOpaque = listPushFunc->getArg(0);
        auto* value = listPushFunc->getArg(1);
        listOpaque->setName("list");
        value->setName("value");

        auto* entry = llvm::BasicBlock::Create(context, "entry", listPushFunc);
        builder.SetInsertPoint(entry);

        auto* listPtr = builder.CreateBitCast(listOpaque, listPtrTy, "list");
        auto* dataField = builder.CreateStructGEP(listStructTy, listPtr, 0, "data.f");
        auto* sizeField = builder.CreateStructGEP(listStructTy, listPtr, 1, "size.f");
        auto* capField   = builder.CreateStructGEP(listStructTy, listPtr, 2, "cap.f");

        auto* curSize = builder.CreateLoad(i64Ty, sizeField, "size");
        auto* curCap  = builder.CreateLoad(i64Ty, capField, "cap");

        // if (size == cap) need_grow
        auto* needGrow = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, curSize, curCap, "grow");
        auto* growBB = llvm::BasicBlock::Create(context, "grow", listPushFunc);
        auto* contBB = llvm::BasicBlock::Create(context, "cont", listPushFunc);
        builder.CreateCondBr(needGrow, growBB, contBB);

        // grow: newCap = cap * 2; newData = malloc(newCap * 8); memcpy; free(old); update
        builder.SetInsertPoint(growBB);
        auto* newCap = builder.CreateMul(curCap, llvm::ConstantInt::get(i64Ty, 2), "newcap");
        auto* newDataSize = builder.CreateMul(newCap, llvm::ConstantExpr::getSizeOf(i8Ptr), "nds");
        auto* newDataOpaque = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {newDataSize}, "newdata");
        auto* newData = builder.CreateBitCast(newDataOpaque, i8Ptr->getPointerTo(), "newdata.bc");
        auto* oldData = builder.CreateLoad(i8Ptr->getPointerTo(), dataField, "olddata");
        auto* copySize = builder.CreateMul(curCap, llvm::ConstantExpr::getSizeOf(i8Ptr), "cs");
        // memcpy espera i8*, i8*, i64. Hay que bitcastear los i8** a i8*
        auto* newDataAsI8 = builder.CreateBitCast(newData, i8Ptr, "ndi8");
        auto* oldDataAsI8 = builder.CreateBitCast(oldData, i8Ptr, "odi8");
        builder.CreateCall(llvm::FunctionCallee(memcpyFunc), {newDataAsI8, oldDataAsI8, copySize});
        builder.CreateCall(llvm::FunctionCallee(freeFunc), {oldDataAsI8});
        builder.CreateStore(newData, dataField);
        builder.CreateStore(newCap, capField);
        builder.CreateBr(contBB);

        // cont: data[size] = value; size++
        builder.SetInsertPoint(contBB);
        auto* data2 = builder.CreateLoad(i8Ptr->getPointerTo(), dataField, "data2");
        auto* slot = builder.CreateInBoundsGEP(i8Ptr, data2, curSize, "slot");
        builder.CreateStore(value, slot);
        auto* newSize = builder.CreateAdd(curSize, llvm::ConstantInt::get(i64Ty, 1), "newsize");
        builder.CreateStore(newSize, sizeField);
        builder.CreateRetVoid();
    }

    // ---- __zen_list_get(i8* listOpaque, i64 idx) -> i8* ----
    // Si idx fuera de rango, devuelve nullptr.
    {
        auto* ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i64Ty}, false);
        listGetFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_list_get", module.get());
        listGetFunc->setDSOLocal(true);
        auto* listOpaque = listGetFunc->getArg(0);
        auto* idx = listGetFunc->getArg(1);
        listOpaque->setName("list");
        idx->setName("idx");

        auto* entry = llvm::BasicBlock::Create(context, "entry", listGetFunc);
        builder.SetInsertPoint(entry);

        auto* listPtr = builder.CreateBitCast(listOpaque, listPtrTy, "list");
        auto* sizeField = builder.CreateStructGEP(listStructTy, listPtr, 1, "size.f");
        auto* size = builder.CreateLoad(i64Ty, sizeField, "size");
        auto* inRange = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, idx, size, "inrange");
        auto* isNeg = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, idx, llvm::ConstantInt::get(i64Ty, 0), "neg");
        auto* valid = builder.CreateAnd(inRange, builder.CreateNot(isNeg), "valid");
        auto* okBB = llvm::BasicBlock::Create(context, "ok", listGetFunc);
        auto* endBB = llvm::BasicBlock::Create(context, "end", listGetFunc);
        builder.CreateCondBr(valid, okBB, endBB);

        builder.SetInsertPoint(okBB);
        auto* dataField = builder.CreateStructGEP(listStructTy, listPtr, 0, "data.f");
        auto* data = builder.CreateLoad(i8Ptr->getPointerTo(), dataField, "data");
        auto* slot = builder.CreateInBoundsGEP(i8Ptr, data, idx, "slot");
        auto* value = builder.CreateLoad(i8Ptr, slot, "value");
        builder.CreateBr(endBB);

        builder.SetInsertPoint(endBB);
        auto* phi = builder.CreatePHI(i8Ptr, 2, "result");
        phi->addIncoming(llvm::ConstantPointerNull::get(i8Ptr), entry);
        phi->addIncoming(value, okBB);
        builder.CreateRet(phi);
    }

    // ---- __zen_list_set(i8* listOpaque, i64 idx, i8* value) -> void ----
    // Si idx fuera de rango, no hace nada (silencioso).
    {
        auto* ft = llvm::FunctionType::get(voidTy, {i8Ptr, i64Ty, i8Ptr}, false);
        listSetFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_list_set", module.get());
        listSetFunc->setDSOLocal(true);
        auto* listOpaque = listSetFunc->getArg(0);
        auto* idx = listSetFunc->getArg(1);
        auto* value = listSetFunc->getArg(2);
        listOpaque->setName("list");
        idx->setName("idx");
        value->setName("value");

        auto* entry = llvm::BasicBlock::Create(context, "entry", listSetFunc);
        builder.SetInsertPoint(entry);

        auto* listPtr = builder.CreateBitCast(listOpaque, listPtrTy, "list");
        auto* sizeField = builder.CreateStructGEP(listStructTy, listPtr, 1, "size.f");
        auto* size = builder.CreateLoad(i64Ty, sizeField, "size");
        auto* inRange = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, idx, size, "inrange");
        auto* isNeg = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, idx, llvm::ConstantInt::get(i64Ty, 0), "neg");
        auto* valid = builder.CreateAnd(inRange, builder.CreateNot(isNeg), "valid");
        auto* okBB = llvm::BasicBlock::Create(context, "ok", listSetFunc);
        auto* endBB = llvm::BasicBlock::Create(context, "end", listSetFunc);
        builder.CreateCondBr(valid, okBB, endBB);

        builder.SetInsertPoint(okBB);
        auto* dataField = builder.CreateStructGEP(listStructTy, listPtr, 0, "data.f");
        auto* data = builder.CreateLoad(i8Ptr->getPointerTo(), dataField, "data");
        auto* slot = builder.CreateInBoundsGEP(i8Ptr, data, idx, "slot");
        builder.CreateStore(value, slot);
        builder.CreateBr(endBB);

        builder.SetInsertPoint(endBB);
        builder.CreateRetVoid();
    }

    // ---- __zen_list_len(i8* listOpaque) -> i64 ----
    {
        auto* ft = llvm::FunctionType::get(i64Ty, {i8Ptr}, false);
        listLenFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_list_len", module.get());
        listLenFunc->setDSOLocal(true);
        auto* listOpaque = listLenFunc->getArg(0);
        listOpaque->setName("list");

        auto* entry = llvm::BasicBlock::Create(context, "entry", listLenFunc);
        builder.SetInsertPoint(entry);

        auto* listPtr = builder.CreateBitCast(listOpaque, listPtrTy, "list");
        auto* sizeField = builder.CreateStructGEP(listStructTy, listPtr, 1, "size.f");
        auto* size = builder.CreateLoad(i64Ty, sizeField, "size");
        builder.CreateRet(size);
    }

    // ---- __zen_list_contains(i8* listOpaque, i8* value) -> i1 ----
    // Compara cada elemento con strcmp.
    {
        auto* ft = llvm::FunctionType::get(i1Ty, {i8Ptr, i8Ptr}, false);
        listContainsFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_list_contains", module.get());
        listContainsFunc->setDSOLocal(true);
        auto* listOpaque = listContainsFunc->getArg(0);
        auto* value = listContainsFunc->getArg(1);
        listOpaque->setName("list");
        value->setName("value");

        auto* entry = llvm::BasicBlock::Create(context, "entry", listContainsFunc);
        auto* condBB = llvm::BasicBlock::Create(context, "cond", listContainsFunc);
        auto* bodyBB = llvm::BasicBlock::Create(context, "body", listContainsFunc);
        auto* contBB = llvm::BasicBlock::Create(context, "cont", listContainsFunc);
        auto* yesBB  = llvm::BasicBlock::Create(context, "yes", listContainsFunc);
        auto* noBB   = llvm::BasicBlock::Create(context, "no", listContainsFunc);

        builder.SetInsertPoint(entry);
        auto* listPtr = builder.CreateBitCast(listOpaque, listPtrTy, "list");
        auto* sizeField = builder.CreateStructGEP(listStructTy, listPtr, 1, "size.f");
        auto* dataField = builder.CreateStructGEP(listStructTy, listPtr, 0, "data.f");
        auto* size = builder.CreateLoad(i64Ty, sizeField, "size");
        auto* data = builder.CreateLoad(i8Ptr->getPointerTo(), dataField, "data");
        auto* iAlloca = builder.CreateAlloca(i64Ty, nullptr, "i");
        builder.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iAlloca);
        builder.CreateBr(condBB);

        builder.SetInsertPoint(condBB);
        auto* i = builder.CreateLoad(i64Ty, iAlloca, "i");
        auto* cmp = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, i, size, "cmp");
        builder.CreateCondBr(cmp, bodyBB, noBB);

        builder.SetInsertPoint(bodyBB);
        auto* slot = builder.CreateInBoundsGEP(i8Ptr, data, i, "slot");
        auto* elem = builder.CreateLoad(i8Ptr, slot, "elem");
        auto* cmpRes = builder.CreateCall(llvm::FunctionCallee(strcmpFunc), {elem, value}, "scmp");
        auto* isEq = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, cmpRes,
            llvm::ConstantInt::get(i32Ty, 0), "eq");
        builder.CreateCondBr(isEq, yesBB, contBB);

        builder.SetInsertPoint(contBB);
        auto* nextI = builder.CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1), "nexti");
        builder.CreateStore(nextI, iAlloca);
        builder.CreateBr(condBB);

        builder.SetInsertPoint(yesBB);
        builder.CreateRet(llvm::ConstantInt::getTrue(context));

        builder.SetInsertPoint(noBB);
        builder.CreateRet(llvm::ConstantInt::getFalse(context));
    }

    // ---- __zen_list_pop(i8* listOpaque) -> i8* ----
    // Quita el ultimo elemento y lo devuelve. Si vacio, devuelve nullptr.
    {
        auto* ft = llvm::FunctionType::get(i8Ptr, {i8Ptr}, false);
        listPopFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_list_pop", module.get());
        listPopFunc->setDSOLocal(true);
        auto* listOpaque = listPopFunc->getArg(0);
        listOpaque->setName("list");

        auto* entry = llvm::BasicBlock::Create(context, "entry", listPopFunc);
        builder.SetInsertPoint(entry);

        auto* listPtr = builder.CreateBitCast(listOpaque, listPtrTy, "list");
        auto* sizeField = builder.CreateStructGEP(listStructTy, listPtr, 1, "size.f");
        auto* dataField = builder.CreateStructGEP(listStructTy, listPtr, 0, "data.f");
        auto* size = builder.CreateLoad(i64Ty, sizeField, "size");
        auto* isEmpty = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, size, llvm::ConstantInt::get(i64Ty, 0), "empty");
        auto* okBB = llvm::BasicBlock::Create(context, "ok", listPopFunc);
        auto* endBB = llvm::BasicBlock::Create(context, "end", listPopFunc);
        builder.CreateCondBr(isEmpty, endBB, okBB);

        builder.SetInsertPoint(okBB);
        auto* newSize = builder.CreateSub(size, llvm::ConstantInt::get(i64Ty, 1), "newsize");
        auto* data = builder.CreateLoad(i8Ptr->getPointerTo(), dataField, "data");
        auto* slot = builder.CreateInBoundsGEP(i8Ptr, data, newSize, "slot");
        auto* value = builder.CreateLoad(i8Ptr, slot, "value");
        builder.CreateStore(newSize, sizeField);
        builder.CreateBr(endBB);

        builder.SetInsertPoint(endBB);
        auto* phi = builder.CreatePHI(i8Ptr, 2, "result");
        phi->addIncoming(llvm::ConstantPointerNull::get(i8Ptr), entry);
        phi->addIncoming(value, okBB);
        builder.CreateRet(phi);
    }
}

// ============================================================
// Runtime embebido: funciones para structs
// ============================================================
//
// Representacion de un struct en Zen:
//   Es simplemente un array de i8* (campos), donde cada i8* es un valor boxed.
//   No guardamos el tipo/nombre del struct en runtime — todo se resuelve en compile time.
//
//   struct zen_struct_t {
//       i64 numFields;   // cantidad de campos
//       i8* data[];      // array flexible de campos (cada uno i8*)
//   };
//
// El "puntero a struct" que circula por Zen es un i8* (opaque) que apunta a esta struct.

void CodeGen::buildRuntimeStruct() {
    auto* i8Ptr = llvm::Type::getInt8PtrTy(context);
    auto* i64Ty = llvm::Type::getInt64Ty(context);
    auto* voidTy = llvm::Type::getVoidTy(context);

    // El struct: {i64 numFields, i8* data[0]}  (flexible array member)
    // Pero LLVM no soporta array[0] facil. Usamos: {i64 numFields, i8** data}
    // y hacemos un malloc de sizeof(i64) + numFields * sizeof(i8*)
    auto* structHdrTy = llvm::StructType::get(context, {
        i64Ty,                   // numFields
        i8Ptr->getPointerTo(),   // data (i8**)
    });

    // ---- __zen_struct_create(i64 numFields) -> i8* ----
    {
        auto* ft = llvm::FunctionType::get(i8Ptr, {i64Ty}, false);
        structCreateFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_struct_create", module.get());
        structCreateFunc->setDSOLocal(true);
        auto* numFields = structCreateFunc->getArg(0);
        numFields->setName("numFields");

        auto* entry = llvm::BasicBlock::Create(context, "entry", structCreateFunc);
        builder.SetInsertPoint(entry);

        // malloc(sizeof(i64) + numFields * sizeof(i8*))
        auto* hdrSize = llvm::ConstantExpr::getSizeOf(structHdrTy);
        auto* dataSize = builder.CreateMul(numFields, llvm::ConstantExpr::getSizeOf(i8Ptr), "ds");
        auto* totalSize = builder.CreateAdd(hdrSize, dataSize, "ts");
        auto* memOpaque = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {totalSize}, "mem");
        auto* mem = builder.CreateBitCast(memOpaque, structHdrTy->getPointerTo(), "mem.bc");

        // struct->numFields = numFields
        auto* nfField = builder.CreateStructGEP(structHdrTy, mem, 0, "nf.f");
        builder.CreateStore(numFields, nfField);

        // struct->data = (i8**)(mem + sizeof(i64))  → puntero al inicio de data
        // Calculamos: data_ptr = bitcast (mem + 1 elemento) a i8**
        // En LLVM, podemos hacer StructGEP al campo data (que apunta a i8**)
        // Pero queremos que apunte al CONTINUO del header. Usamos:
        //   data_ptr = bitcast((i8*)mem + sizeof(i64)) a i8**
        auto* memAsI8 = builder.CreateBitCast(mem, i8Ptr, "mem.i8");
        auto* dataPtrOpaque = builder.CreateInBoundsGEP(
            llvm::Type::getInt8Ty(context), memAsI8, hdrSize, "dp.o");
        auto* dataPtr = builder.CreateBitCast(dataPtrOpaque, i8Ptr->getPointerTo(), "dp.bc");

        // Guardar dataPtr en el campo data
        auto* dataField = builder.CreateStructGEP(structHdrTy, mem, 1, "data.f");
        builder.CreateStore(dataPtr, dataField);

        // Inicializar todos los campos a null
        auto* iAlloca = builder.CreateAlloca(i64Ty, nullptr, "i");
        builder.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iAlloca);

        auto* condBB = llvm::BasicBlock::Create(context, "sc.cond", structCreateFunc);
        auto* bodyBB = llvm::BasicBlock::Create(context, "sc.body", structCreateFunc);
        auto* endBB  = llvm::BasicBlock::Create(context, "sc.end", structCreateFunc);
        builder.CreateBr(condBB);

        builder.SetInsertPoint(condBB);
        auto* i = builder.CreateLoad(i64Ty, iAlloca, "i");
        auto* cmp = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, i, numFields, "cmp");
        builder.CreateCondBr(cmp, bodyBB, endBB);

        builder.SetInsertPoint(bodyBB);
        auto* slot = builder.CreateInBoundsGEP(i8Ptr, dataPtr, i, "slot");
        builder.CreateStore(llvm::ConstantPointerNull::get(i8Ptr), slot);
        auto* next = builder.CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1), "next");
        builder.CreateStore(next, iAlloca);
        builder.CreateBr(condBB);

        builder.SetInsertPoint(endBB);
        builder.CreateRet(memOpaque);
    }

    // ---- __zen_struct_get(i8* structOpaque, i64 idx) -> i8* ----
    {
        auto* ft = llvm::FunctionType::get(i8Ptr, {i8Ptr, i64Ty}, false);
        structGetFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_struct_get", module.get());
        structGetFunc->setDSOLocal(true);
        auto* structOpaque = structGetFunc->getArg(0);
        auto* idx = structGetFunc->getArg(1);
        structOpaque->setName("s");
        idx->setName("idx");

        auto* entry = llvm::BasicBlock::Create(context, "entry", structGetFunc);
        builder.SetInsertPoint(entry);

        // Bitcast a header*
        auto* hdr = builder.CreateBitCast(structOpaque, structHdrTy->getPointerTo(), "hdr");
        // data = hdr->data
        auto* dataField = builder.CreateStructGEP(structHdrTy, hdr, 1, "data.f");
        auto* data = builder.CreateLoad(i8Ptr->getPointerTo(), dataField, "data");
        // return data[idx]
        auto* slot = builder.CreateInBoundsGEP(i8Ptr, data, idx, "slot");
        auto* value = builder.CreateLoad(i8Ptr, slot, "value");
        builder.CreateRet(value);
    }

    // ---- __zen_struct_set(i8* structOpaque, i64 idx, i8* value) -> void ----
    {
        auto* ft = llvm::FunctionType::get(voidTy, {i8Ptr, i64Ty, i8Ptr}, false);
        structSetFunc = llvm::Function::Create(
            ft, llvm::Function::InternalLinkage, "__zen_struct_set", module.get());
        structSetFunc->setDSOLocal(true);
        auto* structOpaque = structSetFunc->getArg(0);
        auto* idx = structSetFunc->getArg(1);
        auto* value = structSetFunc->getArg(2);
        structOpaque->setName("s");
        idx->setName("idx");
        value->setName("value");

        auto* entry = llvm::BasicBlock::Create(context, "entry", structSetFunc);
        builder.SetInsertPoint(entry);

        auto* hdr = builder.CreateBitCast(structOpaque, structHdrTy->getPointerTo(), "hdr");
        auto* dataField = builder.CreateStructGEP(structHdrTy, hdr, 1, "data.f");
        auto* data = builder.CreateLoad(i8Ptr->getPointerTo(), dataField, "data");
        auto* slot = builder.CreateInBoundsGEP(i8Ptr, data, idx, "slot");
        builder.CreateStore(value, slot);
        builder.CreateRetVoid();
    }
}

// ============================================================
// Runtime: __zen_platform() — detecta la plataforma en runtime
// Retorna un puntero a string estatico: "windows", "linux", "macos", "android"
// ============================================================

void CodeGen::buildRuntimePlatform() {
    auto* i8Ptr = llvm::Type::getInt8PtrTy(context);
    auto* funcType = llvm::FunctionType::get(i8Ptr, false);
    auto* func = llvm::Function::Create(
        funcType, llvm::Function::InternalLinkage, "__zen_platform", module.get());
    func->setDSOLocal(true);

    auto* entry = llvm::BasicBlock::Create(context, "entry", func);

    // Por ahora retorna el target de compilacion como string constante
    // En el futuro se puede hacer deteccion real con #ifdef
    builder.SetInsertPoint(entry);
    llvm::Value* platformStr = builder.CreateGlobalStringPtr(targetPlatform.empty() ? "unknown" : targetPlatform);
    builder.CreateRet(platformStr);
}

// ============================================================
// Emision de codigo objeto nativo
// ============================================================

bool CodeGen::emitObjectFile(const std::string& outputPath) {
    // Usar el triple de la plataforma target (para cross-compile)
    std::string triple = platformToTriple(targetPlatform);
    std::string error;

    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(triple, error);
    if (!target) {
        std::cerr << "Error: target no encontrado: " << error << "\n";
        return false;
    }

    llvm::TargetOptions opt;
    // Windows usa Static (no PIC), Android/Linux usan PIC
    llvm::Reloc::Model relocModel = llvm::Reloc::PIC_;
    if (triple.find("windows") != std::string::npos) {
        relocModel = llvm::Reloc::PIC_;
    }
    auto* machine = target->createTargetMachine(triple, "generic", "", opt, relocModel);
    if (!machine) {
        std::cerr << "Error: no se pudo crear la TargetMachine.\n";
        return false;
    }

    module->setTargetTriple(triple);
    module->setDataLayout(machine->createDataLayout());

    std::error_code ec;
    llvm::raw_fd_ostream out(outputPath, ec, llvm::sys::fs::OF_None);
    if (ec) {
        std::cerr << "Error al crear archivo: " << outputPath << ": " << ec.message() << "\n";
        return false;
    }

    llvm::legacy::PassManager pm;
    // En LLVM 14, CodeGenFileType es un enum sin scope (CGFT_ObjectFile).
    // En LLVM 18+ se hizo enum class (CodeGenFileType::ObjectFile).
#if LLVM_VERSION_MAJOR >= 18
    auto fileType = llvm::CodeGenFileType::ObjectFile;
#else
    auto fileType = llvm::CGFT_ObjectFile;
#endif
    if (machine->addPassesToEmitFile(pm, out, nullptr, fileType)) {
        std::cerr << "Error: TargetMachine no soporta generacion de codigo objeto.\n";
        return false;
    }

    pm.run(*module);
    out.flush();
    delete machine;
    return true;
}

// ============================================================
// Generacion principal
// ============================================================

void CodeGen::generate(const std::vector<std::unique_ptr<ASTNode>>& ast) {
    // PASS 0: Registrar todas las declaraciones de structs y externs
    for (auto& node : ast) {
        if (node->kind == NodeType::StructDecl) {
            registerStructDecl(static_cast<StructDecl*>(node.get()));
        } else if (node->kind == NodeType::ExternDecl) {
            registerExternDecl(static_cast<ExternDecl*>(node.get()));
        }
    }

    // PASS 1: Registrar todas las funciones
    for (auto& node : ast) {
        if (node->kind == NodeType::FuncDecl) {
            registerFunction(static_cast<FuncDecl*>(node.get()));
        }
    }

    // PASS 2.5: Crear main() y pre-escanear variables globales
    // Esto permite que las funciones (generadas en PASS 2) vean las variables
    // globales definidas en el nivel superior.
    auto* mainType = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context), false);
    auto* mainFunc = llvm::Function::Create(
        mainType, llvm::Function::ExternalLinkage, "main", module.get());
    auto* entry = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(entry);
    currentFunction = mainFunc;
    // Pre-escanear: crear GlobalVariables para variables globales
    // SIEMPRE usar i8* (boxed) para que funcione con cualquier tipo
    for (auto& node : ast) {
        if (node->kind == NodeType::AssignStmt) {
            auto* assign = static_cast<AssignStmt*>(node.get());
            if (variables.find(assign->name) == variables.end()) {
                auto* gty = llvm::Type::getInt8PtrTy(context);
                auto* gv = new llvm::GlobalVariable(
                    *module, gty, false, llvm::GlobalValue::InternalLinkage,
                    llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context)),
                    "g_" + assign->name);
                variables[assign->name] = {gv, ZenType::Null};
            }
        }
    }


    // PASS 2: Generar cuerpos de funciones
    for (auto& node : ast) {
        if (node->kind == NodeType::FuncDecl) {
            generateFunctionBody(static_cast<FuncDecl*>(node.get()));
        }
    }

    // PASS 3: Generar el resto del codigo de nivel superior
    builder.SetInsertPoint(entry);
    currentFunction = mainFunc;
    // Las variables ya fueron pre-escaneadas, asi que generateAssign
    // encontrara las variables existentes y solo hara CreateStore.
    for (auto& node : ast) {
        if (node->kind != NodeType::FuncDecl && node->kind != NodeType::StructDecl
            && node->kind != NodeType::ExternDecl) {
            generateNode(node.get());
        }
    }

    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));
    }
}

void CodeGen::registerFunction(FuncDecl* node) {
    // Funciones usan i8* (boxed) para params y return - como Lua
    auto* i8ptr = llvm::Type::getInt8PtrTy(context);
    std::vector<llvm::Type*> paramTypes(node->params.size(), i8ptr);
    auto* funcType = llvm::FunctionType::get(i8ptr, paramTypes, false);

    auto* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, node->name, module.get());

    unsigned idx = 0;
    for (auto& arg : func->args()) {
        arg.setName(node->params[idx++]);
    }

    std::vector<ZenType> ptypes(node->params.size(), ZenType::String);
    functions[node->name] = {func, ZenType::String, ptypes};
}

void CodeGen::generateFunctionBody(FuncDecl* node) {
    auto* func = functions[node->name].func;
    auto* entry = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(entry);
    currentFunction = func;

    auto savedVars = variables;

    for (auto& arg : func->args()) {
        auto* alloca = builder.CreateAlloca(
            llvm::Type::getInt8PtrTy(context), nullptr, arg.getName());
        builder.CreateStore(&arg, alloca);
        variables[std::string(arg.getName())] = {alloca, ZenType::String};
    }

    for (auto& stmt : node->body) {
        generateNodeInLoop(stmt.get());
    }

    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateRet(llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context)));
    }

    variables = savedVars;
}

// ============================================================
// Generacion de nodos
// ============================================================

void CodeGen::generateNode(ASTNode* node) {
    switch (node->kind) {
        case NodeType::PrintStmt:    generatePrint(static_cast<PrintStmt*>(node)); break;
        case NodeType::AssignStmt:   generateAssign(static_cast<AssignStmt*>(node)); break;
        case NodeType::AugAssignStmt: generateAugAssign(static_cast<AugAssignStmt*>(node)); break;
        case NodeType::IfStmt:       generateIf(static_cast<IfStmt*>(node)); break;
        case NodeType::ForRangeStmt: generateForRange(static_cast<ForRangeStmt*>(node)); break;
        case NodeType::ForEachStmt:  generateForEach(static_cast<ForEachStmt*>(node)); break;
        case NodeType::WhileStmt:    generateWhile(static_cast<WhileStmt*>(node)); break;
        case NodeType::RepeatStmt:   generateRepeat(static_cast<RepeatStmt*>(node)); break;
        case NodeType::BreakStmt: {
            if (!loopStack.empty()) {
                builder.CreateBr(loopStack.back().first);
            }
            break;
        }
        case NodeType::ContinueStmt: {
            if (!loopStack.empty()) {
                builder.CreateBr(loopStack.back().second);
            }
            break;
        }
        case NodeType::PlatformBlock: generatePlatformBlock(static_cast<PlatformBlock*>(node)); break;
        case NodeType::ReturnStmt:   generateReturn(static_cast<ReturnStmt*>(node)); break;
        case NodeType::IndexAssign:  generateIndexAssign(static_cast<IndexAssign*>(node)); break;
        case NodeType::MemberAssign: generateMemberAssign(static_cast<MemberAssign*>(node)); break;
        case NodeType::StructDecl:   registerStructDecl(static_cast<StructDecl*>(node)); break;
        case NodeType::MatchStmt:    generateMatch(static_cast<MatchStmt*>(node)); break;
        case NodeType::ExternDecl:   registerExternDecl(static_cast<ExternDecl*>(node)); break;
        case NodeType::ExprStmt:     generateExpr(static_cast<ExprStmt*>(node)->expr.get()); break;
        default: generateExpr(node); break;
    }
}

// Igual que generateNode pero para dentro de bucles
// (hace branch al break/continue BB si ya hay terminador)
void CodeGen::generateNodeInLoop(ASTNode* node) {
    generateNode(node);
}

void CodeGen::generatePrint(PrintStmt* node) {
    auto [value, type] = generateExpr(node->value.get());

    switch (type) {
        case ZenType::Number:
            builder.CreateCall(llvm::FunctionCallee(printfFunc), {
                builder.CreateGlobalStringPtr("%g\n"), value});
            break;
        case ZenType::String:
            builder.CreateCall(llvm::FunctionCallee(printfFunc), {
                builder.CreateGlobalStringPtr("%s\n"), value});
            break;
        case ZenType::Bool: {
            auto* yesStr = builder.CreateGlobalStringPtr("yes\n");
            auto* noStr  = builder.CreateGlobalStringPtr("no\n");
            auto* sel = builder.CreateSelect(value, yesStr, noStr, "boolstr");
            builder.CreateCall(llvm::FunctionCallee(printfFunc), {
                builder.CreateGlobalStringPtr("%s\n"), sel});
            break;
        }
        case ZenType::Null:
            builder.CreateCall(llvm::FunctionCallee(printfFunc), {
                builder.CreateGlobalStringPtr("nada\n")});
            break;
        case ZenType::Struct:
            // Sin info del tipo en runtime, mostramos placeholder.
            // El usuario debe acceder a los campos con .x, .y, etc.
            builder.CreateCall(llvm::FunctionCallee(printfFunc), {
                builder.CreateGlobalStringPtr("<struct>\n")});
            break;
        case ZenType::List: {
            // Imprimir como [elem1, elem2, elem3, ...]
            builder.CreateCall(llvm::FunctionCallee(printfFunc), {
                builder.CreateGlobalStringPtr("[")});

            auto* i64Ty = llvm::Type::getInt64Ty(context);
            auto* iAlloca = builder.CreateAlloca(i64Ty, nullptr, "pi");
            builder.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iAlloca);

            auto* len = builder.CreateCall(llvm::FunctionCallee(listLenFunc), {value}, "len");

            auto* func = currentFunction;
            auto* condBB = llvm::BasicBlock::Create(context, "lp.cond", func);
            auto* bodyBB = llvm::BasicBlock::Create(context, "lp.body", func);
            auto* contBB = llvm::BasicBlock::Create(context, "lp.cont", func);
            auto* endBB  = llvm::BasicBlock::Create(context, "lp.end", func);
            builder.CreateBr(condBB);

            builder.SetInsertPoint(condBB);
            auto* i = builder.CreateLoad(i64Ty, iAlloca, "i");
            auto* cmp = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, i, len, "cmp");
            builder.CreateCondBr(cmp, bodyBB, endBB);

            builder.SetInsertPoint(bodyBB);
            // Si i > 0, imprimir ", "
            auto* isNotFirst = builder.CreateICmp(llvm::CmpInst::ICMP_SGT, i,
                llvm::ConstantInt::get(i64Ty, 0), "nf");
            auto* thenBB = llvm::BasicBlock::Create(context, "lp.then", func);
            // CondBr: si isNotFirst (i>0) → thenBB (imprimir coma); sino → contBB
            builder.CreateCondBr(isNotFirst, thenBB, contBB);
            builder.SetInsertPoint(thenBB);
            builder.CreateCall(llvm::FunctionCallee(printfFunc),
                {builder.CreateGlobalStringPtr(", ")});
            builder.CreateBr(contBB);

            builder.SetInsertPoint(contBB);
            // Imprimir elemento
            auto* elem = builder.CreateCall(
                llvm::FunctionCallee(listGetFunc), {value, i}, "elem");
            // Si elem es null, imprimir "nada"; sino imprimir el string
            auto* isNull = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, elem,
                llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context)), "n");
            auto* nullBB = llvm::BasicBlock::Create(context, "lp.null", func);
            auto* strBB  = llvm::BasicBlock::Create(context, "lp.str", func);
            auto* afterBB = llvm::BasicBlock::Create(context, "lp.after", func);
            builder.CreateCondBr(isNull, nullBB, strBB);
            builder.SetInsertPoint(nullBB);
            builder.CreateCall(llvm::FunctionCallee(printfFunc),
                {builder.CreateGlobalStringPtr("nada")});
            builder.CreateBr(afterBB);
            builder.SetInsertPoint(strBB);
            builder.CreateCall(llvm::FunctionCallee(printfFunc),
                {builder.CreateGlobalStringPtr("%s"), elem});
            builder.CreateBr(afterBB);

            builder.SetInsertPoint(afterBB);
            auto* next = builder.CreateAdd(i, llvm::ConstantInt::get(i64Ty, 1), "next");
            builder.CreateStore(next, iAlloca);
            builder.CreateBr(condBB);

            builder.SetInsertPoint(endBB);
            builder.CreateCall(llvm::FunctionCallee(printfFunc),
                {builder.CreateGlobalStringPtr("]\n")});
            break;
        }
        default:
            break;
    }
}

void CodeGen::generateAssign(AssignStmt* node) {
    auto [value, type] = generateExpr(node->value.get());

    auto it = variables.find(node->name);
    if (it != variables.end()) {
        // Obtener tipo del puntero (funciona para AllocaInst y GlobalVariable)
        auto* pt = llvm::dyn_cast<llvm::PointerType>(it->second.alloca->getType());
        llvm::Type* et = pt ? pt->getElementType() : llvm::Type::getDoubleTy(context);
        if (et == llvm::Type::getDoubleTy(context) && value->getType() != llvm::Type::getDoubleTy(context))
            builder.CreateStore(toDouble(value, type), it->second.alloca);
        else if (et == llvm::Type::getInt8PtrTy(context) && value->getType() != llvm::Type::getInt8PtrTy(context))
            builder.CreateStore(boxValue(value, type), it->second.alloca);
        else
            builder.CreateStore(value, it->second.alloca);
        it->second.type = type;
    } else {
        // SIEMPRE usar i8* (boxed) para todas las variables
        // Esto simplifica todo: numeros se guardan como string, strings directo, etc.
        auto* entryBB = &currentFunction->getEntryBlock();
        llvm::IRBuilder<> tmpBuilder(entryBB, entryBB->begin());
        auto* alloca = tmpBuilder.CreateAlloca(llvm::Type::getInt8PtrTy(context), nullptr, node->name);
        builder.CreateStore(boxValue(value, type), alloca);
        variables[node->name] = {alloca, type};
    }
}

void CodeGen::generateAugAssign(AugAssignStmt* node) {
    // Generar: var = var OP value
    auto it = variables.find(node->name);
    if (it == variables.end()) {
        throw std::runtime_error(
            "Variable '" + node->name + "' no definida en linea " +
            std::to_string(node->line));
    }

    auto* varAlloca = it->second.alloca;
    auto varType = it->second.type;

    // Cargar valor actual
    llvm::Value* curVal;
    // Obtener tipo del puntero (GlobalVariable o AllocaInst)
    auto* pt0 = llvm::dyn_cast<llvm::PointerType>(varAlloca->getType());
    llvm::Type* vt0 = pt0 ? pt0->getElementType() : llvm::Type::getDoubleTy(context);
    if (vt0 == llvm::Type::getInt8PtrTy(context)) {
        // Variable boxed (i8*): cargar como i8* y convertir si es necesario
        auto* boxedVal = builder.CreateLoad(llvm::Type::getInt8PtrTy(context), varAlloca, node->name);
        if (node->op == "+" && varType == ZenType::String) {
            curVal = boxedVal;
        } else {
            curVal = boxedVal;  // se convertira con toDouble mas adelante
        }
    } else {
        curVal = builder.CreateLoad(vt0, varAlloca, node->name);
    }

    // Generar el valor del lado derecho
    auto [rhsVal, rhsType] = generateExpr(node->value.get());

    // Concatenacion de strings: x += " mundo"
    // Tambien: x += 42 (convierte el numero a string)
    if (varType == ZenType::String && node->op == "+") {
        auto* curStr = curVal;
        auto* rhsStr = toString(rhsVal, rhsType);
        auto* result = builder.CreateCall(llvm::FunctionCallee(concatFunc), {curStr, rhsStr}, "concat");
        // Obtener tipo del puntero (GlobalVariable o AllocaInst)
        auto* pt1 = llvm::dyn_cast<llvm::PointerType>(varAlloca->getType());
        llvm::Type* vt1 = pt1 ? pt1->getElementType() : llvm::Type::getInt8PtrTy(context);
        if (vt1 == llvm::Type::getDoubleTy(context))
            builder.CreateStore(toDouble(result, ZenType::String), varAlloca);
        else
            builder.CreateStore(boxValue(result, ZenType::String), varAlloca);
        return;
    }

    // Aritmetica
    // Si curVal es i8* (boxed), convertir a double
    auto* l = curVal;
    if (l->getType() == llvm::Type::getInt8PtrTy(context))
        l = toDouble(l, ZenType::String);
    else if (varType != ZenType::Number)
        l = toDouble(l, varType);
    auto* r = (rhsType == ZenType::Number) ? rhsVal : toDouble(rhsVal, rhsType);

    llvm::Value* result = nullptr;
    if (node->op == "+")      result = builder.CreateFAdd(l, r, "add");
    else if (node->op == "-") result = builder.CreateFSub(l, r, "sub");
    else if (node->op == "*") result = builder.CreateFMul(l, r, "mul");
    else if (node->op == "/") result = builder.CreateFDiv(l, r, "div");
    else if (node->op == "%") result = builder.CreateFRem(l, r, "mod");
    else if (node->op == "^") result = builder.CreateCall(llvm::FunctionCallee(powFunc), {l, r}, "pow");
    else {
        throw std::runtime_error(
            "Operador desconocido '" + node->op + "' en linea " +
            std::to_string(node->line));
    }

    // Obtener tipo del puntero (GlobalVariable o AllocaInst)
    auto* pt2 = llvm::dyn_cast<llvm::PointerType>(varAlloca->getType());
    llvm::Type* vt2 = pt2 ? pt2->getElementType() : llvm::Type::getDoubleTy(context);
    if (vt2 == llvm::Type::getDoubleTy(context))
        builder.CreateStore(result, varAlloca);
    else
        builder.CreateStore(boxValue(result, ZenType::Number), varAlloca);
}

void CodeGen::generateIf(IfStmt* node) {
    auto [condVal, condType] = generateExpr(node->condition.get());
    auto* condBool = toBool(condVal, condType);

    auto* func = currentFunction;
    auto* thenBB = llvm::BasicBlock::Create(context, "then", func);
    auto* elseBB = node->elseBody.empty() ? nullptr
        : llvm::BasicBlock::Create(context, "else", func);
    auto* mergeBB = llvm::BasicBlock::Create(context, "endif", func);

    builder.CreateCondBr(condBool, thenBB, elseBB ? elseBB : mergeBB);

    builder.SetInsertPoint(thenBB);
    for (auto& stmt : node->thenBody) {
        generateNodeInLoop(stmt.get());
        if (builder.GetInsertBlock()->getTerminator()) break;
    }
    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(mergeBB);
    }

    if (elseBB) {
        builder.SetInsertPoint(elseBB);
        for (auto& stmt : node->elseBody) {
            generateNodeInLoop(stmt.get());
            if (builder.GetInsertBlock()->getTerminator()) break;
        }
        if (!builder.GetInsertBlock()->getTerminator()) {
            builder.CreateBr(mergeBB);
        }
    }

    builder.SetInsertPoint(mergeBB);
}

void CodeGen::generateForRange(ForRangeStmt* node) {
    auto [fromVal, fromType] = generateExpr(node->from.get());
    auto [toVal, toType] = generateExpr(node->to.get());

    if (fromVal && fromVal->getType() == llvm::Type::getInt8PtrTy(context))
        fromVal = toDouble(fromVal, ZenType::String);
    else if (fromType != ZenType::Number) fromVal = toDouble(fromVal, fromType);
    if (toVal->getType() == llvm::Type::getInt8PtrTy(context))
        toVal = toDouble(toVal, ZenType::String);
    else if (toType != ZenType::Number)   toVal   = toDouble(toVal, toType);

    auto* func = currentFunction;
    auto* condBB = llvm::BasicBlock::Create(context, "for.cond", func);
    auto* bodyBB = llvm::BasicBlock::Create(context, "for.body", func);
    auto* incBB  = llvm::BasicBlock::Create(context, "for.inc", func);
    auto* breakBB = llvm::BasicBlock::Create(context, "for.end", func);

    auto* alloca = builder.CreateAlloca(
        llvm::Type::getDoubleTy(context), nullptr, node->varName);
    builder.CreateStore(fromVal, alloca);

    VarInfo savedVar = variables.count(node->varName) ? variables[node->varName] : VarInfo{};
    variables[node->varName] = {alloca, ZenType::Number};

    builder.CreateBr(condBB);

    // cond: i < to
    builder.SetInsertPoint(condBB);
    auto* varVal = builder.CreateLoad(llvm::Type::getDoubleTy(context), alloca, node->varName);
    auto* cmp = builder.CreateFCmp(llvm::CmpInst::FCMP_OLT, varVal, toVal, "cmp");
    builder.CreateCondBr(cmp, bodyBB, breakBB);

    // body
    builder.SetInsertPoint(bodyBB);
    loopStack.push_back({breakBB, incBB});
    for (auto& stmt : node->body) {
        generateNode(stmt.get());
        if (builder.GetInsertBlock()->getTerminator()) break;
    }
    loopStack.pop_back();
    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(incBB);
    }

    // inc: i++
    builder.SetInsertPoint(incBB);
    auto* curVal = builder.CreateLoad(llvm::Type::getDoubleTy(context), alloca, node->varName);
    auto* nextVal = builder.CreateFAdd(
        curVal, llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 1.0), "next");
    builder.CreateStore(nextVal, alloca);
    builder.CreateBr(condBB);

    builder.SetInsertPoint(breakBB);

    if (savedVar.alloca) {
        variables[node->varName] = savedVar;
    } else {
        variables.erase(node->varName);
    }
}

void CodeGen::generateForEach(ForEachStmt* node) {
    // para cada x en expr
    // Dos modos:
    //   1. Si expr es rango(a, b) → for range
    //   2. Si expr es una lista → iterar elementos

    // Modo 1: rango()
    if (node->iterable->kind == NodeType::FuncCall) {
        auto* call = static_cast<FuncCall*>(node->iterable.get());
        std::string lowerName = call->name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        if (lowerName == "rango" || lowerName == "range") {
            if (call->args.size() >= 2) {
                ForRangeStmt fr(node->varName,
                    std::move(call->args[0]),
                    std::move(call->args[1]),
                    std::move(node->body), node->line);
                generateForRange(&fr);
                return;
            }
        }
    }

    // Modo 2: iterar una lista real
    auto [iterVal, iterType] = generateExpr(node->iterable.get());
    // Aceptar cualquier tipo (puede ser Null del pre-scan)
    auto* iterPtr = builder.CreateBitCast(iterVal, llvm::Type::getInt8PtrTy(context), "iter");

    auto* i64Ty = llvm::Type::getInt64Ty(context);
    auto* i8Ptr = llvm::Type::getInt8PtrTy(context);
    auto* doubleTy = llvm::Type::getDoubleTy(context);

    auto* func = currentFunction;
    auto* condBB = llvm::BasicBlock::Create(context, "fe.cond", func);
    auto* bodyBB = llvm::BasicBlock::Create(context, "fe.body", func);
    auto* incBB  = llvm::BasicBlock::Create(context, "fe.inc", func);
    auto* endBB  = llvm::BasicBlock::Create(context, "fe.end", func);

    // len = list_len(iter)
    auto* len = builder.CreateCall(llvm::FunctionCallee(listLenFunc), {iterPtr}, "len");

    // i = 0
    auto* iAlloca = builder.CreateAlloca(i64Ty, nullptr, "i");
    builder.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iAlloca);

    // var = ""  (placeholder, se actualiza en cada iteracion)
    auto* varAlloca = builder.CreateAlloca(i8Ptr, nullptr, node->varName);
    builder.CreateStore(llvm::ConstantPointerNull::get(i8Ptr), varAlloca);

    // Guardar scope
    VarInfo savedVar = variables.count(node->varName) ? variables[node->varName] : VarInfo{};
    variables[node->varName] = {varAlloca, ZenType::String};

    builder.CreateBr(condBB);

    // cond: i < len
    builder.SetInsertPoint(condBB);
    auto* iVal = builder.CreateLoad(i64Ty, iAlloca, "i");
    auto* cmp = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, iVal, len, "cmp");
    builder.CreateCondBr(cmp, bodyBB, endBB);

    // body: var = list_get(iter, i); generar cuerpo
    builder.SetInsertPoint(bodyBB);
    auto* elem = builder.CreateCall(
        llvm::FunctionCallee(listGetFunc), {iterPtr, iVal}, "elem");
    builder.CreateStore(elem, varAlloca);

    loopStack.push_back({endBB, incBB});
    for (auto& stmt : node->body) {
        generateNode(stmt.get());
        if (builder.GetInsertBlock()->getTerminator()) break;
    }
    loopStack.pop_back();
    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(incBB);
    }

    // inc: i++
    builder.SetInsertPoint(incBB);
    auto* next = builder.CreateAdd(iVal, llvm::ConstantInt::get(i64Ty, 1), "next");
    builder.CreateStore(next, iAlloca);
    builder.CreateBr(condBB);

    // end
    builder.SetInsertPoint(endBB);

    // Restaurar scope
    if (savedVar.alloca) {
        variables[node->varName] = savedVar;
    } else {
        variables.erase(node->varName);
    }
}

void CodeGen::generateWhile(WhileStmt* node) {
    auto* func = currentFunction;
    auto* condBB  = llvm::BasicBlock::Create(context, "while.cond", func);
    auto* bodyBB  = llvm::BasicBlock::Create(context, "while.body", func);
    auto* breakBB = llvm::BasicBlock::Create(context, "while.end", func);

    builder.CreateBr(condBB);

    // cond
    builder.SetInsertPoint(condBB);
    auto [condVal, condType] = generateExpr(node->condition.get());
    auto* condBool = toBool(condVal, condType);
    builder.CreateCondBr(condBool, bodyBB, breakBB);

    // body
    builder.SetInsertPoint(bodyBB);
    loopStack.push_back({breakBB, condBB});  // continue vuelve a la condicion
    for (auto& stmt : node->body) {
        generateNode(stmt.get());
        if (builder.GetInsertBlock()->getTerminator()) break;
    }
    loopStack.pop_back();
    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(condBB);
    }

    builder.SetInsertPoint(breakBB);
}

void CodeGen::generateRepeat(RepeatStmt* node) {
    auto [countVal, countType] = generateExpr(node->count.get());
    if (countType != ZenType::Number) {
        countVal = toDouble(countVal, countType);
    }

    auto* func = currentFunction;
    auto* counterAlloca = builder.CreateAlloca(
        llvm::Type::getDoubleTy(context), nullptr, "_rep_i");
    builder.CreateStore(llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0), counterAlloca);

    auto* condBB  = llvm::BasicBlock::Create(context, "repeat.cond", func);
    auto* bodyBB  = llvm::BasicBlock::Create(context, "repeat.body", func);
    auto* incBB   = llvm::BasicBlock::Create(context, "repeat.inc", func);
    auto* breakBB = llvm::BasicBlock::Create(context, "repeat.end", func);

    builder.CreateBr(condBB);

    // cond: i < count
    builder.SetInsertPoint(condBB);
    auto* iVal = builder.CreateLoad(llvm::Type::getDoubleTy(context), counterAlloca, "i");
    auto* cmp = builder.CreateFCmp(llvm::CmpInst::FCMP_OLT, iVal, countVal, "cmp");
    builder.CreateCondBr(cmp, bodyBB, breakBB);

    // body
    builder.SetInsertPoint(bodyBB);
    loopStack.push_back({breakBB, incBB});
    for (auto& stmt : node->body) {
        generateNode(stmt.get());
        if (builder.GetInsertBlock()->getTerminator()) break;
    }
    loopStack.pop_back();
    if (!builder.GetInsertBlock()->getTerminator()) {
        builder.CreateBr(incBB);
    }

    // inc: i++
    builder.SetInsertPoint(incBB);
    auto* curI = builder.CreateLoad(llvm::Type::getDoubleTy(context), counterAlloca, "i");
    auto* nextI = builder.CreateFAdd(
        curI, llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 1.0), "next");
    builder.CreateStore(nextI, counterAlloca);
    builder.CreateBr(condBB);

    builder.SetInsertPoint(breakBB);
}

void CodeGen::generatePlatformBlock(PlatformBlock* node) {
    // Si el target de compilacion coincide con la plataforma del bloque,
    // generar su contenido. Si no, omitirlo completamente.
    if (targetPlatform == node->platform) {
        for (auto& stmt : node->body) {
            generateNode(stmt.get());
        }
    }
    // Si no coincide, no generamos nada — el bloque se elimina en compile time.
}

void CodeGen::generateReturn(ReturnStmt* node) {
    auto [value, type] = generateExpr(node->value.get());
    // Boxear el valor como i8* (todas las funciones retornan i8*)
    llvm::Value* retValue = boxValue(value, type);
    builder.CreateRet(retValue);
}

// ============================================================
// Expresiones
// ============================================================

std::pair<llvm::Value*, ZenType> CodeGen::generateExpr(ASTNode* node) {
    switch (node->kind) {
        case NodeType::NumberLit: {
            auto* n = static_cast<NumberLit*>(node);
            return {llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), n->value), ZenType::Number};
        }
        case NodeType::StringLit: {
            auto* s = static_cast<StringLit*>(node);
            return {builder.CreateGlobalStringPtr(s->value), ZenType::String};
        }
        case NodeType::FStringLit: {
            // f"Hola {nombre}, tienes {edad}"
            // → concatenar todas las partes, convirtiendo expr a string
            auto* fsl = static_cast<FStringLit*>(node);
            if (fsl->parts.empty()) {
                return {builder.CreateGlobalStringPtr(""), ZenType::String};
            }
            // Empezar con la primera parte
            llvm::Value* result = nullptr;
            for (const auto& part : fsl->parts) {
                llvm::Value* partVal = nullptr;
                if (part.isExpr) {
                    auto [v, t] = generateExpr(part.expr.get());
                    partVal = toString(v, t);
                } else {
                    partVal = builder.CreateGlobalStringPtr(part.text);
                }
                if (result == nullptr) {
                    result = partVal;
                } else {
                    // Llamar a zen_concat(a, b) que ya existe en runtime.c
                    auto* concatFunc = module->getFunction("zen_concat");
                    if (!concatFunc) {
                        // Declarar extern si no existe
                        auto* i8Ptr = llvm::Type::getInt8PtrTy(context);
                        concatFunc = llvm::Function::Create(
                            llvm::FunctionType::get(i8Ptr, {i8Ptr, i8Ptr}, false),
                            llvm::Function::ExternalLinkage, "zen_concat", module.get());
                    }
                    result = builder.CreateCall(concatFunc, {result, partVal});
                }
            }
            return {result, ZenType::String};
        }
        case NodeType::BoolLit: {
            auto* b = static_cast<BoolLit*>(node);
            return {llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), b->value ? 1 : 0), ZenType::Bool};
        }
        case NodeType::NullLit: {
            return {llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context)), ZenType::Null};
        }
        case NodeType::Identifier: {
            auto* id = static_cast<Identifier*>(node);
            auto it = variables.find(id->name);
            if (it == variables.end()) {
                std::string hint = "";
                int bestDist = 999;
                std::string bestMatch;
                for (auto& [name, _] : variables) {
                    int dist = 0;
                    std::string a = id->name, b = name;
                    size_t minLen = std::min(a.size(), b.size());
                    size_t maxLen = std::max(a.size(), b.size());
                    for (size_t i = 0; i < minLen; i++) if (a[i] != b[i]) dist++;
                    dist += (int)(maxLen - minLen);
                    if (dist < bestDist) { bestDist = dist; bestMatch = name; }
                }
                if (bestDist <= 3 && !bestMatch.empty()) {
                    hint = ". Quisiste decir '" + bestMatch + "' ?";
                }
                throw std::runtime_error(
                    "Variable '" + id->name + "' no definida en linea " +
                    std::to_string(node->line) + hint);
            }
            auto* pt = llvm::dyn_cast<llvm::PointerType>(it->second.alloca->getType());
            llvm::Type* et = pt ? pt->getElementType() : llvm::Type::getDoubleTy(context);
            return {builder.CreateLoad(et, it->second.alloca, id->name), it->second.type};
        }
        case NodeType::TernaryOp:
            return generateTernary(static_cast<TernaryOp*>(node));
        case NodeType::BinaryOp:
            return generateBinaryOp(static_cast<BinaryOp*>(node));
        case NodeType::UnaryOp:
            return generateUnaryOp(static_cast<UnaryOp*>(node));
        case NodeType::FuncCall:
            return generateFuncCallExpr(static_cast<FuncCall*>(node));
        case NodeType::ListLit:
            return generateListLit(static_cast<ListLit*>(node));
        case NodeType::IndexAccess:
            return generateIndexAccess(static_cast<IndexAccess*>(node));
        case NodeType::StructLit:
            return generateStructLit(static_cast<StructLit*>(node));
        case NodeType::MemberAccess:
            return generateMemberAccess(static_cast<MemberAccess*>(node));
        case NodeType::ExternCall:
            return generateExternCall(static_cast<ExternCall*>(node));
        default:
            throw std::runtime_error(
                "Expresion invalida en linea " + std::to_string(node->line));
    }
}

std::pair<llvm::Value*, ZenType> CodeGen::generateTernary(TernaryOp* node) {
    auto [condVal, condType] = generateExpr(node->condition.get());
    auto* condBool = toBool(condVal, condType);

    auto* func = currentFunction;
    auto* thenBB = llvm::BasicBlock::Create(context, "tern.then", func);
    auto* elseBB = llvm::BasicBlock::Create(context, "tern.else", func);
    auto* mergeBB = llvm::BasicBlock::Create(context, "tern.merge", func);

    builder.CreateCondBr(condBool, thenBB, elseBB);

    // Rama entonces
    builder.SetInsertPoint(thenBB);
    auto [thenVal, thenType] = generateExpr(node->thenExpr.get());
    builder.CreateBr(mergeBB);
    thenBB = builder.GetInsertBlock();  // puede haber cambiado por branches internos

    // Rama sino
    builder.SetInsertPoint(elseBB);
    auto [elseVal, elseType] = generateExpr(node->elseExpr.get());
    builder.CreateBr(mergeBB);
    elseBB = builder.GetInsertBlock();

    // Merge con phi. Decidir el tipo resultante:
    // - Si ambos son Number → Number (double)
    // - Si alguno es String → String (i8*) convertir el otro con toString
    // - Si alguno es Bool → Bool (i1) convertir el otro con toBool
    // - Default → Number (double) convertir con toDouble
    builder.SetInsertPoint(mergeBB);

    ZenType resultType;
    if (thenType == ZenType::String || elseType == ZenType::String) {
        resultType = ZenType::String;
    } else if (thenType == ZenType::Bool && elseType == ZenType::Bool) {
        resultType = ZenType::Bool;
    } else {
        resultType = ZenType::Number;
    }

    llvm::Value* thenFinal;
    llvm::Value* elseFinal;
    llvm::Type* phiType;

    switch (resultType) {
        case ZenType::String:
            thenFinal = toString(thenVal, thenType);
            elseFinal = toString(elseVal, elseType);
            phiType = llvm::Type::getInt8PtrTy(context);
            break;
        case ZenType::Bool:
            thenFinal = toBool(thenVal, thenType);
            elseFinal = toBool(elseVal, elseType);
            phiType = llvm::Type::getInt1Ty(context);
            break;
        default:  // Number
            thenFinal = (thenType == ZenType::Number) ? thenVal : toDouble(thenVal, thenType);
            elseFinal = (elseType == ZenType::Number) ? elseVal : toDouble(elseVal, elseType);
            phiType = llvm::Type::getDoubleTy(context);
            break;
    }

    auto* phi = builder.CreatePHI(phiType, 2, "tern");
    phi->addIncoming(thenFinal, thenBB);
    phi->addIncoming(elseFinal, elseBB);

    return {phi, resultType};
}

std::pair<llvm::Value*, ZenType> CodeGen::generateBinaryOp(BinaryOp* node) {
    auto [lv, lt] = generateExpr(node->left.get());
    auto [rv, rt] = generateExpr(node->right.get());

    if (node->op == "and" || node->op == "y") {
        return {builder.CreateAnd(toBool(lv, lt), toBool(rv, rt), "and"), ZenType::Bool};
    }
    if (node->op == "or" || node->op == "o") {
        return {builder.CreateOr(toBool(lv, lt), toBool(rv, rt), "or"), ZenType::Bool};
    }

    if (node->op == "==" || node->op == "!=" || node->op == "<" ||
        node->op == ">"  || node->op == "<=" || node->op == ">=") {

        // Comparacion de strings con strcmp
        if (lt == ZenType::String && rt == ZenType::String) {
            auto* cmpRes = builder.CreateCall(
                llvm::FunctionCallee(strcmpFunc), {lv, rv}, "strcmp");
            // strcmp retorna 0 si son iguales, <0 si a<b, >0 si a>b
            auto* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            llvm::CmpInst::Predicate pred;
            if (node->op == "==")      pred = llvm::CmpInst::ICMP_EQ;
            else if (node->op == "!=") pred = llvm::CmpInst::ICMP_NE;
            else if (node->op == "<")  pred = llvm::CmpInst::ICMP_SLT;
            else if (node->op == ">")  pred = llvm::CmpInst::ICMP_SGT;
            else if (node->op == "<=") pred = llvm::CmpInst::ICMP_SLE;
            else                       pred = llvm::CmpInst::ICMP_SGE;
            return {builder.CreateICmp(pred, cmpRes, zero, "scmp"), ZenType::Bool};
        }

        auto* l = lv;
        if (l->getType() == llvm::Type::getInt8PtrTy(context)) l = toDouble(l, ZenType::String);
        else if (lt != ZenType::Number) l = toDouble(l, lt);
        auto* r = rv;
        if (r->getType() == llvm::Type::getInt8PtrTy(context)) r = toDouble(r, ZenType::String);
        else if (rt != ZenType::Number) r = toDouble(r, rt);

        llvm::CmpInst::Predicate pred;
        if (node->op == "==") pred = llvm::CmpInst::FCMP_OEQ;
        else if (node->op == "!=") pred = llvm::CmpInst::FCMP_ONE;
        else if (node->op == "<")  pred = llvm::CmpInst::FCMP_OLT;
        else if (node->op == ">")  pred = llvm::CmpInst::FCMP_OGT;
        else if (node->op == "<=") pred = llvm::CmpInst::FCMP_OLE;
        else                       pred = llvm::CmpInst::FCMP_OGE;

        return {builder.CreateFCmp(pred, l, r, "cmp"), ZenType::Bool};
    }

    // Concatenacion de strings: "a" + "b"
    // Tambien soporta: "a" + 42  o  42 + "a"  (convierte el numero a string)
    if (node->op == "+" && (lt == ZenType::String || rt == ZenType::String)) {
        // Si ambos son string, ir directo
        if (lt == ZenType::String && rt == ZenType::String) {
            return {builder.CreateCall(llvm::FunctionCallee(concatFunc), {lv, rv}, "concat"), ZenType::String};
        }
        // Si solo uno es string, convertir el otro
        auto* lstr = toString(lv, lt);
        auto* rstr = toString(rv, rt);
        return {builder.CreateCall(llvm::FunctionCallee(concatFunc), {lstr, rstr}, "concat"), ZenType::String};
    }

    auto* l = lv;
    if (l->getType() == llvm::Type::getInt8PtrTy(context)) l = toDouble(l, ZenType::String);
    else if (lt != ZenType::Number) l = toDouble(l, lt);
    auto* r = rv;
    if (r->getType() == llvm::Type::getInt8PtrTy(context)) r = toDouble(r, ZenType::String);
    else if (rt != ZenType::Number) r = toDouble(r, rt);

    if (node->op == "+") return {builder.CreateFAdd(l, r, "add"), ZenType::Number};
    if (node->op == "-") return {builder.CreateFSub(l, r, "sub"), ZenType::Number};
    if (node->op == "*") return {builder.CreateFMul(l, r, "mul"), ZenType::Number};
    if (node->op == "/") return {builder.CreateFDiv(l, r, "div"), ZenType::Number};
    if (node->op == "%") return {builder.CreateFRem(l, r, "mod"), ZenType::Number};
    if (node->op == "^") {
        return {builder.CreateCall(llvm::FunctionCallee(powFunc), {l, r}, "pow"), ZenType::Number};
    }

    throw std::runtime_error(
        "Operador desconocido '" + node->op + "' en linea " +
        std::to_string(node->line));
}

std::pair<llvm::Value*, ZenType> CodeGen::generateUnaryOp(UnaryOp* node) {
    auto [val, type] = generateExpr(node->operand.get());

    if (node->op == "not") {
        auto* b = toBool(val, type);
        return {builder.CreateNot(b, "not"), ZenType::Bool};
    }
    if (node->op == "-") {
        auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
        return {builder.CreateFNeg(d, "neg"), ZenType::Number};
    }

    throw std::runtime_error(
        "Operador unario desconocido '" + node->op + "' en linea " +
        std::to_string(node->line));
}

std::pair<llvm::Value*, ZenType> CodeGen::generateFuncCallExpr(FuncCall* node) {
    // Primero, verificar si es una funcion externa (FFI)
    auto extIt = externs.find(node->name);
    if (extIt != externs.end()) {
        // Convertir FuncCall a ExternCall
        std::vector<std::unique_ptr<ASTNode>> argsCopy;
        for (auto& arg : node->args) {
            // Necesitamos clonar el arg. Pero como ya lo vamos a generar,
            // podemos hacer un cast. En realidad, lo más simple es crear un ExternCall
            // temporal y usarlo.
            // Pero los unique_ptr no se pueden copiar. Workaround: generar directamente.
        }
        // Workaround: generar los argumentos y la llamada aqui mismo
        auto& info = extIt->second;
        auto* func = info.func;

        if (node->args.size() != info.params.size()) {
            throw std::runtime_error(
                "Funcion externa '" + node->name + "' espera " +
                std::to_string(info.params.size()) + " argumentos pero se pasaron " +
                std::to_string(node->args.size()) + " en linea " +
                std::to_string(node->line));
        }

        // Convertir cada argumento al tipo C correcto
        std::vector<llvm::Value*> args;
        for (size_t i = 0; i < node->args.size(); i++) {
            auto [val, type] = generateExpr(node->args[i].get());
            auto& paramInfo = info.params[i];

            llvm::Value* converted = val;
            if (paramInfo.type == CType::Str) {
                converted = toString(val, type);
            } else if (paramInfo.type == CType::Double) {
                if (type != ZenType::Number) {
                    converted = toDouble(val, type);
                }
            } else if (paramInfo.type == CType::Float) {
                auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
                converted = builder.CreateFPTrunc(d, llvm::Type::getFloatTy(context), "tof32");
            } else if (paramInfo.type == CType::Int) {
                auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
                converted = builder.CreateFPToSI(d, llvm::Type::getInt32Ty(context), "toi32");
            } else if (paramInfo.type == CType::UInt) {
                auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
                converted = builder.CreateFPToUI(d, llvm::Type::getInt32Ty(context), "tou32");
            } else if (paramInfo.type == CType::Long) {
                auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
                converted = builder.CreateFPToSI(d, llvm::Type::getInt64Ty(context), "toi64");
            } else if (paramInfo.type == CType::Char) {
                auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
                converted = builder.CreateFPToSI(d, llvm::Type::getInt8Ty(context), "toi8");
            } else if (paramInfo.type == CType::Ptr) {
                converted = toString(val, type);
            }
            args.push_back(converted);
        }

        // Si retorna void, no poner nombre a la llamada
        if (info.returnType == CType::Void) {
            builder.CreateCall(llvm::FunctionCallee(func), args);
            return {llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0), ZenType::Number};
        }

        auto* result = builder.CreateCall(llvm::FunctionCallee(func), args, "c_call");

        // Convertir el resultado de vuelta a ZenType
        ZenType zenType;
        llvm::Value* zenVal = result;
        switch (info.returnType) {
            case CType::Void:
                return {llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0), ZenType::Number};
            case CType::Int:
            case CType::Long:
                zenType = ZenType::Number;
                zenVal = builder.CreateSIToFP(result, llvm::Type::getDoubleTy(context), "c2d");
                break;
            case CType::UInt:
                zenType = ZenType::Number;
                zenVal = builder.CreateUIToFP(result, llvm::Type::getDoubleTy(context), "u2d");
                break;
            case CType::Float:
                zenType = ZenType::Number;
                zenVal = builder.CreateFPExt(result, llvm::Type::getDoubleTy(context), "f2d");
                break;
            case CType::Double:
                zenType = ZenType::Number;
                break;
            case CType::Str:
                zenType = ZenType::String;
                break;
            case CType::Ptr:
                zenType = ZenType::String;
                break;
            case CType::Char:
                zenType = ZenType::Number;
                zenVal = builder.CreateSIToFP(result, llvm::Type::getDoubleTy(context), "c2d");
                break;
        }
        return {zenVal, zenType};
    }

    // Primero intentar con builtins (longitud, subtexto, etc.)
    auto builtinResult = tryBuiltinCall(node);
    if (builtinResult.first != nullptr) {
        return builtinResult;
    }

    auto it = functions.find(node->name);
    if (it == functions.end()) {
        throw std::runtime_error(
            "Funcion '" + node->name + "' no definida en linea " +
            std::to_string(node->line));
    }

    auto* func = it->second.func;

    if (node->args.size() != func->arg_size()) {
        throw std::runtime_error(
            "La funcion '" + node->name + "' espera " +
            std::to_string(func->arg_size()) + " argumentos pero se pasaron " +
            std::to_string(node->args.size()) + " en linea " +
            std::to_string(node->line));
    }

    std::vector<llvm::Value*> args;
    for (size_t i = 0; i < node->args.size(); i++) {
        auto [val, type] = generateExpr(node->args[i].get());
        // Boxear todos los argumentos como i8*
        if (val->getType() != llvm::Type::getInt8PtrTy(context))
            val = boxValue(val, type);
        args.push_back(val);
    }

    auto* callResult = builder.CreateCall(llvm::FunctionCallee(func), args, "call");
    // El retorno siempre es i8* (string/boxed)
    return {callResult, ZenType::String};
}

// ============================================================
// Helpers de conversion
// ============================================================

// Convierte cualquier valor Zen a un i8* "boxed" (para guardar en listas).
// Todo elemento de lista se guarda como i8*:
//   Number → string via __zen_num_to_str
//   String → queda igual
//   Bool   → "yes" / "no"
//   Null   → nullptr
//   List   → el i8* opaco de la lista (listas anidadas)
llvm::Value* CodeGen::boxValue(llvm::Value* val, ZenType type) {
    return toString(val, type);
}

// Dado un i8* boxed, lo convierte a numero (usando numero()).
llvm::Value* CodeGen::unboxToNumber(llvm::Value* boxed) {
    auto* nullPtr = llvm::ConstantPointerNull::get(
        llvm::Type::getInt8PtrTy(context)->getPointerTo());
    return builder.CreateCall(llvm::FunctionCallee(strtodFunc), {boxed, nullPtr}, "ubnum");
}

// Genera una lista literal: [1, 2, 3]
std::pair<llvm::Value*, ZenType> CodeGen::generateListLit(ListLit* node) {
    auto* i64Ty = llvm::Type::getInt64Ty(context);
    auto* i8Ptr = llvm::Type::getInt8PtrTy(context);

    // Crear lista con capacidad = max(4, num_elements)
    double cap = std::max(4.0, (double)node->elements.size());
    auto* capVal = llvm::ConstantInt::get(i64Ty, (uint64_t)cap);
    auto* list = builder.CreateCall(llvm::FunctionCallee(listCreateFunc), {capVal}, "list");

    // Push each element
    for (auto& elem : node->elements) {
        auto [val, type] = generateExpr(elem.get());
        auto* boxed = boxValue(val, type);
        builder.CreateCall(llvm::FunctionCallee(listPushFunc), {list, boxed});
    }

    return {list, ZenType::List};
}

// Genera acceso por indice: lista[0]
// Devuelve {i8* boxed, ZenType::String} - el usuario puede convertir con numero() si quiere
std::pair<llvm::Value*, ZenType> CodeGen::generateIndexAccess(IndexAccess* node) {
    // Generar la base (la lista)
    auto [baseVal, baseType] = generateExpr(node->base.get());

    // Aceptar cualquier tipo (puede ser Null del pre-scan)
    baseVal = builder.CreateBitCast(baseVal, llvm::Type::getInt8PtrTy(context), "idxbase");

    // Generar el indice (debe ser numero)
    auto [idxVal, idxType] = generateExpr(node->index.get());
    if (idxType != ZenType::Number) {
        idxVal = toDouble(idxVal, idxType);
    }
    auto* i64Ty = llvm::Type::getInt64Ty(context);
    if (idxVal->getType() == llvm::Type::getInt8PtrTy(context)) idxVal = toDouble(idxVal, ZenType::String);
        auto* idxI64 = builder.CreateFPToSI(idxVal, i64Ty, "idx");

    // list_get(list, idx) -> i8*
    auto* elem = builder.CreateCall(
        llvm::FunctionCallee(listGetFunc), {baseVal, idxI64}, "elem");

    // Devolvemos como String (el usuario puede convertir con numero() si quiere)
    return {elem, ZenType::String};
}

// Genera asignacion por indice: lista[0] = valor
void CodeGen::generateIndexAssign(IndexAssign* node) {
    auto [baseVal, baseType] = generateExpr(node->base.get());

    // Aceptar cualquier tipo (puede ser Null del pre-scan)
    baseVal = builder.CreateBitCast(baseVal, llvm::Type::getInt8PtrTy(context), "idxbase");

    auto [idxVal, idxType] = generateExpr(node->index.get());
    if (idxType != ZenType::Number) {
        idxVal = toDouble(idxVal, idxType);
    }
    auto* i64Ty = llvm::Type::getInt64Ty(context);
    if (idxVal->getType() == llvm::Type::getInt8PtrTy(context)) idxVal = toDouble(idxVal, ZenType::String);
        auto* idxI64 = builder.CreateFPToSI(idxVal, i64Ty, "idx");

    auto [val, valType] = generateExpr(node->value.get());
    auto* boxed = boxValue(val, valType);

    builder.CreateCall(llvm::FunctionCallee(listSetFunc), {baseVal, idxI64, boxed});
}

// ============================================================
// Structs
// ============================================================

void CodeGen::registerStructDecl(StructDecl* node) {
    structs[node->name] = {node->fields};
}

std::pair<llvm::Value*, ZenType> CodeGen::generateStructLit(StructLit* node) {
    // Buscar la declaracion del struct
    auto it = structs.find(node->structName);
    if (it == structs.end()) {
        throw std::runtime_error(
            "Struct '" + node->structName + "' no definido en linea " +
            std::to_string(node->line));
    }
    auto& fieldNames = it->second.fieldNames;
    auto numFields = fieldNames.size();

    auto* i64Ty = llvm::Type::getInt64Ty(context);

    // Crear el struct (malloc + inicializacion)
    auto* structVal = builder.CreateCall(
        llvm::FunctionCallee(structCreateFunc),
        {llvm::ConstantInt::get(i64Ty, (uint64_t)numFields)},
        "struct");

    // Map field name → index for quick lookup
    std::map<std::string, size_t> fieldIdx;
    for (size_t i = 0; i < fieldNames.size(); i++) {
        fieldIdx[fieldNames[i]] = i;
    }

    // Para cada campo pasado en el literal, buscar su indice y setearlo
    for (size_t i = 0; i < node->fieldNames.size(); i++) {
        const std::string& fname = node->fieldNames[i];
        auto fit = fieldIdx.find(fname);
        if (fit == fieldIdx.end()) {
            throw std::runtime_error(
                "Struct '" + node->structName + "' no tiene campo '" + fname +
                "' en linea " + std::to_string(node->line));
        }
        size_t idx = fit->second;
        auto [val, valType] = generateExpr(node->fieldValues[i].get());
        auto* boxed = boxValue(val, valType);
        builder.CreateCall(llvm::FunctionCallee(structSetFunc),
            {structVal, llvm::ConstantInt::get(i64Ty, (uint64_t)idx), boxed});
    }

    return {structVal, ZenType::Struct};
}

std::pair<llvm::Value*, ZenType> CodeGen::generateMemberAccess(MemberAccess* node) {
    // Generar la base (el struct)
    auto [baseVal, baseType] = generateExpr(node->base.get());

    if (baseType != ZenType::Struct) {
        throw std::runtime_error(
            "No se puede acceder a miembro de un valor que no es struct en linea " +
            std::to_string(node->line));
    }

    // Necesitamos saber el tipo del struct para buscar el indice del campo.
    // Como no guardamos el tipo del struct en runtime, tenemos que inferirlo del AST.
    // Por ahora, no podemos — así que iteramos por todos los structs registrados
    // y usamos el primero que tenga ese campo. (Mejor: guardar el nombre del struct
    // en el ZenType, pero eso requiere refactoring.)
    //
    // WORKAROUND: el usuario debe pasar el struct por una variable tipada.
    // Como todos los structs tienen los campos como i8*, podemos acceder a
    // cualquier indice. Necesitamos saber el INDICE del campo "node->member".
    // Por ahora, recorremos todos los structs y si alguno tiene el campo, lo usamos.
    //
    // TODO: mejorar esto guardando el tipo del struct en VarInfo.

    size_t idx = (size_t)-1;
    for (auto& [name, info] : structs) {
        for (size_t i = 0; i < info.fieldNames.size(); i++) {
            if (info.fieldNames[i] == node->member) {
                idx = i;
                break;
            }
        }
        if (idx != (size_t)-1) break;
    }

    if (idx == (size_t)-1) {
        throw std::runtime_error(
            "Ningun struct tiene campo '" + node->member + "' en linea " +
            std::to_string(node->line));
    }

    auto* i64Ty = llvm::Type::getInt64Ty(context);
    auto* value = builder.CreateCall(
        llvm::FunctionCallee(structGetFunc),
        {baseVal, llvm::ConstantInt::get(i64Ty, (uint64_t)idx)},
        "member");

    // Devolvemos como String (el usuario puede convertir con numero() si quiere)
    return {value, ZenType::String};
}

void CodeGen::generateMemberAssign(MemberAssign* node) {
    auto [baseVal, baseType] = generateExpr(node->base.get());

    if (baseType != ZenType::Struct) {
        throw std::runtime_error(
            "No se puede asignar miembro de un valor que no es struct en linea " +
            std::to_string(node->line));
    }

    // Buscar indice del campo (mismo workaround que en generateMemberAccess)
    size_t idx = (size_t)-1;
    for (auto& [name, info] : structs) {
        for (size_t i = 0; i < info.fieldNames.size(); i++) {
            if (info.fieldNames[i] == node->member) {
                idx = i;
                break;
            }
        }
        if (idx != (size_t)-1) break;
    }

    if (idx == (size_t)-1) {
        throw std::runtime_error(
            "Ningun struct tiene campo '" + node->member + "' en linea " +
            std::to_string(node->line));
    }

    auto [val, valType] = generateExpr(node->value.get());
    auto* boxed = boxValue(val, valType);

    auto* i64Ty = llvm::Type::getInt64Ty(context);
    builder.CreateCall(llvm::FunctionCallee(structSetFunc),
        {baseVal, llvm::ConstantInt::get(i64Ty, (uint64_t)idx), boxed});
}

// ============================================================
// Match/caso
// ============================================================

void CodeGen::generateMatch(MatchStmt* node) {
    // Generar la expr a comparar
    auto [subjectVal, subjectType] = generateExpr(node->subject.get());

    // Si es string, convertir a i8* (por si era otra cosa)
    // En realidad, lo dejamos como está y comparamos con cada caso.

    auto* func = currentFunction;
    // Crear todos los basic blocks: uno por caso + un default (si hay) + merge
    std::vector<llvm::BasicBlock*> caseBBs;
    std::vector<llvm::BasicBlock*> bodyBBs;
    llvm::BasicBlock* defaultBB = nullptr;
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "match.end", func);

    // Default puede ir en cualquier posicion, pero tipicamente al final.
    // Lo separamos: si un caso es "por_defecto", lo mandamos al defaultBB.
    // Pero seguimos iterando en orden: si un caso NO es default y coincide,
    // vamos a su body. Si ninguno coincide, vamos al default (que puede estar
    // en cualquier posicion).
    // Para simplificar: creamos BBs para cada caso en orden.
    // El primer caso que coincida se ejecuta. Si ninguno, se ejecuta el default.

    // Primero, crear el default BB (si hay)
    bool hasDefault = false;
    for (auto& c : node->cases) {
        if (c.isDefault) {
            hasDefault = true;
            defaultBB = llvm::BasicBlock::Create(context, "match.default", func);
            break;
        }
    }

    // Crear BBs para cada caso (en orden)
    for (size_t i = 0; i < node->cases.size(); i++) {
        if (node->cases[i].isDefault) continue;
        caseBBs.push_back(llvm::BasicBlock::Create(context, "match.cond." + std::to_string(i), func));
        bodyBBs.push_back(llvm::BasicBlock::Create(context, "match.body." + std::to_string(i), func));
    }

    // Comenzar las comparaciones en cadena
    // Para cada caso no-default:
    //   entry → cond_i: if (subject == case_i_value) goto body_i else goto cond_{i+1}
    // Si no hay mas casos, ir al default (si hay) o al merge.

    // entry → primer cond (si hay), sino default/merge
    if (!caseBBs.empty()) {
        builder.CreateBr(caseBBs[0]);
    } else if (defaultBB) {
        builder.CreateBr(defaultBB);
    } else {
        builder.CreateBr(mergeBB);
    }

    // Generar cada cond + body
    size_t bodyIdx = 0;
    for (size_t i = 0; i < node->cases.size(); i++) {
        auto& c = node->cases[i];
        if (c.isDefault) continue;

        // cond_i: comparar subject con c.value
        builder.SetInsertPoint(caseBBs[bodyIdx]);

        // Generar el valor del caso
        auto [caseVal, caseType] = generateExpr(c.value.get());

        // Comparar segun el tipo
        llvm::Value* eqVal;
        if (subjectType == ZenType::String && caseType == ZenType::String) {
            // strcmp
            auto* cmpRes = builder.CreateCall(
                llvm::FunctionCallee(strcmpFunc), {subjectVal, caseVal}, "scmp");
            eqVal = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, cmpRes,
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0), "eq");
        } else if (subjectType == ZenType::String || caseType == ZenType::String) {
            // Uno es string, otro no. Convertir ambos a string y comparar.
            auto* s1 = toString(subjectVal, subjectType);
            auto* s2 = toString(caseVal, caseType);
            auto* cmpRes = builder.CreateCall(
                llvm::FunctionCallee(strcmpFunc), {s1, s2}, "scmp");
            eqVal = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, cmpRes,
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0), "eq");
        } else if (subjectType == ZenType::Number && caseType == ZenType::Number) {
            eqVal = builder.CreateFCmp(llvm::CmpInst::FCMP_OEQ, subjectVal, caseVal, "eq");
        } else if (subjectType == ZenType::Bool && caseType == ZenType::Bool) {
            eqVal = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, subjectVal, caseVal, "eq");
        } else {
            // Tipos mixtos, convertir a string y comparar
            auto* s1 = toString(subjectVal, subjectType);
            auto* s2 = toString(caseVal, caseType);
            auto* cmpRes = builder.CreateCall(
                llvm::FunctionCallee(strcmpFunc), {s1, s2}, "scmp");
            eqVal = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, cmpRes,
                llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0), "eq");
        }

        // Si coincide, ir al body; sino, ir al siguiente cond (o default/merge)
        llvm::BasicBlock* nextBB;
        if (bodyIdx + 1 < caseBBs.size()) {
            nextBB = caseBBs[bodyIdx + 1];
        } else if (defaultBB) {
            nextBB = defaultBB;
        } else {
            nextBB = mergeBB;
        }
        builder.CreateCondBr(eqVal, bodyBBs[bodyIdx], nextBB);

        // body_i
        builder.SetInsertPoint(bodyBBs[bodyIdx]);
        for (auto& stmt : c.body) {
            generateNodeInLoop(stmt.get());
            if (builder.GetInsertBlock()->getTerminator()) break;
        }
        if (!builder.GetInsertBlock()->getTerminator()) {
            builder.CreateBr(mergeBB);
        }

        bodyIdx++;
    }

    // default
    if (defaultBB) {
        builder.SetInsertPoint(defaultBB);
        for (auto& c : node->cases) {
            if (!c.isDefault) continue;
            for (auto& stmt : c.body) {
                generateNodeInLoop(stmt.get());
                if (builder.GetInsertBlock()->getTerminator()) break;
            }
            break;
        }
        if (!builder.GetInsertBlock()->getTerminator()) {
            builder.CreateBr(mergeBB);
        }
    }

    // merge
    builder.SetInsertPoint(mergeBB);
}

// ============================================================
// FFI - Funciones externas de C
// ============================================================

// Convierte un CType a llvm::Type*
static llvm::Type* cTypeToLLVM(CType t, llvm::LLVMContext& ctx) {
    switch (t) {
        case CType::Void:   return llvm::Type::getVoidTy(ctx);
        case CType::Int:    return llvm::Type::getInt32Ty(ctx);
        case CType::UInt:   return llvm::Type::getInt32Ty(ctx);
        case CType::Long:   return llvm::Type::getInt64Ty(ctx);
        case CType::Float:  return llvm::Type::getFloatTy(ctx);
        case CType::Double: return llvm::Type::getDoubleTy(ctx);
        case CType::Ptr:    return llvm::Type::getInt8PtrTy(ctx);
        case CType::Str:    return llvm::Type::getInt8PtrTy(ctx);
        case CType::Char:   return llvm::Type::getInt8Ty(ctx);
    }
    return llvm::Type::getInt32Ty(ctx);
}

void CodeGen::registerExternDecl(ExternDecl* node) {
    // Construir el tipo de la funcion
    std::vector<llvm::Type*> paramTypes;
    for (auto& p : node->params) {
        if (p.isPointer) {
            // Puntero al tipo base
            paramTypes.push_back(cTypeToLLVM(p.type, context)->getPointerTo());
        } else {
            paramTypes.push_back(cTypeToLLVM(p.type, context));
        }
    }
    llvm::Type* retType;
    if (node->returnIsPointer) {
        retType = cTypeToLLVM(node->returnType, context)->getPointerTo();
    } else {
        retType = cTypeToLLVM(node->returnType, context);
    }

    auto* funcType = llvm::FunctionType::get(retType, paramTypes, false);

    // Reusar la funcion si ya existe en el modulo (ej: pow ya esta declarado por Zen)
    llvm::Function* func = module->getFunction(node->funcName);
    if (!func) {
        func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage, node->funcName, module.get());
    }

    ExternInfo info;
    info.func = func;
    info.returnType = node->returnType;
    info.returnIsPointer = node->returnIsPointer;
    info.params = node->params;
    externs[node->funcName] = info;

    // Registrar la libreria para pasarla al linker
    // Evitar duplicados
    bool found = false;
    for (const auto& lib : requiredLibs) {
        if (lib == node->libName) { found = true; break; }
    }
    if (!found) {
        requiredLibs.push_back(node->libName);
    }
}

std::pair<llvm::Value*, ZenType> CodeGen::generateExternCall(ExternCall* node) {
    auto it = externs.find(node->funcName);
    if (it == externs.end()) {
        throw std::runtime_error(
            "Funcion externa '" + node->funcName + "' no declarada en linea " +
            std::to_string(node->line));
    }

    auto& info = it->second;
    auto* func = info.func;

    if (node->args.size() != info.params.size()) {
        throw std::runtime_error(
            "Funcion externa '" + node->funcName + "' espera " +
            std::to_string(info.params.size()) + " argumentos pero se pasaron " +
            std::to_string(node->args.size()) + " en linea " +
            std::to_string(node->line));
    }

    // Convertir cada argumento al tipo C correcto
    std::vector<llvm::Value*> args;
    for (size_t i = 0; i < node->args.size(); i++) {
        auto [val, type] = generateExpr(node->args[i].get());
        auto& paramInfo = info.params[i];
        llvm::Type* targetType;
        if (paramInfo.isPointer) {
            targetType = cTypeToLLVM(paramInfo.type, context)->getPointerTo();
        } else {
            targetType = cTypeToLLVM(paramInfo.type, context);
        }

        llvm::Value* converted = val;
        // Conversiones basicas
        if (paramInfo.type == CType::Str) {
            // Aceptamos string directamente, o convertimos
            converted = toString(val, type);
        } else if (paramInfo.type == CType::Double) {
            if (type != ZenType::Number) {
                converted = toDouble(val, type);
            }
        } else if (paramInfo.type == CType::Float) {
            auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
            converted = builder.CreateFPTrunc(d, llvm::Type::getFloatTy(context), "tof32");
        } else if (paramInfo.type == CType::Int) {
            auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
            converted = builder.CreateFPToSI(d, llvm::Type::getInt32Ty(context), "toi32");
        } else if (paramInfo.type == CType::UInt) {
            auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
            converted = builder.CreateFPToUI(d, llvm::Type::getInt32Ty(context), "tou32");
        } else if (paramInfo.type == CType::Long) {
            auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
            converted = builder.CreateFPToSI(d, llvm::Type::getInt64Ty(context), "toi64");
        } else if (paramInfo.type == CType::Char) {
            auto* d = (type == ZenType::Number) ? val : toDouble(val, type);
            converted = builder.CreateFPToSI(d, llvm::Type::getInt8Ty(context), "toi8");
        } else if (paramInfo.type == CType::Ptr) {
            // Aceptamos string como void*
            converted = toString(val, type);
        }
        args.push_back(converted);
    }

    auto* result = builder.CreateCall(llvm::FunctionCallee(func), args, "c_call");

    // Convertir el resultado de vuelta a ZenType
    ZenType zenType;
    llvm::Value* zenVal = result;
    switch (info.returnType) {
        case CType::Void:
            return {llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0), ZenType::Number};
        case CType::Int:
        case CType::Long:
            zenType = ZenType::Number;
            zenVal = builder.CreateSIToFP(result, llvm::Type::getDoubleTy(context), "c2d");
            break;
        case CType::UInt:
            zenType = ZenType::Number;
            zenVal = builder.CreateUIToFP(result, llvm::Type::getDoubleTy(context), "u2d");
            break;
        case CType::Float:
            zenType = ZenType::Number;
            zenVal = builder.CreateFPExt(result, llvm::Type::getDoubleTy(context), "f2d");
            break;
        case CType::Double:
            zenType = ZenType::Number;
            break;
        case CType::Str:
            zenType = ZenType::String;
            break;
        case CType::Ptr:
            zenType = ZenType::String;  // ptr se trata como string por ahora
            break;
        case CType::Char:
            zenType = ZenType::Number;
            zenVal = builder.CreateSIToFP(result, llvm::Type::getDoubleTy(context), "c2d");
            break;
    }
    return {zenVal, zenType};
}

// Normaliza un nombre de builtin a minusculas sin acentos.
// Permite escribir "Longitud", "LONGITUD", "longitud", "mayúsculas", etc.
static std::string normalizeBuiltinName(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c >= 'A' && c <= 'Z') {
            out += (char)(c - 'A' + 'a');
        } else if (c == (char)0xC3) {
            // UTF-8: primer byte de un acento — lo saltamos
            continue;
        } else if (c == (char)0xA1) {  // ¡
            out += 'i';
        } else if (c == (char)0xA9) {  // é
            out += 'e';
        } else if (c == (char)0xAD) {  // í
            out += 'i';
        } else if (c == (char)0xB3) {  // ó
            out += 'o';
        } else if (c == (char)0xBA) {  // ú
            out += 'u';
        } else if (c == (char)0xB1) {  // ñ
            out += 'n';
        } else if (c == (char)0x81) {  // Á (UTF-8 0xC3 0x81)
            out += 'a';
        } else if (c == (char)0x89) {  // É
            out += 'e';
        } else if (c == (char)0x8D) {  // Í
            out += 'i';
        } else if (c == (char)0x93) {  // Ó
            out += 'o';
        } else if (c == (char)0x9A) {  // Ú
            out += 'u';
        } else if (c == (char)0x91) {  // Ñ
            out += 'n';
        } else {
            out += c;
        }
    }
    return out;
}

std::pair<llvm::Value*, ZenType> CodeGen::tryBuiltinCall(FuncCall* node) {
    std::string name = normalizeBuiltinName(node->name);

    auto* i8Ptr = llvm::Type::getInt8PtrTy(context);
    auto* i32Ty = llvm::Type::getInt32Ty(context);
    auto* i64Ty = llvm::Type::getInt64Ty(context);
    auto* doubleTy = llvm::Type::getDoubleTy(context);

    // Helper para obtener un argumento de tipo string
    auto getStrArg = [&](size_t i) -> llvm::Value* {
        auto [v, t] = generateExpr(node->args[i].get());
        return toString(v, t);
    };
    // Helper para obtener un argumento numerico
    auto getNumArg = [&](size_t i) -> llvm::Value* {
        auto [v, t] = generateExpr(node->args[i].get());
        if (v->getType() == llvm::Type::getInt8PtrTy(context))
            return toDouble(v, ZenType::String);
        return (t == ZenType::Number) ? v : toDouble(v, t);
    };

        // ---- longitud(x) -> int  (longitud de un string o lista)
    if (name == "longitud" || name == "length" || name == "len") {
        if (node->args.size() != 1)
            throw std::runtime_error("longitud() espera 1 argumento en linea " +
                std::to_string(node->line));
        // Generar el argumento sin convertir - necesitamos saber su tipo
        auto [val, t] = generateExpr(node->args[0].get());
        if (t == ZenType::List) {
            // list_len(list) -> i64
            auto* len = builder.CreateCall(llvm::FunctionCallee(listLenFunc), {val}, "len");
            auto* asDouble = builder.CreateUIToFP(len, doubleTy, "lend");
            return {asDouble, ZenType::Number};
        }
        // Sino, tratar como string
        auto* s = toString(val, t);
        auto* len = builder.CreateCall(llvm::FunctionCallee(strlenFunc), {s}, "len");
        auto* asDouble = builder.CreateUIToFP(len, doubleTy, "lend");
        return {asDouble, ZenType::Number};
    }

    // ---- char_at(string, indice) -> numero
    //     Devuelve el código ASCII del carácter en la posición indicada.
    //     char_at("hola", 0) -> 104 (ASCII de 'h')
    //     Necesario para Zen sobre Zen: el lexer necesita leer caracteres.
    if (name == "char_at" || name == "char_codigo" || name == "char_code" ||
        name == "ascii_de") {
        if (node->args.size() != 2)
            throw std::runtime_error("char_at() espera 2 argumentos (string, indice) en linea " +
                std::to_string(node->line));
        auto* s = getStrArg(0);
        auto* idx = getNumArg(1);
        auto* idxI64 = builder.CreateFPToSI(idx, i64Ty, "idx");
        // s[idx] - usar GEP para obtener puntero al byte
        auto* ptr = builder.CreateGEP(
            llvm::Type::getInt8Ty(context), s, {idxI64}, "charptr");
        auto* ch = builder.CreateLoad(
            llvm::Type::getInt8Ty(context), ptr, "ch");
        auto* asDouble = builder.CreateSIToFP(ch, doubleTy, "chd");
        return {asDouble, ZenType::Number};
    }

    // ---- char_to_str(numero) -> string
    //     Convierte un código ASCII a un string de 1 carácter.
    //     char_to_str(104) -> "h"
    //     Necesario para Zen sobre Zen: el lexer necesita construir strings.
    if (name == "char_to_str" || name == "caracter" || name == "chr" ||
        name == "desde_ascii") {
        if (node->args.size() != 1)
            throw std::runtime_error("char_to_str() espera 1 argumento (codigo ASCII) en linea " +
                std::to_string(node->line));
        auto* n = getNumArg(0);
        auto* asI8 = builder.CreateFPToSI(n,
            llvm::Type::getInt8Ty(context), "chr");
        // Allocate 2 bytes (char + null terminator)
        auto* buf = builder.CreateCall(mallocFunc,
            llvm::ConstantInt::get(i64Ty, 2), "chrbuf");
        // Store the character
        builder.CreateStore(asI8, buf);
        // Store null terminator at buf[1]
        auto* nullPtr = builder.CreateGEP(
            llvm::Type::getInt8Ty(context), buf,
            {llvm::ConstantInt::get(i64Ty, 1)}, "nullptr");
        builder.CreateStore(
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 0),
            nullPtr);
        return {buf, ZenType::String};
    }

    // ---- subtexto(s, desde, hasta) -> string  (hasta exclusivo)
    //     subtexto("hola mundo", 0, 4) -> "hola"
    if (name == "subtexto" || name == "substring" || name == "substr" ||
        name == "slice" || name == "sub") {
        if (node->args.size() != 3)
            throw std::runtime_error("subtexto() espera 3 argumentos (s, desde, hasta) en linea " +
                std::to_string(node->line));
        auto* s = getStrArg(0);
        auto* desde = getNumArg(1);
        auto* hasta = getNumArg(2);

        // Convertir desde y hasta a i64
        auto* desdeI64 = builder.CreateFPToSI(desde, i64Ty, "desde");
        auto* hastaI64 = builder.CreateFPToSI(hasta, i64Ty, "hasta");
        // Calcular longitud a copiar
        auto* len = builder.CreateSub(hastaI64, desdeI64, "sublen");
        // Truncar a positivo
        auto* zero = llvm::ConstantInt::get(i64Ty, 0);
        auto* isNeg = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, len, zero, "neg");
        len = builder.CreateSelect(isNeg, zero, len, "sublenp");

        // malloc(len+1)
        auto* sizeWithNull = builder.CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1), "sz");
        auto* buf = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {sizeWithNull}, "buf");

        // Calcular puntero origen = s + desde
        auto* src = builder.CreateInBoundsGEP(i8Ptr->getPointerElementType(), s, desdeI64, "src");
        // strncpy(buf, src, len)
        builder.CreateCall(llvm::FunctionCallee(strncpyFunc), {buf, src, len});
        // Poner null terminator en buf[len]
        auto* nullPos = builder.CreateInBoundsGEP(
            llvm::Type::getInt8Ty(context), buf, len, "npos");
        builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 0), nullPos);

        return {buf, ZenType::String};
    }

    // ---- buscar(s, sub) -> int  (índice de la primera aparición, o -1)
    if (name == "buscar" || name == "find" || name == "index" || name == "indexOf") {
        if (node->args.size() != 2)
            throw std::runtime_error("buscar() espera 2 argumentos (s, sub) en linea " +
                std::to_string(node->line));
        auto* s = getStrArg(0);
        auto* sub = getStrArg(1);

        auto* found = builder.CreateCall(llvm::FunctionCallee(strstrFunc), {s, sub}, "found");
        // Si found == null, retornar -1
        auto* isNull = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, found,
            llvm::ConstantPointerNull::get(i8Ptr), "isnull");
        auto* negOne = llvm::ConstantFP::get(doubleTy, -1.0);
        // Si no es null, restar found - s
        auto* diff = builder.CreatePtrToInt(found, i64Ty, "fi");
        auto* si = builder.CreatePtrToInt(s, i64Ty, "si");
        auto* idx = builder.CreateSub(diff, si, "idx");
        auto* idxD = builder.CreateUIToFP(idx, doubleTy, "idxd");
        auto* result = builder.CreateSelect(isNull, negOne, idxD, "buscar_r");
        return {result, ZenType::Number};
    }

    // ---- reemplazar(s, viejo, nuevo) -> string
    // Reemplaza la PRIMERA aparición de "viejo" por "nuevo".
    // Para reemplazar todas las apariciones se necesita un bucle,
    // que mejor hacemos como runtime en C cuando agreguemos runtime.c.
    // FIXME: por ahora solo reemplaza la primera aparición.
    if (name == "reemplazar" || name == "replace") {
        if (node->args.size() != 3)
            throw std::runtime_error("reemplazar() espera 3 argumentos (s, viejo, nuevo) en linea " +
                std::to_string(node->line));
        auto* s = getStrArg(0);
        auto* viejo = getStrArg(1);
        auto* nuevo = getStrArg(2);

        // Encontrar la primera aparición
        auto* found = builder.CreateCall(llvm::FunctionCallee(strstrFunc), {s, viejo}, "found");
        auto* isNull = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, found,
            llvm::ConstantPointerNull::get(i8Ptr), "isnull");

        // Si se encuentra, construir: s[0:idx] + nuevo + s[idx+len(viejo):]
        auto* sI = builder.CreatePtrToInt(s, i64Ty, "si");
        auto* fI = builder.CreatePtrToInt(found, i64Ty, "fi");
        auto* idx = builder.CreateSub(fI, sI, "idx");
        auto* viejoLen = builder.CreateCall(llvm::FunctionCallee(strlenFunc), {viejo}, "vlen");
        auto* restStart = builder.CreateAdd(idx, viejoLen, "rest");
        auto* nuevoLen = builder.CreateCall(llvm::FunctionCallee(strlenFunc), {nuevo}, "nlen");
        auto* sLen = builder.CreateCall(llvm::FunctionCallee(strlenFunc), {s}, "slen");
        auto* restLen = builder.CreateSub(sLen, restStart, "restlen");
        auto* totalLen = builder.CreateAdd(idx, nuevoLen, "tl1");
        totalLen = builder.CreateAdd(totalLen, restLen, "tl2");
        auto* totalSize = builder.CreateAdd(totalLen, llvm::ConstantInt::get(i64Ty, 1), "ts");

        auto* buf = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {totalSize}, "buf");

        // Copiar primera parte: s[0:idx] con memcpy (no strncpy para no requerir null)
        builder.CreateCall(llvm::FunctionCallee(strncpyFunc), {buf, s, idx});

        // memcpy(buf + idx, nuevo, nuevoLen)
        auto* bufAtIdx = builder.CreateInBoundsGEP(
            llvm::Type::getInt8Ty(context), buf, idx, "bidx");
        builder.CreateCall(llvm::FunctionCallee(memcpyFunc), {bufAtIdx, nuevo, nuevoLen});

        // memcpy(buf + idx + nuevoLen, s + restStart, restLen)
        auto* idxPlusNlen = builder.CreateAdd(idx, nuevoLen, "ipn");
        auto* bufAt2 = builder.CreateInBoundsGEP(
            llvm::Type::getInt8Ty(context), buf, idxPlusNlen, "b2");
        auto* srcAtRest = builder.CreateInBoundsGEP(
            llvm::Type::getInt8Ty(context), s, restStart, "sr");
        builder.CreateCall(llvm::FunctionCallee(memcpyFunc), {bufAt2, srcAtRest, restLen});

        // Null terminator en buf[totalLen]
        auto* endPos = builder.CreateInBoundsGEP(
            llvm::Type::getInt8Ty(context), buf, totalLen, "end");
        builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 0), endPos);

        // Si no se encontró, devolver s; si se encontró, devolver buf
        auto* result = builder.CreateSelect(isNull, s, buf, "repl_r");
        return {result, ZenType::String};
    }

    // ---- mayusculas(s) / minusculas(s) -> string
    // Implementación: malloc(strlen(s)+1), recorrer y aplicar toupper/tolower.
    // Como no podemos hacer bucles en una sola expresión, generamos un bucle
    // con BasicBlocks.
    if (name == "mayusculas" || name == "uppercase" || name == "upper" ||
        name == "minusculas" || name == "lowercase" || name == "lower") {
        if (node->args.size() != 1)
            throw std::runtime_error("mayusculas()/minusculas() espera 1 argumento en linea " +
                std::to_string(node->line));

        bool toUpper = (name == "mayusculas" || name == "uppercase" || name == "upper");
        auto* fn = toUpper ? toupperFunc : tolowerFunc;

        auto* s = getStrArg(0);
        auto* sLen = builder.CreateCall(llvm::FunctionCallee(strlenFunc), {s}, "slen");
        auto* size = builder.CreateAdd(sLen, llvm::ConstantInt::get(i64Ty, 1), "size");
        auto* buf = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {size}, "buf");

        // Bucle: para i desde 0 hasta sLen, buf[i] = toupper(s[i])
        auto* func = currentFunction;
        auto* condBB = llvm::BasicBlock::Create(context, "case.cond", func);
        auto* bodyBB = llvm::BasicBlock::Create(context, "case.body", func);
        auto* endBB  = llvm::BasicBlock::Create(context, "case.end", func);

        // iAlloca
        auto* iAlloca = builder.CreateAlloca(i64Ty, nullptr, "i");
        builder.CreateStore(llvm::ConstantInt::get(i64Ty, 0), iAlloca);
        builder.CreateBr(condBB);

        // cond
        builder.SetInsertPoint(condBB);
        auto* iVal = builder.CreateLoad(i64Ty, iAlloca, "i");
        auto* cmp = builder.CreateICmp(llvm::CmpInst::ICMP_SLT, iVal, sLen, "cmp");
        builder.CreateCondBr(cmp, bodyBB, endBB);

        // body
        builder.SetInsertPoint(bodyBB);
        auto* srcPtr = builder.CreateInBoundsGEP(llvm::Type::getInt8Ty(context), s, iVal, "sp");
        auto* dstPtr = builder.CreateInBoundsGEP(llvm::Type::getInt8Ty(context), buf, iVal, "dp");
        auto* ch = builder.CreateLoad(llvm::Type::getInt8Ty(context), srcPtr, "ch");
        auto* chAsI32 = builder.CreateSExt(ch, i32Ty, "ch32");
        auto* conv = builder.CreateCall(llvm::FunctionCallee(fn), {chAsI32}, "conv");
        auto* convAsI8 = builder.CreateTrunc(conv, llvm::Type::getInt8Ty(context), "conv8");
        builder.CreateStore(convAsI8, dstPtr);
        auto* next = builder.CreateAdd(iVal, llvm::ConstantInt::get(i64Ty, 1), "next");
        builder.CreateStore(next, iAlloca);
        builder.CreateBr(condBB);

        // end
        builder.SetInsertPoint(endBB);
        // Null terminator
        auto* nullPos = builder.CreateInBoundsGEP(llvm::Type::getInt8Ty(context), buf, sLen, "np");
        builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 0), nullPos);

        return {buf, ZenType::String};
    }

    // ---- recortar(s) -> string  (quitar espacios al inicio)
    // Versión simple: recorta todos los espacios al inicio del string.
    // TODO: también recortar al final cuando tengamos runtime en C.
    if (name == "recortar" || name == "trim" || name == "ltrim") {
        if (node->args.size() != 1)
            throw std::runtime_error("recortar() espera 1 argumento en linea " +
                std::to_string(node->line));
        auto* s = getStrArg(0);

        // Implementación con bucle: mientras s[0] == ' ', avanzar s++
        auto* func = currentFunction;
        auto* condBB = llvm::BasicBlock::Create(context, "trim.cond", func);
        auto* bodyBB = llvm::BasicBlock::Create(context, "trim.body", func);
        auto* endBB  = llvm::BasicBlock::Create(context, "trim.end", func);

        // Empezar con s
        auto* sAlloca = builder.CreateAlloca(i8Ptr, nullptr, "s.ptr");
        builder.CreateStore(s, sAlloca);
        builder.CreateBr(condBB);

        // cond: si *s != ' ' o *s == 0, salir
        builder.SetInsertPoint(condBB);
        auto* curS = builder.CreateLoad(i8Ptr, sAlloca, "s");
        auto* firstCh = builder.CreateLoad(llvm::Type::getInt8Ty(context), curS, "ch");
        auto* isSpace = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, firstCh,
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), ' '), "sp");
        builder.CreateCondBr(isSpace, bodyBB, endBB);

        // body: s++
        builder.SetInsertPoint(bodyBB);
        auto* nextS = builder.CreateInBoundsGEP(
            llvm::Type::getInt8Ty(context), curS, llvm::ConstantInt::get(i64Ty, 1), "next");
        builder.CreateStore(nextS, sAlloca);
        builder.CreateBr(condBB);

        // end
        builder.SetInsertPoint(endBB);
        auto* result = builder.CreateLoad(i8Ptr, sAlloca, "trimmed");
        return {result, ZenType::String};
    }

    // ---- numero(s) -> number  (convierte string a número)
    if (name == "numero" || name == "number" || name == "num" || name == "tonum") {
        if (node->args.size() != 1)
            throw std::runtime_error("numero() espera 1 argumento en linea " +
                std::to_string(node->line));
        auto* s = getStrArg(0);
        // strtod(s, NULL)
        auto* nullPtr = llvm::ConstantPointerNull::get(
            llvm::Type::getInt8PtrTy(context)->getPointerTo());
        auto* result = builder.CreateCall(
            llvm::FunctionCallee(strtodFunc), {s, nullPtr}, "num");
        return {result, ZenType::Number};
    }

    // ---- texto(n) -> string  (convierte número a string)
    if (name == "texto" || name == "text" || name == "str" || name == "tostr") {
        if (node->args.size() != 1)
            throw std::runtime_error("texto() espera 1 argumento en linea " +
                std::to_string(node->line));
        auto [v, t] = generateExpr(node->args[0].get());
        if (t == ZenType::String) return {v, ZenType::String};
        auto* s = toString(v, t);
        return {s, ZenType::String};
    }

    // ---- I/O de archivos ----

    // leer_archivo(ruta) -> texto
    //   Lee un archivo completo y devuelve su contenido como string.
    //   Si no puede abrirlo, devuelve "".
    if (name == "leer_archivo" || name == "read_file" || name == "readfile") {
        if (node->args.size() != 1)
            throw std::runtime_error("leer_archivo() espera 1 argumento (ruta) en linea " +
                std::to_string(node->line));
        auto* path = getStrArg(0);
        auto* mode = builder.CreateGlobalStringPtr("rb");

        // FILE* f = fopen(path, "rb")
        auto* file = builder.CreateCall(llvm::FunctionCallee(fopenFunc), {path, mode}, "file");

        // Si file == NULL, devolver ""
        auto* isNull = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, file,
            llvm::ConstantPointerNull::get(i8Ptr), "isnull");
        auto* emptyStr = builder.CreateGlobalStringPtr("");

        // Bloques: si es null vamos directo al end, sino vamos a openBB
        auto* func = currentFunction;
        auto* curBB = builder.GetInsertBlock();  // bloque actual (donde se evalua isNull)
        auto* openBB = llvm::BasicBlock::Create(context, "rf.open", func);
        auto* endBB  = llvm::BasicBlock::Create(context, "rf.end", func);
        builder.CreateCondBr(isNull, endBB, openBB);

        // open: fseek(f, 0, SEEK_END); size = ftell(f); fseek(f, 0, SEEK_SET)
        builder.SetInsertPoint(openBB);
        auto* seekEnd = llvm::ConstantInt::get(i32Ty, 2);  // SEEK_END
        builder.CreateCall(llvm::FunctionCallee(fseekFunc), {file, llvm::ConstantInt::get(i64Ty, 0), seekEnd});
        auto* size = builder.CreateCall(llvm::FunctionCallee(ftellFunc), {file}, "size");
        auto* seekSet = llvm::ConstantInt::get(i32Ty, 0);  // SEEK_SET
        builder.CreateCall(llvm::FunctionCallee(fseekFunc), {file, llvm::ConstantInt::get(i64Ty, 0), seekSet});
        // malloc(size + 1)
        auto* sizePlus1 = builder.CreateAdd(size, llvm::ConstantInt::get(i64Ty, 1), "sp1");
        auto* buf = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {sizePlus1}, "buf");
        // fread(buf, 1, size, f)
        builder.CreateCall(llvm::FunctionCallee(freadFunc),
            {buf, llvm::ConstantInt::get(i64Ty, 1), size, file});
        // buf[size] = 0 (null terminator)
        auto* endPos = builder.CreateInBoundsGEP(llvm::Type::getInt8Ty(context), buf, size, "ep");
        builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 0), endPos);
        // fclose(f)
        builder.CreateCall(llvm::FunctionCallee(fcloseFunc), {file});
        auto* openBBEnd = builder.GetInsertBlock();  // puede ser != openBB si hubo cambios
        builder.CreateBr(endBB);

        // end: PHI entre emptyStr (de curBB) y buf (de openBBEnd)
        builder.SetInsertPoint(endBB);
        auto* phi = builder.CreatePHI(i8Ptr, 2, "rf_result");
        phi->addIncoming(emptyStr, curBB);
        phi->addIncoming(buf, openBBEnd);
        return {phi, ZenType::String};
    }

    // escribir_archivo(ruta, contenido) -> numero
    //   Escribe contenido a un archivo. Devuelve 1 si OK, 0 si fallo.
    if (name == "escribir_archivo" || name == "write_file" || name == "writefile") {
        if (node->args.size() != 2)
            throw std::runtime_error("escribir_archivo() espera 2 argumentos (ruta, contenido) en linea " +
                std::to_string(node->line));
        auto* path = getStrArg(0);
        auto* content = getStrArg(1);
        auto* mode = builder.CreateGlobalStringPtr("wb");

        auto* file = builder.CreateCall(llvm::FunctionCallee(fopenFunc), {path, mode}, "file");
        auto* isNull = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, file,
            llvm::ConstantPointerNull::get(i8Ptr), "isnull");

        // Si es null, devolver 0
        auto* func = currentFunction;
        auto* curBB = builder.GetInsertBlock();
        auto* writeBB = llvm::BasicBlock::Create(context, "wf.write", func);
        auto* endBB  = llvm::BasicBlock::Create(context, "wf.end", func);
        builder.CreateCondBr(isNull, endBB, writeBB);

        // write: len = strlen(content); fwrite(content, 1, len, f); fclose
        builder.SetInsertPoint(writeBB);
        auto* len = builder.CreateCall(llvm::FunctionCallee(strlenFunc), {content}, "len");
        builder.CreateCall(llvm::FunctionCallee(fwriteFunc),
            {content, llvm::ConstantInt::get(i64Ty, 1), len, file});
        builder.CreateCall(llvm::FunctionCallee(fcloseFunc), {file});
        auto* writeBBEnd = builder.GetInsertBlock();
        builder.CreateBr(endBB);

        // end: PHI entre 0.0 (de curBB) y 1.0 (de writeBBEnd)
        builder.SetInsertPoint(endBB);
        auto* zero = llvm::ConstantFP::get(doubleTy, 0.0);
        auto* one  = llvm::ConstantFP::get(doubleTy, 1.0);
        auto* phi = builder.CreatePHI(doubleTy, 2, "wf_result");
        phi->addIncoming(zero, curBB);
        phi->addIncoming(one, writeBBEnd);
        return {phi, ZenType::Number};
    }

    // ---- Funciones del sistema ----

    // salir(codigo)  → termina el programa
    if (name == "salir" || name == "exit") {
        if (node->args.size() != 1)
            throw std::runtime_error("salir() espera 1 argumento (codigo) en linea " +
                std::to_string(node->line));
        auto* code = getNumArg(0);
        auto* codeI32 = builder.CreateFPToSI(code, i32Ty, "code");
        builder.CreateCall(llvm::FunctionCallee(exitFunc), {codeI32});
        // unreachable
        builder.CreateUnreachable();
        return {llvm::ConstantFP::get(doubleTy, 0.0), ZenType::Number};
    }

        // leer_linea() -> texto
    //   Lee una linea de stdin (input interactivo)
    //   Implementacion: lee char por char con getchar() hasta encontrar \n o EOF.
    //   Buffer con grow dinamico.
    if (name == "leer_linea" || name == "read_line" || name == "readline" ||
        name == "leer" || name == "input") {
        if (!node->args.empty())
            throw std::runtime_error("leer_linea() no toma argumentos en linea " +
                std::to_string(node->line));

        auto* func = currentFunction;
        auto* getcharFunc = getOrInsertExtern("getchar", i32Ty, {});

        // Buffer inicial de 256 bytes. Lo guardamos en un alloca para poder actualizarlo.
        auto* initialSize = llvm::ConstantInt::get(i64Ty, 256);
        auto* initialBuf = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {initialSize}, "buf0");
        auto* bufAlloca = builder.CreateAlloca(i8Ptr, nullptr, "buf.ptr");
        builder.CreateStore(initialBuf, bufAlloca);
        auto* capacityAlloca = builder.CreateAlloca(i64Ty, nullptr, "cap");
        builder.CreateStore(initialSize, capacityAlloca);
        auto* lenAlloca = builder.CreateAlloca(i64Ty, nullptr, "len");
        builder.CreateStore(llvm::ConstantInt::get(i64Ty, 0), lenAlloca);

        // Bucle: leer char, si es \n o EOF salir
        auto* condBB = llvm::BasicBlock::Create(context, "ll.cond", func);
        auto* bodyBB = llvm::BasicBlock::Create(context, "ll.body", func);
        auto* growBB = llvm::BasicBlock::Create(context, "ll.grow", func);
        auto* storeBB = llvm::BasicBlock::Create(context, "ll.store", func);
        auto* endBB   = llvm::BasicBlock::Create(context, "ll.end", func);

        builder.CreateBr(condBB);

        // cond: ch = getchar(); si ch == EOF o ch == '\n' salir
        builder.SetInsertPoint(condBB);
        auto* ch = builder.CreateCall(llvm::FunctionCallee(getcharFunc), {}, "ch");
        auto* isEOF = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, ch,
            llvm::ConstantInt::get(i32Ty, -1), "eof");  // EOF = -1
        auto* isNewline = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, ch,
            llvm::ConstantInt::get(i32Ty, '\n'), "nl");
        auto* isEnd = builder.CreateOr(isEOF, isNewline, "isend");
        builder.CreateCondBr(isEnd, endBB, bodyBB);

        // body: si len == cap, grow
        builder.SetInsertPoint(bodyBB);
        auto* curLen = builder.CreateLoad(i64Ty, lenAlloca, "len");
        auto* curCap = builder.CreateLoad(i64Ty, capacityAlloca, "cap");
        auto* needGrow = builder.CreateICmp(llvm::CmpInst::ICMP_EQ, curLen, curCap, "grow");
        builder.CreateCondBr(needGrow, growBB, storeBB);

        // grow: newCap = cap * 2; newBuf = malloc(newCap); memcpy; free(old); update buf y cap
        builder.SetInsertPoint(growBB);
        auto* newCap = builder.CreateMul(curCap, llvm::ConstantInt::get(i64Ty, 2), "newcap");
        auto* newBuf = builder.CreateCall(llvm::FunctionCallee(mallocFunc), {newCap}, "newbuf");
        auto* oldBuf = builder.CreateLoad(i8Ptr, bufAlloca, "oldbuf");
        auto* newBufAsI8 = builder.CreateBitCast(newBuf, i8Ptr, "nbi8");
        auto* oldBufAsI8 = builder.CreateBitCast(oldBuf, i8Ptr, "obi8");
        builder.CreateCall(llvm::FunctionCallee(memcpyFunc),
            {newBufAsI8, oldBufAsI8, curCap});
        builder.CreateCall(llvm::FunctionCallee(freeFunc), {oldBufAsI8});
        builder.CreateStore(newBuf, bufAlloca);
        builder.CreateStore(newCap, capacityAlloca);
        builder.CreateBr(storeBB);

        // store: buf[len] = ch; len++
        builder.SetInsertPoint(storeBB);
        auto* lenForStore = builder.CreateLoad(i64Ty, lenAlloca, "len2");
        auto* curBuf = builder.CreateLoad(i8Ptr, bufAlloca, "cbuf");
        auto* storePtr = builder.CreateInBoundsGEP(llvm::Type::getInt8Ty(context), curBuf, lenForStore, "sp");
        auto* chI8 = builder.CreateTrunc(ch, llvm::Type::getInt8Ty(context), "ch8");
        builder.CreateStore(chI8, storePtr);
        auto* newLen = builder.CreateAdd(lenForStore, llvm::ConstantInt::get(i64Ty, 1), "newlen");
        builder.CreateStore(newLen, lenAlloca);
        builder.CreateBr(condBB);

        // end: buf[len] = 0; return buf
        builder.SetInsertPoint(endBB);
        auto* finalBuf = builder.CreateLoad(i8Ptr, bufAlloca, "fbuf");
        auto* finalLen = builder.CreateLoad(i64Ty, lenAlloca, "flen");
        auto* endPtr = builder.CreateInBoundsGEP(llvm::Type::getInt8Ty(context), finalBuf, finalLen, "ep");
        builder.CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(context), 0), endPtr);
        return {finalBuf, ZenType::String};
    }

    // azar(n) -> numero   (numero aleatorio entre 0 y n-1)
    if (name == "azar" || name == "random" || name == "rand") {
        if (node->args.size() != 1)
            throw std::runtime_error("azar() espera 1 argumento (n) en linea " +
                std::to_string(node->line));
        // La primera vez, sembrar rand() con time(NULL) para que sea aleatorio de verdad
        if (!srandCalled) {
            srandCalled = true;
            auto* nullPtr = llvm::ConstantPointerNull::get(
                llvm::Type::getInt64Ty(context)->getPointerTo());
            auto* t = builder.CreateCall(llvm::FunctionCallee(timeFunc), {nullPtr}, "seed");
            auto* tI32 = builder.CreateTrunc(t, llvm::Type::getInt32Ty(context), "seed32");
            builder.CreateCall(llvm::FunctionCallee(srandFunc), {tI32});
        }
        auto* n = getNumArg(0);
        // rand() % n  (n convertido a i32)
        auto* nI32 = builder.CreateFPToSI(n, i32Ty, "n");
        auto* r = builder.CreateCall(llvm::FunctionCallee(randFunc), {}, "r");
        auto* mod = builder.CreateSRem(r, nI32, "mod");
        auto* asDouble = builder.CreateSIToFP(mod, doubleTy, "az");
        return {asDouble, ZenType::Number};
    }

    // reloj() -> numero  (timestamp actual en segundos)
    if (name == "reloj" || name == "clock" || name == "time" || name == "now") {
        if (!node->args.empty())
            throw std::runtime_error("reloj() no toma argumentos en linea " +
                std::to_string(node->line));
        auto* nullPtr = llvm::ConstantPointerNull::get(i64Ty->getPointerTo());
        auto* t = builder.CreateCall(llvm::FunctionCallee(timeFunc), {nullPtr}, "t");
        auto* asDouble = builder.CreateSIToFP(t, doubleTy, "td");
        return {asDouble, ZenType::Number};
    }

    // dormir(ms) -> nada  (espera N milisegundos)
    if (name == "dormir" || name == "sleep" || name == "esperar") {
        if (node->args.size() != 1)
            throw std::runtime_error("dormir() espera 1 argumento (ms) en linea " +
                std::to_string(node->line));
        auto* ms = getNumArg(0);
        // usleep toma microsegundos, asi que multiplicamos por 1000
        auto* us = builder.CreateFMul(ms, llvm::ConstantFP::get(doubleTy, 1000.0), "us");
        auto* usI32 = builder.CreateFPToSI(us, i32Ty, "us32");
        builder.CreateCall(llvm::FunctionCallee(usleepFunc), {usI32});
        return {llvm::ConstantFP::get(doubleTy, 0.0), ZenType::Number};
    }

    // limpiar() -> nada  (limpia la pantalla con codigo ANSI)
    if (name == "limpiar" || name == "clear" || name == "cls") {
        if (!node->args.empty())
            throw std::runtime_error("limpiar() no toma argumentos en linea " +
                std::to_string(node->line));
        auto* clearSeq = builder.CreateGlobalStringPtr("\x1b[2J\x1b[H");
        builder.CreateCall(llvm::FunctionCallee(printfFunc), {clearSeq});
        return {llvm::ConstantFP::get(doubleTy, 0.0), ZenType::Number};
    }

    // ---- Builtins de listas ----

    // agregar(lista, valor) -> nada  (push al final)
    if (name == "agregar" || name == "push" || name == "append" || name == "add") {
        if (node->args.size() != 2)
            throw std::runtime_error("agregar() espera 2 argumentos (lista, valor) en linea " +
                std::to_string(node->line));
        auto [listVal, listType] = generateExpr(node->args[0].get());
        // Aceptar cualquier tipo (puede ser Null del pre-scan, se resolvia en runtime)
        // No lanzar error si no es List - tratarlo como lista igual
        auto [val, valType] = generateExpr(node->args[1].get());
        auto* boxed = boxValue(val, valType);
        // Si no es lista, bitcastear a i8* (el runtime lo manejara)
        auto* listPtr = builder.CreateBitCast(listVal, llvm::Type::getInt8PtrTy(context), "lst");
        builder.CreateCall(llvm::FunctionCallee(listPushFunc), {listPtr, boxed});
        return {llvm::ConstantFP::get(doubleTy, 0.0), ZenType::Number};
    }

    // contiene(lista, valor) -> bool
    if (name == "contiene" || name == "contains" || name == "in") {
        if (node->args.size() != 2)
            throw std::runtime_error("contiene() espera 2 argumentos (lista, valor) en linea " +
                std::to_string(node->line));
        auto [listVal, listType] = generateExpr(node->args[0].get());
        if (listType != ZenType::List) {
            throw std::runtime_error("contiene() espera una lista como primer argumento en linea " +
                std::to_string(node->line));
        }
        auto [val, valType] = generateExpr(node->args[1].get());
        auto* boxed = boxValue(val, valType);
        auto* result = builder.CreateCall(
            llvm::FunctionCallee(listContainsFunc), {listVal, boxed}, "has");
        return {result, ZenType::Bool};
    }

    // quitar(lista) -> valor  (pop del final; devuelve el elemento quitado)
    if (name == "quitar" || name == "pop" || name == "remove_last") {
        if (node->args.size() != 1)
            throw std::runtime_error("quitar() espera 1 argumento (lista) en linea " +
                std::to_string(node->line));
        auto [listVal, listType] = generateExpr(node->args[0].get());
        if (listType != ZenType::List) {
            throw std::runtime_error("quitar() espera una lista en linea " +
                std::to_string(node->line));
        }
        auto* result = builder.CreateCall(
            llvm::FunctionCallee(listPopFunc), {listVal}, "popped");
        return {result, ZenType::String};
    }

    // ---- longitud(lista) cuando existan listas: ya implementado arriba

    // No es un builtin conocido
    
    // === STRINGS RICOS v2.0 ===
    if (name == "empieza_con" || name == "starts_with" || name == "startswith") {
        if (node->args.size() != 2)
            throw std::runtime_error("empieza_con() espera 2 args");
        auto sv = generateExpr(node->args[0].get());
        auto pv = generateExpr(node->args[1].get());
        auto str = toString(sv.first, sv.second);
        auto pref = toString(pv.first, pv.second);
        auto* cmp = builder.CreateCall(strcmpFunc, {str, pref}, "scmp");
        auto* eq = builder.CreateICmpEQ(cmp, builder.getInt32(0), "eq");
        return std::make_pair(eq, ZenType::Bool);
    }
    if (name == "termina_con" || name == "ends_with" || name == "endswith") {
        if (node->args.size() != 2)
            throw std::runtime_error("termina_con() espera 2 args");
        auto sv = generateExpr(node->args[0].get());
        auto fv = generateExpr(node->args[1].get());
        auto str = toString(sv.first, sv.second);
        auto suf = toString(fv.first, fv.second);
        auto* slen = builder.CreateCall(strlenFunc, {str}, "slen");
        auto* flen = builder.CreateCall(strlenFunc, {suf}, "flen");
        auto* diff = builder.CreateSub(slen, flen, "diff");
        auto* neg = builder.CreateICmpSLT(diff, builder.getInt64(0), "neg");
        auto* off = builder.CreateInBoundsGEP(llvm::Type::getInt8Ty(context), str, diff, "soff");
        auto* cmp = builder.CreateCall(strcmpFunc, {off, suf}, "scmp");
        auto* eq = builder.CreateICmpEQ(cmp, builder.getInt32(0), "eq");
        auto* res = builder.CreateSelect(neg, builder.getInt1(0), eq, "res");
        return std::make_pair(res, ZenType::Bool);
    }
    if (name == "contiene_texto" || name == "contains_str") {
        if (node->args.size() != 2)
            throw std::runtime_error("contiene_texto() espera 2 args");
        auto sv = generateExpr(node->args[0].get());
        auto bv = generateExpr(node->args[1].get());
        auto str = toString(sv.first, sv.second);
        auto search = toString(bv.first, bv.second);
        auto* r = builder.CreateCall(strstrFunc, {str, search}, "strstr");
        auto* nn = builder.CreateICmpNE(r, llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context)), "nn");
        return std::make_pair(nn, ZenType::Bool);
    }
    if (name == "longitud_texto" || name == "strlen" || name == "string_length") {
        if (node->args.size() != 1)
            throw std::runtime_error("longitud_texto() espera 1 arg");
        auto sv = generateExpr(node->args[0].get());
        auto str = toString(sv.first, sv.second);
        auto* len = builder.CreateCall(strlenFunc, {str}, "sl");
        auto* ld = builder.CreateSIToFP(len, llvm::Type::getDoubleTy(context), "ld");
        return std::make_pair(ld, ZenType::Number);
    }


    // === MATH BUILTINS (v2.0) ===
    // abs(x) -> |x|
    if (name == "abs" || name == "valor_absoluto") {
        if (node->args.size() != 1) throw std::runtime_error("abs() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("fabs", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "abs"), ZenType::Number);
    }
    // piso(x) -> floor
    if (name == "piso" || name == "floor" || name == "piso_inf") {
        if (node->args.size() != 1) throw std::runtime_error("piso() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("floor", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "floor"), ZenType::Number);
    }
    // techo(x) -> ceil
    if (name == "techo" || name == "ceil" || name == "piso_sup") {
        if (node->args.size() != 1) throw std::runtime_error("techo() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("ceil", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "ceil"), ZenType::Number);
    }
    // redondear(x) -> round
    if (name == "redondear" || name == "round") {
        if (node->args.size() != 1) throw std::runtime_error("redondear() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("round", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "round"), ZenType::Number);
    }
    // raiz(x) -> sqrt
    if (name == "raiz" || name == "sqrt" || name == "raiz_cuadrada") {
        if (node->args.size() != 1) throw std::runtime_error("raiz() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("sqrt", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "sqrt"), ZenType::Number);
    }
    // seno(x) -> sin (radianes)
    if (name == "seno" || name == "sin" || name == "sin_rad") {
        if (node->args.size() != 1) throw std::runtime_error("seno() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("sin", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "sin"), ZenType::Number);
    }
    // coseno(x) -> cos
    if (name == "coseno" || name == "cos" || name == "cos_rad") {
        if (node->args.size() != 1) throw std::runtime_error("coseno() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("cos", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "cos"), ZenType::Number);
    }
    // tangente(x) -> tan
    if (name == "tangente" || name == "tan") {
        if (node->args.size() != 1) throw std::runtime_error("tangente() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("tan", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "tan"), ZenType::Number);
    }
    // atan2(y, x) -> arco tangente
    if (name == "atan2" || name == "arctan2") {
        if (node->args.size() != 2) throw std::runtime_error("atan2() espera 2 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto y = toDouble(v1, t1);
        auto x = toDouble(v2, t2);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("atan2", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)}),
            {y, x}, "atan2"), ZenType::Number);
    }
    // min(a, b) -> menor
    if (name == "min" || name == "menor") {
        if (node->args.size() != 2) throw std::runtime_error("min() espera 2 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto a = toDouble(v1, t1);
        auto b = toDouble(v2, t2);
        auto* cmp = builder.CreateFCmpOLT(a, b, "cmp");
        return std::make_pair(builder.CreateSelect(cmp, a, b, "min"), ZenType::Number);
    }
    // max(a, b) -> mayor
    if (name == "max" || name == "mayor") {
        if (node->args.size() != 2) throw std::runtime_error("max() espera 2 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto a = toDouble(v1, t1);
        auto b = toDouble(v2, t2);
        auto* cmp = builder.CreateFCmpOGT(a, b, "cmp");
        return std::make_pair(builder.CreateSelect(cmp, a, b, "max"), ZenType::Number);
    }
    // potencia(base, exp) -> pow
    if (name == "potencia" || name == "pow" || name == "power") {
        if (node->args.size() != 2) throw std::runtime_error("potencia() espera 2 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto base = toDouble(v1, t1);
        auto exp = toDouble(v2, t2);
        return std::make_pair(builder.CreateCall(powFunc, {base, exp}, "pow"), ZenType::Number);
    }
    // limitar(valor, min, max) -> clamp
    if (name == "limitar" || name == "clamp") {
        if (node->args.size() != 3) throw std::runtime_error("limitar() espera 3 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto [v3, t3] = generateExpr(node->args[2].get());
        auto val = toDouble(v1, t1);
        auto lo = toDouble(v2, t2);
        auto hi = toDouble(v3, t3);
        // clamp = max(min(val, hi), lo)
        auto* cmp1 = builder.CreateFCmpOLT(val, hi, "c1");
        auto* step1 = builder.CreateSelect(cmp1, val, hi, "s1");
        auto* cmp2 = builder.CreateFCmpOGT(step1, lo, "c2");
        auto* result = builder.CreateSelect(cmp2, step1, lo, "clamp");
        return std::make_pair(result, ZenType::Number);
    }
    // interpolar(a, b, t) -> lerp
    if (name == "interpolar" || name == "lerp") {
        if (node->args.size() != 3) throw std::runtime_error("interpolar() espera 3 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto [v3, t3] = generateExpr(node->args[2].get());
        auto a = toDouble(v1, t1);
        auto b = toDouble(v2, t2);
        auto t = toDouble(v3, t3);
        // lerp = a + (b - a) * t
        auto* diff = builder.CreateFSub(b, a, "diff");
        auto* scaled = builder.CreateFMul(diff, t, "scaled");
        auto* result = builder.CreateFAdd(a, scaled, "lerp");
        return std::make_pair(result, ZenType::Number);
    }
    // distancia(x1, y1, x2, y2) -> dist (2D)
    if (name == "distancia" || name == "dist" || name == "distance") {
        if (node->args.size() != 4) throw std::runtime_error("distancia() espera 4 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto [v3, t3] = generateExpr(node->args[2].get());
        auto [v4, t4] = generateExpr(node->args[3].get());
        auto x1 = toDouble(v1, t1);
        auto y1 = toDouble(v2, t2);
        auto x2 = toDouble(v3, t3);
        auto y2 = toDouble(v4, t4);
        // dx = x2-x1, dy = y2-y1, dist = sqrt(dx*dx + dy*dy)
        auto* dx = builder.CreateFSub(x2, x1, "dx");
        auto* dy = builder.CreateFSub(y2, y1, "dy");
        auto* dx2 = builder.CreateFMul(dx, dx, "dx2");
        auto* dy2 = builder.CreateFMul(dy, dy, "dy2");
        auto* sum = builder.CreateFAdd(dx2, dy2, "sum");
        auto* sqrt = builder.CreateCall(
            getOrInsertExtern("sqrt", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {sum}, "sqrt");
        return std::make_pair(sqrt, ZenType::Number);
    }
    // grados(radianes) -> grados
    if (name == "grados" || name == "degrees" || name == "to_degrees") {
        if (node->args.size() != 1) throw std::runtime_error("grados() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* result = builder.CreateFMul(d, llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 57.295779513), "deg");
        return std::make_pair(result, ZenType::Number);
    }
    // radianes(grados) -> radianes
    if (name == "radianes" || name == "radians" || name == "to_radians") {
        if (node->args.size() != 1) throw std::runtime_error("radianes() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* result = builder.CreateFMul(d, llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.017453293), "rad");
        return std::make_pair(result, ZenType::Number);
    }
    // signo(x) -> -1, 0, or 1
    if (name == "signo" || name == "sign") {
        if (node->args.size() != 1) throw std::runtime_error("signo() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* zero = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0);
        auto* one = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 1.0);
        auto* neg_one = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), -1.0);
        auto* is_pos = builder.CreateFCmpOGT(d, zero, "is_pos");
        auto* is_neg = builder.CreateFCmpOLT(d, zero, "is_neg");
        auto* step1 = builder.CreateSelect(is_pos, one, neg_one, "s1");
        auto* is_zero = builder.CreateFCmpOEQ(d, zero, "is_zero");
        auto* result = builder.CreateSelect(is_zero, zero, step1, "sign");
        return std::make_pair(result, ZenType::Number);
    }


    // === MATH BUILTINS (v2.0) ===
    // abs(x) -> |x|
    if (name == "abs" || name == "valor_absoluto") {
        if (node->args.size() != 1) throw std::runtime_error("abs() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("fabs", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "abs"), ZenType::Number);
    }
    // piso(x) -> floor
    if (name == "piso" || name == "floor" || name == "piso_inf") {
        if (node->args.size() != 1) throw std::runtime_error("piso() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("floor", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "floor"), ZenType::Number);
    }
    // techo(x) -> ceil
    if (name == "techo" || name == "ceil" || name == "piso_sup") {
        if (node->args.size() != 1) throw std::runtime_error("techo() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("ceil", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "ceil"), ZenType::Number);
    }
    // redondear(x) -> round
    if (name == "redondear" || name == "round") {
        if (node->args.size() != 1) throw std::runtime_error("redondear() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("round", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "round"), ZenType::Number);
    }
    // raiz(x) -> sqrt
    if (name == "raiz" || name == "sqrt" || name == "raiz_cuadrada") {
        if (node->args.size() != 1) throw std::runtime_error("raiz() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("sqrt", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "sqrt"), ZenType::Number);
    }
    // seno(x) -> sin (radianes)
    if (name == "seno" || name == "sin" || name == "sin_rad") {
        if (node->args.size() != 1) throw std::runtime_error("seno() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("sin", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "sin"), ZenType::Number);
    }
    // coseno(x) -> cos
    if (name == "coseno" || name == "cos" || name == "cos_rad") {
        if (node->args.size() != 1) throw std::runtime_error("coseno() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("cos", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "cos"), ZenType::Number);
    }
    // tangente(x) -> tan
    if (name == "tangente" || name == "tan") {
        if (node->args.size() != 1) throw std::runtime_error("tangente() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("tan", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {d}, "tan"), ZenType::Number);
    }
    // atan2(y, x) -> arco tangente
    if (name == "atan2" || name == "arctan2") {
        if (node->args.size() != 2) throw std::runtime_error("atan2() espera 2 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto y = toDouble(v1, t1);
        auto x = toDouble(v2, t2);
        return std::make_pair(builder.CreateCall(
            getOrInsertExtern("atan2", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)}),
            {y, x}, "atan2"), ZenType::Number);
    }
    // min(a, b) -> menor
    if (name == "min" || name == "menor") {
        if (node->args.size() != 2) throw std::runtime_error("min() espera 2 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto a = toDouble(v1, t1);
        auto b = toDouble(v2, t2);
        auto* cmp = builder.CreateFCmpOLT(a, b, "cmp");
        return std::make_pair(builder.CreateSelect(cmp, a, b, "min"), ZenType::Number);
    }
    // max(a, b) -> mayor
    if (name == "max" || name == "mayor") {
        if (node->args.size() != 2) throw std::runtime_error("max() espera 2 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto a = toDouble(v1, t1);
        auto b = toDouble(v2, t2);
        auto* cmp = builder.CreateFCmpOGT(a, b, "cmp");
        return std::make_pair(builder.CreateSelect(cmp, a, b, "max"), ZenType::Number);
    }
    // potencia(base, exp) -> pow
    if (name == "potencia" || name == "pow" || name == "power") {
        if (node->args.size() != 2) throw std::runtime_error("potencia() espera 2 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto base = toDouble(v1, t1);
        auto exp = toDouble(v2, t2);
        return std::make_pair(builder.CreateCall(powFunc, {base, exp}, "pow"), ZenType::Number);
    }
    // limitar(valor, min, max) -> clamp
    if (name == "limitar" || name == "clamp") {
        if (node->args.size() != 3) throw std::runtime_error("limitar() espera 3 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto [v3, t3] = generateExpr(node->args[2].get());
        auto val = toDouble(v1, t1);
        auto lo = toDouble(v2, t2);
        auto hi = toDouble(v3, t3);
        // clamp = max(min(val, hi), lo)
        auto* cmp1 = builder.CreateFCmpOLT(val, hi, "c1");
        auto* step1 = builder.CreateSelect(cmp1, val, hi, "s1");
        auto* cmp2 = builder.CreateFCmpOGT(step1, lo, "c2");
        auto* result = builder.CreateSelect(cmp2, step1, lo, "clamp");
        return std::make_pair(result, ZenType::Number);
    }
    // interpolar(a, b, t) -> lerp
    if (name == "interpolar" || name == "lerp") {
        if (node->args.size() != 3) throw std::runtime_error("interpolar() espera 3 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto [v3, t3] = generateExpr(node->args[2].get());
        auto a = toDouble(v1, t1);
        auto b = toDouble(v2, t2);
        auto t = toDouble(v3, t3);
        // lerp = a + (b - a) * t
        auto* diff = builder.CreateFSub(b, a, "diff");
        auto* scaled = builder.CreateFMul(diff, t, "scaled");
        auto* result = builder.CreateFAdd(a, scaled, "lerp");
        return std::make_pair(result, ZenType::Number);
    }
    // distancia(x1, y1, x2, y2) -> dist (2D)
    if (name == "distancia" || name == "dist" || name == "distance") {
        if (node->args.size() != 4) throw std::runtime_error("distancia() espera 4 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto [v3, t3] = generateExpr(node->args[2].get());
        auto [v4, t4] = generateExpr(node->args[3].get());
        auto x1 = toDouble(v1, t1);
        auto y1 = toDouble(v2, t2);
        auto x2 = toDouble(v3, t3);
        auto y2 = toDouble(v4, t4);
        // dx = x2-x1, dy = y2-y1, dist = sqrt(dx*dx + dy*dy)
        auto* dx = builder.CreateFSub(x2, x1, "dx");
        auto* dy = builder.CreateFSub(y2, y1, "dy");
        auto* dx2 = builder.CreateFMul(dx, dx, "dx2");
        auto* dy2 = builder.CreateFMul(dy, dy, "dy2");
        auto* sum = builder.CreateFAdd(dx2, dy2, "sum");
        auto* sqrt = builder.CreateCall(
            getOrInsertExtern("sqrt", llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context)}), {sum}, "sqrt");
        return std::make_pair(sqrt, ZenType::Number);
    }
    // grados(radianes) -> grados
    if (name == "grados" || name == "degrees" || name == "to_degrees") {
        if (node->args.size() != 1) throw std::runtime_error("grados() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* result = builder.CreateFMul(d, llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 57.295779513), "deg");
        return std::make_pair(result, ZenType::Number);
    }
    // radianes(grados) -> radianes
    if (name == "radianes" || name == "radians" || name == "to_radians") {
        if (node->args.size() != 1) throw std::runtime_error("radianes() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* result = builder.CreateFMul(d, llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.017453293), "rad");
        return std::make_pair(result, ZenType::Number);
    }
    // signo(x) -> -1, 0, or 1
    if (name == "signo" || name == "sign") {
        if (node->args.size() != 1) throw std::runtime_error("signo() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* zero = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0);
        auto* one = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 1.0);
        auto* neg_one = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), -1.0);
        auto* is_pos = builder.CreateFCmpOGT(d, zero, "is_pos");
        auto* is_neg = builder.CreateFCmpOLT(d, zero, "is_neg");
        auto* step1 = builder.CreateSelect(is_pos, one, neg_one, "s1");
        auto* is_zero = builder.CreateFCmpOEQ(d, zero, "is_zero");
        auto* result = builder.CreateSelect(is_zero, zero, step1, "sign");
        return std::make_pair(result, ZenType::Number);
    }


    // === STRINGS RICOS v2.0 ===
    if (name == "empieza_con" || name == "starts_with" || name == "startswith") {
        if (node->args.size() != 2)
            throw std::runtime_error("empieza_con() espera 2 args");
        auto sv = generateExpr(node->args[0].get());
        auto pv = generateExpr(node->args[1].get());
        auto str = toString(sv.first, sv.second);
        auto pref = toString(pv.first, pv.second);
        auto* cmp = builder.CreateCall(strcmpFunc, {str, pref}, "scmp");
        auto* eq = builder.CreateICmpEQ(cmp, builder.getInt32(0), "eq");
        return std::make_pair(eq, ZenType::Bool);
    }
    if (name == "termina_con" || name == "ends_with" || name == "endswith") {
        if (node->args.size() != 2)
            throw std::runtime_error("termina_con() espera 2 args");
        auto sv = generateExpr(node->args[0].get());
        auto fv = generateExpr(node->args[1].get());
        auto str = toString(sv.first, sv.second);
        auto suf = toString(fv.first, fv.second);
        auto* slen = builder.CreateCall(strlenFunc, {str}, "slen");
        auto* flen = builder.CreateCall(strlenFunc, {suf}, "flen");
        auto* diff = builder.CreateSub(slen, flen, "diff");
        auto* neg = builder.CreateICmpSLT(diff, builder.getInt64(0), "neg");
        auto* off = builder.CreateInBoundsGEP(llvm::Type::getInt8Ty(context), str, diff, "soff");
        auto* cmp = builder.CreateCall(strcmpFunc, {off, suf}, "scmp");
        auto* eq = builder.CreateICmpEQ(cmp, builder.getInt32(0), "eq");
        auto* res = builder.CreateSelect(neg, builder.getInt1(0), eq, "res");
        return std::make_pair(res, ZenType::Bool);
    }
    if (name == "contiene_texto" || name == "contains_str") {
        if (node->args.size() != 2)
            throw std::runtime_error("contiene_texto() espera 2 args");
        auto sv = generateExpr(node->args[0].get());
        auto bv = generateExpr(node->args[1].get());
        auto str = toString(sv.first, sv.second);
        auto search = toString(bv.first, bv.second);
        auto* r = builder.CreateCall(strstrFunc, {str, search}, "strstr");
        auto* nn = builder.CreateICmpNE(r, llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context)), "nn");
        return std::make_pair(nn, ZenType::Bool);
    }
    if (name == "longitud_texto" || name == "strlen" || name == "string_length") {
        if (node->args.size() != 1)
            throw std::runtime_error("longitud_texto() espera 1 arg");
        auto sv = generateExpr(node->args[0].get());
        auto str = toString(sv.first, sv.second);
        auto* len = builder.CreateCall(strlenFunc, {str}, "sl");
        auto* ld = builder.CreateSIToFP(len, llvm::Type::getDoubleTy(context), "ld");
        return std::make_pair(ld, ZenType::Number);
    }


    if (name == "dict" || name == "diccionario") {
        auto* r = builder.CreateCall(dictCreateFunc, {}, "dnew");
        return std::make_pair(r, ZenType::Struct);
    }
    if (name == "dict_asignar" || name == "dict_set") {
        if (node->args.size() != 3) throw std::runtime_error("dict_asignar() espera 3 args");
        auto [dv,dt] = generateExpr(node->args[0].get());
        auto [kv,kt] = generateExpr(node->args[1].get());
        auto [vv,vt] = generateExpr(node->args[2].get());
        builder.CreateCall(dictSetFunc, {dv, toString(kv,kt), toString(vv,vt)}, "dset");
        return std::make_pair(dv, ZenType::Struct);
    }
    if (name == "dict_obtener" || name == "dict_get") {
        if (node->args.size() != 2) throw std::runtime_error("dict_obtener() espera 2 args");
        auto [dv,dt] = generateExpr(node->args[0].get());
        auto [kv,kt] = generateExpr(node->args[1].get());
        auto* r = builder.CreateCall(dictGetFunc, {dv, toString(kv,kt)}, "dget");
        return std::make_pair(r, ZenType::String);
    }
    if (name == "dict_contiene" || name == "dict_has") {
        if (node->args.size() != 2) throw std::runtime_error("dict_contiene() espera 2 args");
        auto [dv,dt] = generateExpr(node->args[0].get());
        auto [kv,kt] = generateExpr(node->args[1].get());
        auto* r = builder.CreateCall(dictHasFunc, {dv, toString(kv,kt)}, "dhas");
        return std::make_pair(r, ZenType::Bool);
    }
    if (name == "dict_tamano" || name == "dict_size") {
        if (node->args.size() != 1) throw std::runtime_error("dict_tamano() espera 1 arg");
        auto [dv,dt] = generateExpr(node->args[0].get());
        auto* r = builder.CreateCall(dictSizeFunc, {dv}, "dsize");
        return std::make_pair(builder.CreateSIToFP(r, llvm::Type::getDoubleTy(context), "ds"), ZenType::Number);
    }


    // === TYPE CHECKS Y CONVERSIONES (v2.0 final) ===
    // tipo(x) -> texto con el tipo ("numero", "texto", "bool", "lista", "struct")
    if (name == "tipo" || name == "type_of" || name == "typeof") {
        if (node->args.size() != 1) throw std::runtime_error("tipo() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        const char* type_name = "desconocido";
        if (t == ZenType::Number) type_name = "numero";
        else if (t == ZenType::String) type_name = "texto";
        else if (t == ZenType::Bool) type_name = "bool";
        else if (t == ZenType::List) type_name = "lista";
        else if (t == ZenType::Struct) type_name = "struct";
        else if (t == ZenType::Null) type_name = "nulo";
        return std::make_pair(builder.CreateGlobalStringPtr(type_name, "typename"), ZenType::String);
    }
    // es_numero(x) -> bool
    if (name == "es_numero" || name == "is_number" || name == "is_num") {
        if (node->args.size() != 1) throw std::runtime_error("es_numero() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        return std::make_pair(builder.getInt1(t == ZenType::Number), ZenType::Bool);
    }
    // es_texto(x) -> bool
    if (name == "es_texto" || name == "is_string" || name == "is_str") {
        if (node->args.size() != 1) throw std::runtime_error("es_texto() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        return std::make_pair(builder.getInt1(t == ZenType::String), ZenType::Bool);
    }
    // es_bool(x) -> bool
    if (name == "es_bool" || name == "is_bool" || name == "is_boolean") {
        if (node->args.size() != 1) throw std::runtime_error("es_bool() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        return std::make_pair(builder.getInt1(t == ZenType::Bool), ZenType::Bool);
    }
    // es_lista(x) -> bool
    if (name == "es_lista" || name == "is_list") {
        if (node->args.size() != 1) throw std::runtime_error("es_lista() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        return std::make_pair(builder.getInt1(t == ZenType::List), ZenType::Bool);
    }
    // a_texto(x) -> texto (alias de texto())
    if (name == "a_texto" || name == "to_string" || name == "to_str") {
        if (node->args.size() != 1) throw std::runtime_error("a_texto() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        return std::make_pair(toString(v, t), ZenType::String);
    }
    // a_numero(x) -> numero (alias de numero())
    if (name == "a_numero" || name == "to_number" || name == "to_num") {
        if (node->args.size() != 1) throw std::runtime_error("a_numero() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        return std::make_pair(toDouble(v, t), ZenType::Number);
    }
    // repetir_texto(texto, n) -> texto repetido n veces
    if (name == "repetir_texto" || name == "repeat_str" || name == "repeat_string") {
        if (node->args.size() != 2) throw std::runtime_error("repetir_texto() espera 2 args");
        auto [sv, st] = generateExpr(node->args[0].get());
        auto [nv, nt] = generateExpr(node->args[1].get());
        auto str = toString(sv, st);
        auto n = toDouble(nv, nt);
        // Simple: just return the string (placeholder, real impl needs a loop)
        return std::make_pair(str, ZenType::String);
    }
    // lista_vacia() -> lista vacia
    if (name == "lista_vacia" || name == "empty_list" || name == "new_list") {
        auto list = generateListLit(nullptr);
        return list;
    }
    // imprimir(x) - alias de muestra pero como expresion (retorna x)
    if (name == "imprimir" || name == "print_val") {
        if (node->args.size() != 1) throw std::runtime_error("imprimir() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto str = toString(v, t);
        builder.CreateCall(printfFunc, {builder.CreateGlobalStringPtr("%s\n", "fmt"), str}, "print");
        return std::make_pair(v, t);
    }
    // entero(x) -> parte entera de x (truncar)
    if (name == "entero" || name == "trunc" || name == "truncate" || name == "int_part") {
        if (node->args.size() != 1) throw std::runtime_error("entero() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        // trunc via cast to i64 then back to double
        auto* i = builder.CreateFPToSI(d, llvm::Type::getInt64Ty(context), "trunc_i");
        return std::make_pair(builder.CreateSIToFP(i, llvm::Type::getDoubleTy(context), "trunc_d"), ZenType::Number);
    }
    // aleatorio(min, max) -> numero entre min y max
    if (name == "aleatorio" || name == "random_range" || name == "rand_range") {
        if (node->args.size() != 2) throw std::runtime_error("aleatorio() espera 2 args");
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto lo = toDouble(v1, t1);
        auto hi = toDouble(v2, t2);
        // rand() / RAND_MAX * (hi - lo) + lo
        auto* r = builder.CreateCall(
            getOrInsertExtern("rand", llvm::Type::getInt32Ty(context), {}), {}, "rand");
        auto* r_double = builder.CreateSIToFP(r, llvm::Type::getDoubleTy(context), "r_d");
        auto* rand_max = llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 2147483647.0);
        auto* normalized = builder.CreateFDiv(r_double, rand_max, "norm");
        auto* range = builder.CreateFSub(hi, lo, "range");
        auto* scaled = builder.CreateFMul(normalized, range, "scaled");
        auto* result = builder.CreateFAdd(scaled, lo, "aleatorio");
        return std::make_pair(result, ZenType::Number);
    }


    // === GAMING API (v3.0) ===
    // --- INPUT: teclado ---
    if (name == "tecla" || name == "key_down" || name == "is_key_down") {
        if (node->args.size() != 1) throw std::runtime_error("tecla() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* i = builder.CreateFPToSI(d, llvm::Type::getInt32Ty(context), "key_i");
        auto* fn = getOrInsertExtern("arx_key_down", llvm::Type::getInt32Ty(context), {llvm::Type::getInt32Ty(context)});
        auto* r = builder.CreateCall(fn, {i}, "kd");
        auto* b = builder.CreateICmpNE(r, builder.getInt32(0), "kd_b");
        return std::make_pair(b, ZenType::Bool);
    }
    if (name == "tecla_presionada" || name == "key_pressed") {
        if (node->args.size() != 1) throw std::runtime_error("tecla_presionada() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* i = builder.CreateFPToSI(d, llvm::Type::getInt32Ty(context), "key_i");
        auto* fn = getOrInsertExtern("arx_key_pressed", llvm::Type::getInt32Ty(context), {llvm::Type::getInt32Ty(context)});
        auto* r = builder.CreateCall(fn, {i}, "kp");
        auto* b = builder.CreateICmpNE(r, builder.getInt32(0), "kp_b");
        return std::make_pair(b, ZenType::Bool);
    }
    if (name == "tecla_soltada" || name == "key_released") {
        if (node->args.size() != 1) throw std::runtime_error("tecla_soltada() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* i = builder.CreateFPToSI(d, llvm::Type::getInt32Ty(context), "key_i");
        auto* fn = getOrInsertExtern("arx_key_released", llvm::Type::getInt32Ty(context), {llvm::Type::getInt32Ty(context)});
        auto* r = builder.CreateCall(fn, {i}, "kr");
        auto* b = builder.CreateICmpNE(r, builder.getInt32(0), "kr_b");
        return std::make_pair(b, ZenType::Bool);
    }
    // --- INPUT: mouse ---
    if (name == "mouse_x" || name == "raton_x") {
        auto* fn = getOrInsertExtern("arx_mouse_x", llvm::Type::getDoubleTy(context), {});
        return std::make_pair(builder.CreateCall(fn, {}, "mx"), ZenType::Number);
    }
    if (name == "mouse_y" || name == "raton_y") {
        auto* fn = getOrInsertExtern("arx_mouse_y", llvm::Type::getDoubleTy(context), {});
        return std::make_pair(builder.CreateCall(fn, {}, "my"), ZenType::Number);
    }
    if (name == "mouse_presionado" || name == "mouse_down" || name == "mouse_pressed") {
        if (node->args.size() != 1) throw std::runtime_error("mouse_presionado() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* i = builder.CreateFPToSI(d, llvm::Type::getInt32Ty(context), "mb_i");
        auto* fn = getOrInsertExtern("arx_mouse_pressed", llvm::Type::getInt32Ty(context), {llvm::Type::getInt32Ty(context)});
        auto* r = builder.CreateCall(fn, {i}, "mp");
        auto* b = builder.CreateICmpNE(r, builder.getInt32(0), "mp_b");
        return std::make_pair(b, ZenType::Bool);
    }
    // --- AUDIO ---
    if (name == "reproducir" || name == "play_sound" || name == "audio_play") {
        if (node->args.size() != 1) throw std::runtime_error("reproducir() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto str = toString(v, t);
        auto* fn = getOrInsertExtern("arx_audio_play", llvm::Type::getVoidTy(context), {llvm::Type::getInt8PtrTy(context)});
        builder.CreateCall(fn, {str}, "aplay");
        return std::make_pair(builder.getInt1(1), ZenType::Bool);
    }
    if (name == "detener_audio" || name == "stop_sound" || name == "audio_stop") {
        auto* fn = getOrInsertExtern("arx_audio_stop", llvm::Type::getVoidTy(context), {});
        builder.CreateCall(fn, {}, "astop");
        return std::make_pair(builder.getInt1(1), ZenType::Bool);
    }
    if (name == "volumen" || name == "set_volume" || name == "audio_volume") {
        if (node->args.size() != 1) throw std::runtime_error("volumen() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto d = toDouble(v, t);
        auto* fn = getOrInsertExtern("arx_audio_set_volume", llvm::Type::getVoidTy(context), {llvm::Type::getDoubleTy(context)});
        builder.CreateCall(fn, {d}, "vol");
        return std::make_pair(d, ZenType::Number);
    }
    // --- RENDER 2D ---
    if (name == "dibujar_rect" || name == "draw_rect") {
        if (node->args.size() != 8) throw std::runtime_error("dibujar_rect() espera 8 args (x,y,w,h,r,g,b,a)");
        auto* fn = getOrInsertExtern("arx_draw_rect", llvm::Type::getVoidTy(context),
            {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)});
        std::vector<llvm::Value*> args;
        for (int i = 0; i < 8; i++) {
            auto [v, t] = generateExpr(node->args[i].get());
            args.push_back(toDouble(v, t));
        }
        builder.CreateCall(fn, args, "drect");
        return std::make_pair(builder.getInt1(1), ZenType::Bool);
    }
    if (name == "dibujar_circulo" || name == "draw_circle") {
        if (node->args.size() != 7) throw std::runtime_error("dibujar_circulo() espera 7 args (x,y,r,r,g,b,a)");
        auto* fn = getOrInsertExtern("arx_draw_circle", llvm::Type::getVoidTy(context),
            {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context)});
        std::vector<llvm::Value*> args;
        for (int i = 0; i < 7; i++) {
            auto [v, t] = generateExpr(node->args[i].get());
            args.push_back(toDouble(v, t));
        }
        builder.CreateCall(fn, args, "dcirc");
        return std::make_pair(builder.getInt1(1), ZenType::Bool);
    }
    if (name == "dibujar_texto" || name == "draw_text") {
        if (node->args.size() != 6) throw std::runtime_error("dibujar_texto() espera 6 args (texto,x,y,r,g,b)");
        auto* fn = getOrInsertExtern("arx_draw_text", llvm::Type::getVoidTy(context),
            {llvm::Type::getInt8PtrTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)});
        auto [sv, st] = generateExpr(node->args[0].get());
        auto str = toString(sv, st);
        std::vector<llvm::Value*> args = {str};
        for (int i = 1; i < 6; i++) {
            auto [v, t] = generateExpr(node->args[i].get());
            args.push_back(toDouble(v, t));
        }
        builder.CreateCall(fn, args, "dtext");
        return std::make_pair(builder.getInt1(1), ZenType::Bool);
    }
    if (name == "dibujar_linea" || name == "draw_line") {
        if (node->args.size() != 7) throw std::runtime_error("dibujar_linea() espera 7 args (x1,y1,x2,y2,r,g,b)");
        auto* fn = getOrInsertExtern("arx_draw_line", llvm::Type::getVoidTy(context),
            {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context)});
        std::vector<llvm::Value*> args;
        for (int i = 0; i < 7; i++) {
            auto [v, t] = generateExpr(node->args[i].get());
            args.push_back(toDouble(v, t));
        }
        builder.CreateCall(fn, args, "dline");
        return std::make_pair(builder.getInt1(1), ZenType::Bool);
    }
    // --- ENGINE ---
    if (name == "fps" || name == "get_fps") {
        auto* fn = getOrInsertExtern("arx_get_fps", llvm::Type::getDoubleTy(context), {});
        return std::make_pair(builder.CreateCall(fn, {}, "fps"), ZenType::Number);
    }
    if (name == "delta" || name == "get_delta" || name == "delta_time") {
        auto* fn = getOrInsertExtern("arx_get_delta", llvm::Type::getDoubleTy(context), {});
        return std::make_pair(builder.CreateCall(fn, {}, "delta"), ZenType::Number);
    }
    if (name == "salir" || name == "quit" || name == "exit_game") {
        auto* fn = getOrInsertExtern("arx_quit", llvm::Type::getVoidTy(context), {});
        builder.CreateCall(fn, {}, "quit");
        return std::make_pair(builder.getInt1(1), ZenType::Bool);
    }
    // --- PHYSICS ---
    if (name == "gravedad" || name == "set_gravity") {
        if (node->args.size() != 3) throw std::runtime_error("gravedad() espera 3 args (x,y,z)");
        auto* fn = getOrInsertExtern("arx_physics_set_gravity", llvm::Type::getVoidTy(context),
            {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)});
        auto [v1, t1] = generateExpr(node->args[0].get());
        auto [v2, t2] = generateExpr(node->args[1].get());
        auto [v3, t3] = generateExpr(node->args[2].get());
        builder.CreateCall(fn, {toDouble(v1, t1), toDouble(v2, t2), toDouble(v3, t3)}, "grav");
        return std::make_pair(builder.getInt1(1), ZenType::Bool);
    }
    if (name == "raycast" || name == "lanzar_rayo") {
        if (node->args.size() != 7) throw std::runtime_error("raycast() espera 7 args (x,y,z,dx,dy,dz,dist)");
        auto* fn = getOrInsertExtern("arx_physics_raycast", llvm::Type::getInt32Ty(context),
            {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
             llvm::Type::getDoubleTy(context)});
        std::vector<llvm::Value*> args;
        for (int i = 0; i < 7; i++) {
            auto [v, t] = generateExpr(node->args[i].get());
            args.push_back(toDouble(v, t));
        }
        auto* r = builder.CreateCall(fn, args, "ray");
        auto* b = builder.CreateICmpNE(r, builder.getInt32(0), "ray_b");
        return std::make_pair(b, ZenType::Bool);
    }


    // === AEX LAZY LOADING ===
    if (name == "aex_cargar" || name == "aex_load") {
        if (node->args.size() != 1) throw std::runtime_error("aex_cargar() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto str = toString(v, t);
        auto* fn = getOrInsertExtern("arx_aex_cargar", llvm::Type::getInt32Ty(context), {llvm::Type::getInt8PtrTy(context)});
        auto* r = builder.CreateCall(fn, {str}, "aex_load");
        auto* b = builder.CreateICmpNE(r, builder.getInt32(0), "aex_ok");
        return std::make_pair(b, ZenType::Bool);
    }
    if (name == "aex_descargar" || name == "aex_unload") {
        if (node->args.size() != 1) throw std::runtime_error("aex_descargar() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto str = toString(v, t);
        auto* fn = getOrInsertExtern("arx_aex_descargar", llvm::Type::getInt32Ty(context), {llvm::Type::getInt8PtrTy(context)});
        auto* r = builder.CreateCall(fn, {str}, "aex_unload");
        auto* b = builder.CreateICmpNE(r, builder.getInt32(0), "aex_ok");
        return std::make_pair(b, ZenType::Bool);
    }
    if (name == "aex_cargado" || name == "aex_is_loaded") {
        if (node->args.size() != 1) throw std::runtime_error("aex_cargado() espera 1 arg");
        auto [v, t] = generateExpr(node->args[0].get());
        auto str = toString(v, t);
        auto* fn = getOrInsertExtern("arx_aex_cargado", llvm::Type::getInt32Ty(context), {llvm::Type::getInt8PtrTy(context)});
        auto* r = builder.CreateCall(fn, {str}, "aex_chk");
        auto* b = builder.CreateICmpNE(r, builder.getInt32(0), "aex_b");
        return std::make_pair(b, ZenType::Bool);
    }

return {nullptr, ZenType::Void};
}

llvm::Value* CodeGen::toDouble(llvm::Value* val, ZenType type) {
    switch (type) {
        case ZenType::Number: return val;
        case ZenType::Bool:
            return builder.CreateUIToFP(val, llvm::Type::getDoubleTy(context), "tof");
        case ZenType::String:
            // Si el valor ya es double (GlobalVariable de tipo double), retornarlo
            if (val->getType() == llvm::Type::getDoubleTy(context))
                return val;
            // Si es i8*, convertir con strtod
            if (val->getType() == llvm::Type::getInt8PtrTy(context)) {
                auto* nullPtr = llvm::ConstantPointerNull::get(
                    llvm::Type::getInt8PtrTy(context)->getPointerTo());
                return builder.CreateCall(
                    llvm::FunctionCallee(strtodFunc), {val, nullPtr}, "s2d");
            }
            return val;
        default:
            return llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0);
    }
}

llvm::Value* CodeGen::toBool(llvm::Value* val, ZenType type) {
    switch (type) {
        case ZenType::Bool: return val;
        case ZenType::Number:
            return builder.CreateFCmp(llvm::CmpInst::FCMP_ONE, val,
                llvm::ConstantFP::get(llvm::Type::getDoubleTy(context), 0.0), "tobool");
        case ZenType::String:
            return builder.CreateICmp(llvm::CmpInst::ICMP_NE, val,
                llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context)), "tobool");
        default:
            return llvm::ConstantInt::getFalse(context);
    }
}

llvm::Value* CodeGen::toString(llvm::Value* val, ZenType type) {
    // Si el valor ya es i8*, retornarlo sin importar el tipo
    if (val->getType() == llvm::Type::getInt8PtrTy(context))
        return val;
    // Si el valor es double, convertir a string con numToStr
    if (val->getType() == llvm::Type::getDoubleTy(context))
        return builder.CreateCall(llvm::FunctionCallee(numToStrFunc), {val}, "numstr");
    switch (type) {
        case ZenType::String:
            return val;
        case ZenType::Number:
            // Llamar a __zen_num_to_str(double) -> char*
            return builder.CreateCall(llvm::FunctionCallee(numToStrFunc), {val}, "numstr");
        case ZenType::Bool: {
            // "yes" o "no" (coincide con generatePrint)
            auto* yesStr = builder.CreateGlobalStringPtr("yes");
            auto* noStr  = builder.CreateGlobalStringPtr("no");
            return builder.CreateSelect(val, yesStr, noStr, "boolstr");
        }
        case ZenType::Null:
            return builder.CreateGlobalStringPtr("nada");
        default:
            return builder.CreateGlobalStringPtr("");
    }
}

// ============================================================
// Output
// ============================================================

std::string CodeGen::getIR() {
    std::string str;
    llvm::raw_string_ostream os(str);
    module->print(os, nullptr);
    return str;
}

bool CodeGen::verify() {
    return llvm::verifyModule(*module, &llvm::errs());
}

std::vector<std::string> CodeGen::getRequiredLibs() const {
    return requiredLibs;
}