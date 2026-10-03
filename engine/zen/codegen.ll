; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@g_NODO_NUMBER = internal global i8* null
@g_NODO_STRING = internal global i8* null
@g_NODO_BOOL = internal global i8* null
@g_NODO_NULL = internal global i8* null
@g_NODO_IDENTIFIER = internal global i8* null
@g_NODO_BINARY_OP = internal global i8* null
@g_NODO_UNARY_OP = internal global i8* null
@g_NODO_FUNC_CALL = internal global i8* null
@g_NODO_ASSIGN = internal global i8* null
@g_NODO_PRINT = internal global i8* null
@g_NODO_IF = internal global i8* null
@g_NODO_WHILE = internal global i8* null
@g_NODO_FUNC_DECL = internal global i8* null
@g_NODO_RETURN = internal global i8* null
@3 = private unnamed_addr constant [4 x i8] c"zen\00", align 1
@4 = private unnamed_addr constant [7 x i8] c"printf\00", align 1
@5 = private unnamed_addr constant [7 x i8] c"malloc\00", align 1
@6 = private unnamed_addr constant [7 x i8] c"strlen\00", align 1
@7 = private unnamed_addr constant [5 x i8] c"main\00", align 1
@8 = private unnamed_addr constant [6 x i8] c"entry\00", align 1
@9 = private unnamed_addr constant [38 x i8] c"=== Codegen de Zen escrito en Zen ===\00", align 1
@10 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@11 = private unnamed_addr constant [23 x i8] c"Etapa 4: Zen sobre Zen\00", align 1
@12 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@13 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@14 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@15 = private unnamed_addr constant [28 x i8] c"FFI a LLVM-C API declarado:\00", align 1
@16 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@17 = private unnamed_addr constant [48 x i8] c"  - LLVMModuleCreateWithName, LLVMCreateBuilder\00", align 1
@18 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@19 = private unnamed_addr constant [60 x i8] c"  - LLVMBuildAdd, LLVMBuildSub, LLVMBuildMul, LLVMBuildFDiv\00", align 1
@20 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@21 = private unnamed_addr constant [49 x i8] c"  - LLVMConstReal, LLVMConstString, LLVMConstInt\00", align 1
@22 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@23 = private unnamed_addr constant [54 x i8] c"  - LLVMAppendBasicBlock, LLVMBuildRet, LLVMBuildCall\00", align 1
@24 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@25 = private unnamed_addr constant [53 x i8] c"  - LLVMBuildCondBr, LLVMBuildPhi (control de flujo)\00", align 1
@26 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@27 = private unnamed_addr constant [35 x i8] c"  - LLVMPrintModuleToFile (output)\00", align 1
@28 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@29 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@30 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@31 = private unnamed_addr constant [19 x i8] c"Funciones codegen:\00", align 1
@32 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@33 = private unnamed_addr constant [29 x i8] c"  - codegen_init() -> estado\00", align 1
@34 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@35 = private unnamed_addr constant [37 x i8] c"  - codegen_declarar_externs(estado)\00", align 1
@36 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@37 = private unnamed_addr constant [50 x i8] c"  - codegen_expresion(estado, nodo) -> valor LLVM\00", align 1
@38 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@39 = private unnamed_addr constant [36 x i8] c"  - codegen_statement(estado, nodo)\00", align 1
@40 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@41 = private unnamed_addr constant [39 x i8] c"  - codegen_programa(estado, programa)\00", align 1
@42 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@43 = private unnamed_addr constant [35 x i8] c"  - codegen_compilar(estado, ruta)\00", align 1
@44 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@45 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@46 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@47 = private unnamed_addr constant [27 x i8] c"Codegen en Zen cargado OK!\00", align 1
@48 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@49 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@50 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@51 = private unnamed_addr constant [61 x i8] c"PENDIENTE: implementar codegen_expresion y codegen_statement\00", align 1
@52 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@53 = private unnamed_addr constant [37 x i8] c"con todos los tipos de nodo del AST.\00", align 1
@54 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

declare i32 @printf(i8*, ...)

declare double @pow(double, double)

declare i32 @strcmp(i8*, i8*)

declare i64 @strlen(i8*)

declare i8* @strstr(i8*, i8*)

declare i8* @strcpy(i8*, i8*)

declare i8* @strncpy(i8*, i8*, i64)

declare i8* @malloc(i64)

declare void @memcpy(i8*, i8*, i64)

declare i32 @toupper(i32)

declare i32 @tolower(i32)

declare double @strtod(i8*, i8**)

declare i8* @fopen(i8*, i8*)

declare i32 @fclose(i8*)

declare i64 @fread(i8*, i64, i64, i8*)

declare i64 @fwrite(i8*, i64, i64, i8*)

declare i32 @fseek(i8*, i64, i32)

declare i64 @ftell(i8*)

declare void @exit(i32)

declare i64 @getline(i8**, i64*, i8*)

declare void @free(i8*)

declare i32 @rand()

declare void @srand(i32)

declare i64 @time(i64*)

declare i32 @usleep(i32)

define internal i8* @zen_concat(i8* %a, i8* %b) {
entry:
  %la = call i64 @strlen(i8* %a)
  %lb = call i64 @strlen(i8* %b)
  %total = add i64 %la, %lb
  %size = add i64 %total, 1
  %result = call i8* @malloc(i64 %size)
  %isnull = icmp eq i8* %result, null
  br i1 %isnull, label %alloc.fail, label %alloc.ok

alloc.fail:                                       ; preds = %entry
  ret i8* getelementptr inbounds ([1 x i8], [1 x i8]* @0, i32 0, i32 0)

alloc.ok:                                         ; preds = %entry
  call void @memcpy(i8* %result, i8* %a, i64 %la)
  %dst = getelementptr inbounds i8, i8* %result, i64 %la
  %lb1 = add i64 %lb, 1
  call void @memcpy(i8* %dst, i8* %b, i64 %lb1)
  ret i8* %result
}

declare i32 @snprintf(i8*, i64, i8*, ...)

define internal i8* @__zen_num_to_str(double %n) {
entry:
  %buf = call i8* @malloc(i64 32)
  %0 = call i32 (i8*, i64, i8*, ...) @snprintf(i8* %buf, i64 32, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @1, i32 0, i32 0), double %n)
  ret i8* %buf
}

define internal i8* @__zen_list_create(i64 %capacity) {
entry:
  %small = icmp slt i64 %capacity, 4
  %realcap = select i1 %small, i64 4, i64 %capacity
  %list = call i8* @malloc(i64 ptrtoint ({ i8**, i64, i64 }* getelementptr ({ i8**, i64, i64 }, { i8**, i64, i64 }* null, i32 1) to i64))
  %list.bc = bitcast i8* %list to { i8**, i64, i64 }*
  %ds = mul i64 %realcap, ptrtoint (i8** getelementptr (i8*, i8** null, i32 1) to i64)
  %data = call i8* @malloc(i64 %ds)
  %data.bc = bitcast i8* %data to i8**
  %data.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list.bc, i32 0, i32 0
  store i8** %data.bc, i8*** %data.f, align 8
  %size.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list.bc, i32 0, i32 1
  store i64 0, i64* %size.f, align 4
  %cap.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list.bc, i32 0, i32 2
  store i64 %realcap, i64* %cap.f, align 4
  ret i8* %list
}

define internal void @__zen_list_push(i8* %list, i8* %value) {
entry:
  %list1 = bitcast i8* %list to { i8**, i64, i64 }*
  %data.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 0
  %size.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 1
  %cap.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 2
  %size = load i64, i64* %size.f, align 4
  %cap = load i64, i64* %cap.f, align 4
  %grow = icmp eq i64 %size, %cap
  br i1 %grow, label %grow2, label %cont

grow2:                                            ; preds = %entry
  %newcap = mul i64 %cap, 2
  %nds = mul i64 %newcap, ptrtoint (i8** getelementptr (i8*, i8** null, i32 1) to i64)
  %newdata = call i8* @malloc(i64 %nds)
  %newdata.bc = bitcast i8* %newdata to i8**
  %olddata = load i8**, i8*** %data.f, align 8
  %cs = mul i64 %cap, ptrtoint (i8** getelementptr (i8*, i8** null, i32 1) to i64)
  %ndi8 = bitcast i8** %newdata.bc to i8*
  %odi8 = bitcast i8** %olddata to i8*
  call void @memcpy(i8* %ndi8, i8* %odi8, i64 %cs)
  call void @free(i8* %odi8)
  store i8** %newdata.bc, i8*** %data.f, align 8
  store i64 %newcap, i64* %cap.f, align 4
  br label %cont

cont:                                             ; preds = %grow2, %entry
  %data2 = load i8**, i8*** %data.f, align 8
  %slot = getelementptr inbounds i8*, i8** %data2, i64 %size
  store i8* %value, i8** %slot, align 8
  %newsize = add i64 %size, 1
  store i64 %newsize, i64* %size.f, align 4
  ret void
}

define internal i8* @__zen_list_get(i8* %list, i64 %idx) {
entry:
  %list1 = bitcast i8* %list to { i8**, i64, i64 }*
  %size.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 1
  %size = load i64, i64* %size.f, align 4
  %inrange = icmp slt i64 %idx, %size
  %neg = icmp slt i64 %idx, 0
  %0 = xor i1 %neg, true
  %valid = and i1 %inrange, %0
  br i1 %valid, label %ok, label %end

ok:                                               ; preds = %entry
  %data.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 0
  %data = load i8**, i8*** %data.f, align 8
  %slot = getelementptr inbounds i8*, i8** %data, i64 %idx
  %value = load i8*, i8** %slot, align 8
  br label %end

end:                                              ; preds = %ok, %entry
  %result = phi i8* [ null, %entry ], [ %value, %ok ]
  ret i8* %result
}

define internal void @__zen_list_set(i8* %list, i64 %idx, i8* %value) {
entry:
  %list1 = bitcast i8* %list to { i8**, i64, i64 }*
  %size.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 1
  %size = load i64, i64* %size.f, align 4
  %inrange = icmp slt i64 %idx, %size
  %neg = icmp slt i64 %idx, 0
  %0 = xor i1 %neg, true
  %valid = and i1 %inrange, %0
  br i1 %valid, label %ok, label %end

ok:                                               ; preds = %entry
  %data.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 0
  %data = load i8**, i8*** %data.f, align 8
  %slot = getelementptr inbounds i8*, i8** %data, i64 %idx
  store i8* %value, i8** %slot, align 8
  br label %end

end:                                              ; preds = %ok, %entry
  ret void
}

define internal i64 @__zen_list_len(i8* %list) {
entry:
  %list1 = bitcast i8* %list to { i8**, i64, i64 }*
  %size.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 1
  %size = load i64, i64* %size.f, align 4
  ret i64 %size
}

define internal i1 @__zen_list_contains(i8* %list, i8* %value) {
entry:
  %list1 = bitcast i8* %list to { i8**, i64, i64 }*
  %size.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 1
  %data.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 0
  %size = load i64, i64* %size.f, align 4
  %data = load i8**, i8*** %data.f, align 8
  %i = alloca i64, align 8
  store i64 0, i64* %i, align 4
  br label %cond

cond:                                             ; preds = %cont, %entry
  %i2 = load i64, i64* %i, align 4
  %cmp = icmp slt i64 %i2, %size
  br i1 %cmp, label %body, label %no

body:                                             ; preds = %cond
  %slot = getelementptr inbounds i8*, i8** %data, i64 %i2
  %elem = load i8*, i8** %slot, align 8
  %scmp = call i32 @strcmp(i8* %elem, i8* %value)
  %eq = icmp eq i32 %scmp, 0
  br i1 %eq, label %yes, label %cont

cont:                                             ; preds = %body
  %nexti = add i64 %i2, 1
  store i64 %nexti, i64* %i, align 4
  br label %cond

yes:                                              ; preds = %body
  ret i1 true

no:                                               ; preds = %cond
  ret i1 false
}

define internal i8* @__zen_list_pop(i8* %list) {
entry:
  %list1 = bitcast i8* %list to { i8**, i64, i64 }*
  %size.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 1
  %data.f = getelementptr inbounds { i8**, i64, i64 }, { i8**, i64, i64 }* %list1, i32 0, i32 0
  %size = load i64, i64* %size.f, align 4
  %empty = icmp eq i64 %size, 0
  br i1 %empty, label %end, label %ok

ok:                                               ; preds = %entry
  %newsize = sub i64 %size, 1
  %data = load i8**, i8*** %data.f, align 8
  %slot = getelementptr inbounds i8*, i8** %data, i64 %newsize
  %value = load i8*, i8** %slot, align 8
  store i64 %newsize, i64* %size.f, align 4
  br label %end

end:                                              ; preds = %ok, %entry
  %result = phi i8* [ null, %entry ], [ %value, %ok ]
  ret i8* %result
}

define internal i8* @__zen_struct_create(i64 %numFields) {
entry:
  %ds = mul i64 %numFields, ptrtoint (i8** getelementptr (i8*, i8** null, i32 1) to i64)
  %ts = add i64 ptrtoint ({ i64, i8** }* getelementptr ({ i64, i8** }, { i64, i8** }* null, i32 1) to i64), %ds
  %mem = call i8* @malloc(i64 %ts)
  %mem.bc = bitcast i8* %mem to { i64, i8** }*
  %nf.f = getelementptr inbounds { i64, i8** }, { i64, i8** }* %mem.bc, i32 0, i32 0
  store i64 %numFields, i64* %nf.f, align 4
  %mem.i8 = bitcast { i64, i8** }* %mem.bc to i8*
  %dp.o = getelementptr inbounds i8, i8* %mem.i8, i64 ptrtoint ({ i64, i8** }* getelementptr ({ i64, i8** }, { i64, i8** }* null, i32 1) to i64)
  %dp.bc = bitcast i8* %dp.o to i8**
  %data.f = getelementptr inbounds { i64, i8** }, { i64, i8** }* %mem.bc, i32 0, i32 1
  store i8** %dp.bc, i8*** %data.f, align 8
  %i = alloca i64, align 8
  store i64 0, i64* %i, align 4
  br label %sc.cond

sc.cond:                                          ; preds = %sc.body, %entry
  %i1 = load i64, i64* %i, align 4
  %cmp = icmp slt i64 %i1, %numFields
  br i1 %cmp, label %sc.body, label %sc.end

sc.body:                                          ; preds = %sc.cond
  %slot = getelementptr inbounds i8*, i8** %dp.bc, i64 %i1
  store i8* null, i8** %slot, align 8
  %next = add i64 %i1, 1
  store i64 %next, i64* %i, align 4
  br label %sc.cond

sc.end:                                           ; preds = %sc.cond
  ret i8* %mem
}

define internal i8* @__zen_struct_get(i8* %s, i64 %idx) {
entry:
  %hdr = bitcast i8* %s to { i64, i8** }*
  %data.f = getelementptr inbounds { i64, i8** }, { i64, i8** }* %hdr, i32 0, i32 1
  %data = load i8**, i8*** %data.f, align 8
  %slot = getelementptr inbounds i8*, i8** %data, i64 %idx
  %value = load i8*, i8** %slot, align 8
  ret i8* %value
}

define internal void @__zen_struct_set(i8* %s, i64 %idx, i8* %value) {
entry:
  %hdr = bitcast i8* %s to { i64, i8** }*
  %data.f = getelementptr inbounds { i64, i8** }, { i64, i8** }* %hdr, i32 0, i32 1
  %data = load i8**, i8*** %data.f, align 8
  %slot = getelementptr inbounds i8*, i8** %data, i64 %idx
  store i8* %value, i8** %slot, align 8
  ret void
}

define i8* @__zen_dict_create() {
entry:
  %raw = call i8* @malloc(i64 32)
  %memzero = call i8* @memset(i8* %raw, i32 0, i64 32)
  ret i8* %raw
}

declare i8* @memset(i8*, i32, i64)

define void @__zen_dict_set(i8* %0, i8* %1, i8* %2) {
entry:
  ret void
}

define i8* @__zen_dict_get(i8* %0, i8* %1) {
entry:
  ret i8* getelementptr inbounds ([1 x i8], [1 x i8]* @empty, i32 0, i32 0)
}

define i1 @__zen_dict_has(i8* %0, i8* %1) {
entry:
  ret i1 false
}

define i64 @__zen_dict_size(i8* %0) {
entry:
  ret i64 0
}

define internal i8* @__zen_platform() {
entry:
  ret i8* getelementptr inbounds ([6 x i8], [6 x i8]* @2, i32 0, i32 0)
}

declare i8* @LLVMModuleCreateWithName(i8*)

declare i8* @LLVMCreateBuilder()

declare void @LLVMDisposeBuilder(i8*)

declare void @LLVMDisposeModule(i8*)

declare void @LLVMDumpModule(i8*)

declare i32 @LLVMPrintModuleToFile(i8*, i8*, i8*)

declare i8* @LLVMInt1Type()

declare i8* @LLVMInt8Type()

declare i8* @LLVMInt32Type()

declare i8* @LLVMInt64Type()

declare i8* @LLVMFloatType()

declare i8* @LLVMDoubleType()

declare i8* @LLVMPointerType(i8*, i32)

declare i8* @LLVMVoidType()

declare i8* @LLVMFunctionType(i8*, i8*, i32)

declare i8* @LLVMAddFunction(i8*, i8*, i8*)

declare i8* @LLVMGetParam(i8*, i32)

declare i8* @LLVMAppendBasicBlock(i8*, i8*)

declare void @LLVMPositionBuilderAtEnd(i8*, i8*)

declare i8* @LLVMBuildRet(i8*, i8*)

declare i8* @LLVMBuildRetVoid(i8*)

declare i8* @LLVMBuildAdd(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildSub(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildMul(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildSDiv(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFAdd(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFSub(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFMul(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFDiv(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildAlloca(i8*, i8*)

declare i8* @LLVMBuildStore(i8*, i8*, i8*)

declare i8* @LLVMBuildLoad(i8*, i8*, i8*)

declare i8* @LLVMBuildCall(i8*, i8*, i8*, i32, i8*)

declare i8* @LLVMConstReal(i8*, double)

declare i8* @LLVMConstString(i8*, i32, i32)

declare i8* @LLVMConstInt(i8*, i32, i32)

declare i8* @LLVMBuildUIToFP(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildSIToFP(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFPToSI(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildIntToPtr(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFCmp(i8*, i32, i8*, i8*, i8*)

declare i8* @LLVMBuildICmp(i8*, i32, i8*, i8*, i8*)

declare i8* @LLVMBuildBr(i8*, i8*)

declare i8* @LLVMBuildCondBr(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildPhi(i8*, i8*)

declare void @LLVMAddIncoming(i8*, i8*, i8*, i32, i8*)

define i8* @i8ptr_type() {
entry:
  %call = call i8* @i8ptr_type()
  ret i8* %call
}

declare i8* @estado_modulo(i8*)

declare i8* @estado_builder(i8*)

define i8* @estado_modulo.1(i8* %e) {
entry:
  %e1 = alloca i8*, align 8
  store i8* %e, i8** %e1, align 8
  %e2 = load i8*, i8** %e1, align 8
  %member = call i8* @__zen_struct_get(i8* %e2, i64 0)
  ret i8* %member

entry3:                                           ; No predecessors!
  %e4 = alloca i8*, align 8
  store i8* %e, i8** %e4, align 8
  %e5 = load i8*, i8** %e4, align 8
  %member6 = call i8* @__zen_struct_get(i8* %e5, i64 0)
  ret i8* %member6
}

define i8* @estado_builder.2(i8* %e) {
entry:
  %e1 = alloca i8*, align 8
  store i8* %e, i8** %e1, align 8
  %e2 = load i8*, i8** %e1, align 8
  %member = call i8* @__zen_struct_get(i8* %e2, i64 1)
  ret i8* %member

entry3:                                           ; No predecessors!
  %e4 = alloca i8*, align 8
  store i8* %e, i8** %e4, align 8
  %e5 = load i8*, i8** %e4, align 8
  %member6 = call i8* @__zen_struct_get(i8* %e5, i64 1)
  ret i8* %member6
}

define i8* @nodo_tipo(i8* %n) {
entry:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  %n2 = load i8*, i8** %n1, align 8
  %member = call i8* @__zen_struct_get(i8* %n2, i64 4)
  ret i8* %member
}

define i8* @nodo_valor(i8* %n) {
entry:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  %n2 = load i8*, i8** %n1, align 8
  %member = call i8* @__zen_struct_get(i8* %n2, i64 3)
  ret i8* %member
}

define i8* @nodo_nombre(i8* %n) {
entry:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  %n2 = load i8*, i8** %n1, align 8
  %member = call i8* @__zen_struct_get(i8* %n2, i64 3)
  ret i8* %member
}

define i8* @nodo_op(i8* %n) {
entry:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  %n2 = load i8*, i8** %n1, align 8
  %member = call i8* @__zen_struct_get(i8* %n2, i64 3)
  ret i8* %member
}

define i8* @nodo_linea(i8* %n) {
entry:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  %n2 = load i8*, i8** %n1, align 8
  %member = call i8* @__zen_struct_get(i8* %n2, i64 1)
  ret i8* %member
}

define i8* @codegen_init() {
entry:
  %estado = alloca i8*, align 8
  %struct = call i8* @__zen_struct_create(i64 5)
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_struct_set(i8* %struct, i64 0, i8* %numstr)
  %numstr1 = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_struct_set(i8* %struct, i64 1, i8* %numstr1)
  %numstr2 = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_struct_set(i8* %struct, i64 2, i8* %numstr2)
  %numstr3 = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_struct_set(i8* %struct, i64 3, i8* %numstr3)
  %numstr4 = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_struct_set(i8* %struct, i64 4, i8* %numstr4)
  store i8* %struct, i8** %estado, align 8
  %estado5 = load i8*, i8** %estado, align 8
  %c_call = call i8* @LLVMModuleCreateWithName(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @3, i32 0, i32 0))
  call void @__zen_struct_set(i8* %estado5, i64 0, i8* %c_call)
  %estado6 = load i8*, i8** %estado, align 8
  %c_call7 = call i8* @LLVMCreateBuilder()
  call void @__zen_struct_set(i8* %estado6, i64 1, i8* %c_call7)
  %estado8 = load i8*, i8** %estado, align 8
  ret i8* %estado8
}

define i8* @codegen_declarar_externs(i8* %estado) {
entry:
  %strlen_type = alloca i8*, align 8
  %malloc_type = alloca i8*, align 8
  %i64 = alloca i8*, align 8
  %printf_type = alloca i8*, align 8
  %i32 = alloca i8*, align 8
  %i8ptr = alloca i8*, align 8
  %estado1 = alloca i8*, align 8
  store i8* %estado, i8** %estado1, align 8
  %call = call i8* @i8ptr_type()
  store i8* %call, i8** %i8ptr, align 8
  %c_call = call i8* @LLVMInt32Type()
  store i8* %c_call, i8** %i32, align 8
  %i322 = load i8*, i8** %i32, align 8
  %i8ptr3 = load i8*, i8** %i8ptr, align 8
  %c_call4 = call i8* @LLVMFunctionType(i8* %i322, i8* %i8ptr3, i32 1)
  store i8* %c_call4, i8** %printf_type, align 8
  %estado5 = load i8*, i8** %estado1, align 8
  %call6 = call i8* @estado_modulo.1(i8* %estado5)
  %printf_type7 = load i8*, i8** %printf_type, align 8
  %c_call8 = call i8* @LLVMAddFunction(i8* %call6, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @4, i32 0, i32 0), i8* %printf_type7)
  %c_call9 = call i8* @LLVMInt64Type()
  store i8* %c_call9, i8** %i64, align 8
  %i8ptr10 = load i8*, i8** %i8ptr, align 8
  %i6411 = load i8*, i8** %i64, align 8
  %c_call12 = call i8* @LLVMFunctionType(i8* %i8ptr10, i8* %i6411, i32 0)
  store i8* %c_call12, i8** %malloc_type, align 8
  %estado13 = load i8*, i8** %estado1, align 8
  %call14 = call i8* @estado_modulo.1(i8* %estado13)
  %malloc_type15 = load i8*, i8** %malloc_type, align 8
  %c_call16 = call i8* @LLVMAddFunction(i8* %call14, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @5, i32 0, i32 0), i8* %malloc_type15)
  %i6417 = load i8*, i8** %i64, align 8
  %i8ptr18 = load i8*, i8** %i8ptr, align 8
  %c_call19 = call i8* @LLVMFunctionType(i8* %i6417, i8* %i8ptr18, i32 0)
  store i8* %c_call19, i8** %strlen_type, align 8
  %estado20 = load i8*, i8** %estado1, align 8
  %call21 = call i8* @estado_modulo.1(i8* %estado20)
  %strlen_type22 = load i8*, i8** %strlen_type, align 8
  %c_call23 = call i8* @LLVMAddFunction(i8* %call21, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @6, i32 0, i32 0), i8* %strlen_type22)
  ret i8* null
}

define i8* @codegen_expresion(i8* %estado, i8* %nodo) {
entry:
  %tipo = alloca i8*, align 8
  %estado1 = alloca i8*, align 8
  store i8* %estado, i8** %estado1, align 8
  %nodo2 = alloca i8*, align 8
  store i8* %nodo, i8** %nodo2, align 8
  %nodo3 = load i8*, i8** %nodo2, align 8
  %call = call i8* @nodo_tipo(i8* %nodo3)
  store i8* %call, i8** %tipo, align 8
  %tipo4 = load i8*, i8** %tipo, align 8
  %NODO_NUMBER = load i8*, i8** @g_NODO_NUMBER, align 8
  %s2d = call double @strtod(i8* %tipo4, i8** null)
  %s2d5 = call double @strtod(i8* %NODO_NUMBER, i8** null)
  %cmp = fcmp oeq double %s2d, %s2d5
  br i1 %cmp, label %then, label %endif

then:                                             ; preds = %entry
  %c_call = call i8* @LLVMDoubleType()
  %nodo6 = load i8*, i8** %nodo2, align 8
  %call7 = call i8* @nodo_valor(i8* %nodo6)
  %num = call double @strtod(i8* %call7, i8** null)
  %c_call8 = call i8* @LLVMConstReal(i8* %c_call, double %num)
  ret i8* %c_call8

endif:                                            ; preds = %entry
  %tipo9 = load i8*, i8** %tipo, align 8
  %NODO_STRING = load i8*, i8** @g_NODO_STRING, align 8
  %s2d10 = call double @strtod(i8* %tipo9, i8** null)
  %s2d11 = call double @strtod(i8* %NODO_STRING, i8** null)
  %cmp12 = fcmp oeq double %s2d10, %s2d11
  br i1 %cmp12, label %then13, label %endif14

then13:                                           ; preds = %endif
  %nodo15 = load i8*, i8** %nodo2, align 8
  %call16 = call i8* @nodo_valor(i8* %nodo15)
  %c_call17 = call i8* @LLVMConstString(i8* %call16, i32 0, i32 0)
  ret i8* %c_call17

endif14:                                          ; preds = %endif
  %tipo18 = load i8*, i8** %tipo, align 8
  %NODO_IDENTIFIER = load i8*, i8** @g_NODO_IDENTIFIER, align 8
  %s2d19 = call double @strtod(i8* %tipo18, i8** null)
  %s2d20 = call double @strtod(i8* %NODO_IDENTIFIER, i8** null)
  %cmp21 = fcmp oeq double %s2d19, %s2d20
  br i1 %cmp21, label %then22, label %endif23

then22:                                           ; preds = %endif14
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr

endif23:                                          ; preds = %endif14
  %tipo24 = load i8*, i8** %tipo, align 8
  %NODO_BINARY_OP = load i8*, i8** @g_NODO_BINARY_OP, align 8
  %s2d25 = call double @strtod(i8* %tipo24, i8** null)
  %s2d26 = call double @strtod(i8* %NODO_BINARY_OP, i8** null)
  %cmp27 = fcmp oeq double %s2d25, %s2d26
  br i1 %cmp27, label %then28, label %endif29

then28:                                           ; preds = %endif23
  %numstr30 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr30

endif29:                                          ; preds = %endif23
  %tipo31 = load i8*, i8** %tipo, align 8
  %NODO_FUNC_CALL = load i8*, i8** @g_NODO_FUNC_CALL, align 8
  %s2d32 = call double @strtod(i8* %tipo31, i8** null)
  %s2d33 = call double @strtod(i8* %NODO_FUNC_CALL, i8** null)
  %cmp34 = fcmp oeq double %s2d32, %s2d33
  br i1 %cmp34, label %then35, label %endif36

then35:                                           ; preds = %endif29
  %numstr37 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr37

endif36:                                          ; preds = %endif29
  %numstr38 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr38
}

define i8* @codegen_statement(i8* %estado, i8* %nodo) {
entry:
  %tipo = alloca i8*, align 8
  %estado1 = alloca i8*, align 8
  store i8* %estado, i8** %estado1, align 8
  %nodo2 = alloca i8*, align 8
  store i8* %nodo, i8** %nodo2, align 8
  %nodo3 = load i8*, i8** %nodo2, align 8
  %call = call i8* @nodo_tipo(i8* %nodo3)
  store i8* %call, i8** %tipo, align 8
  %tipo4 = load i8*, i8** %tipo, align 8
  %NODO_PRINT = load i8*, i8** @g_NODO_PRINT, align 8
  %s2d = call double @strtod(i8* %tipo4, i8** null)
  %s2d5 = call double @strtod(i8* %NODO_PRINT, i8** null)
  %cmp = fcmp oeq double %s2d, %s2d5
  br i1 %cmp, label %then, label %endif

then:                                             ; preds = %entry
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr

endif:                                            ; preds = %entry
  %tipo6 = load i8*, i8** %tipo, align 8
  %NODO_ASSIGN = load i8*, i8** @g_NODO_ASSIGN, align 8
  %s2d7 = call double @strtod(i8* %tipo6, i8** null)
  %s2d8 = call double @strtod(i8* %NODO_ASSIGN, i8** null)
  %cmp9 = fcmp oeq double %s2d7, %s2d8
  br i1 %cmp9, label %then10, label %endif11

then10:                                           ; preds = %endif
  %numstr12 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr12

endif11:                                          ; preds = %endif
  %tipo13 = load i8*, i8** %tipo, align 8
  %NODO_IF = load i8*, i8** @g_NODO_IF, align 8
  %s2d14 = call double @strtod(i8* %tipo13, i8** null)
  %s2d15 = call double @strtod(i8* %NODO_IF, i8** null)
  %cmp16 = fcmp oeq double %s2d14, %s2d15
  br i1 %cmp16, label %then17, label %endif18

then17:                                           ; preds = %endif11
  %numstr19 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr19

endif18:                                          ; preds = %endif11
  %tipo20 = load i8*, i8** %tipo, align 8
  %NODO_WHILE = load i8*, i8** @g_NODO_WHILE, align 8
  %s2d21 = call double @strtod(i8* %tipo20, i8** null)
  %s2d22 = call double @strtod(i8* %NODO_WHILE, i8** null)
  %cmp23 = fcmp oeq double %s2d21, %s2d22
  br i1 %cmp23, label %then24, label %endif25

then24:                                           ; preds = %endif18
  %numstr26 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr26

endif25:                                          ; preds = %endif18
  %tipo27 = load i8*, i8** %tipo, align 8
  %NODO_FUNC_DECL = load i8*, i8** @g_NODO_FUNC_DECL, align 8
  %s2d28 = call double @strtod(i8* %tipo27, i8** null)
  %s2d29 = call double @strtod(i8* %NODO_FUNC_DECL, i8** null)
  %cmp30 = fcmp oeq double %s2d28, %s2d29
  br i1 %cmp30, label %then31, label %endif32

then31:                                           ; preds = %endif25
  %numstr33 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr33

endif32:                                          ; preds = %endif25
  %tipo34 = load i8*, i8** %tipo, align 8
  %NODO_RETURN = load i8*, i8** @g_NODO_RETURN, align 8
  %s2d35 = call double @strtod(i8* %tipo34, i8** null)
  %s2d36 = call double @strtod(i8* %NODO_RETURN, i8** null)
  %cmp37 = fcmp oeq double %s2d35, %s2d36
  br i1 %cmp37, label %then38, label %endif39

then38:                                           ; preds = %endif32
  %numstr40 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr40

endif39:                                          ; preds = %endif32
  ret i8* null
}

define i8* @codegen_programa(i8* %estado, i8* %programa) {
entry:
  %entry12 = alloca i8*, align 8
  %main_func = alloca i8*, align 8
  %main_type = alloca i8*, align 8
  %i32 = alloca i8*, align 8
  %estado1 = alloca i8*, align 8
  store i8* %estado, i8** %estado1, align 8
  %programa2 = alloca i8*, align 8
  store i8* %programa, i8** %programa2, align 8
  %estado3 = load i8*, i8** %estado1, align 8
  %call = call i8* @codegen_declarar_externs(i8* %estado3)
  %c_call = call i8* @LLVMInt32Type()
  store i8* %c_call, i8** %i32, align 8
  %i324 = load i8*, i8** %i32, align 8
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  %c_call5 = call i8* @LLVMFunctionType(i8* %i324, i8* %numstr, i32 0)
  store i8* %c_call5, i8** %main_type, align 8
  %estado6 = load i8*, i8** %estado1, align 8
  %call7 = call i8* @estado_modulo.1(i8* %estado6)
  %main_type8 = load i8*, i8** %main_type, align 8
  %c_call9 = call i8* @LLVMAddFunction(i8* %call7, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @7, i32 0, i32 0), i8* %main_type8)
  store i8* %c_call9, i8** %main_func, align 8
  %main_func10 = load i8*, i8** %main_func, align 8
  %c_call11 = call i8* @LLVMAppendBasicBlock(i8* %main_func10, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @8, i32 0, i32 0))
  store i8* %c_call11, i8** %entry12, align 8
  %estado13 = load i8*, i8** %estado1, align 8
  %call14 = call i8* @estado_builder.2(i8* %estado13)
  %entry15 = load i8*, i8** %entry12, align 8
  call void @LLVMPositionBuilderAtEnd(i8* %call14, i8* %entry15)
  %programa16 = load i8*, i8** %programa2, align 8
  %len = call i64 @__zen_list_len(i8* %programa16)
  %i = alloca i64, align 8
  store i64 0, i64* %i, align 4
  %stmt = alloca i8*, align 8
  store i8* null, i8** %stmt, align 8
  br label %fe.cond

fe.cond:                                          ; preds = %fe.inc, %entry
  %i17 = load i64, i64* %i, align 4
  %cmp = icmp slt i64 %i17, %len
  br i1 %cmp, label %fe.body, label %fe.end

fe.body:                                          ; preds = %fe.cond
  %elem = call i8* @__zen_list_get(i8* %programa16, i64 %i17)
  store i8* %elem, i8** %stmt, align 8
  %estado18 = load i8*, i8** %estado1, align 8
  %stmt19 = load i8*, i8** %stmt, align 8
  %call20 = call i8* @codegen_statement(i8* %estado18, i8* %stmt19)
  br label %fe.inc

fe.inc:                                           ; preds = %fe.body
  %next = add i64 %i17, 1
  store i64 %next, i64* %i, align 4
  br label %fe.cond

fe.end:                                           ; preds = %fe.cond
  %estado21 = load i8*, i8** %estado1, align 8
  %call22 = call i8* @estado_builder.2(i8* %estado21)
  %i3223 = load i8*, i8** %i32, align 8
  %c_call24 = call i8* @LLVMConstInt(i8* %i3223, i32 0, i32 0)
  %c_call25 = call i8* @LLVMBuildRet(i8* %call22, i8* %c_call24)
  ret i8* null
}

define i8* @codegen_compilar(i8* %estado, i8* %ruta_salida) {
entry:
  %estado1 = alloca i8*, align 8
  store i8* %estado, i8** %estado1, align 8
  %ruta_salida2 = alloca i8*, align 8
  store i8* %ruta_salida, i8** %ruta_salida2, align 8
  %estado3 = load i8*, i8** %estado1, align 8
  %call = call i8* @estado_modulo.1(i8* %estado3)
  %ruta_salida4 = load i8*, i8** %ruta_salida2, align 8
  %c_call = call i32 @LLVMPrintModuleToFile(i8* %call, i8* %ruta_salida4, i8* null)
  %c2d = sitofp i32 %c_call to double
  ret i8* null
}

define i8* @codegen_finalize(i8* %estado) {
entry:
  %estado1 = alloca i8*, align 8
  store i8* %estado, i8** %estado1, align 8
  %estado2 = load i8*, i8** %estado1, align 8
  %call = call i8* @estado_builder.2(i8* %estado2)
  call void @LLVMDisposeBuilder(i8* %call)
  %estado3 = load i8*, i8** %estado1, align 8
  %call4 = call i8* @estado_modulo.1(i8* %estado3)
  call void @LLVMDisposeModule(i8* %call4)
  ret i8* null
}

define i32 @main() {
entry:
  %numstr = call i8* @__zen_num_to_str(double 1.000000e+00)
  store i8* %numstr, i8** @g_NODO_NUMBER, align 8
  %numstr1 = call i8* @__zen_num_to_str(double 2.000000e+00)
  store i8* %numstr1, i8** @g_NODO_STRING, align 8
  %numstr2 = call i8* @__zen_num_to_str(double 3.000000e+00)
  store i8* %numstr2, i8** @g_NODO_BOOL, align 8
  %numstr3 = call i8* @__zen_num_to_str(double 4.000000e+00)
  store i8* %numstr3, i8** @g_NODO_NULL, align 8
  %numstr4 = call i8* @__zen_num_to_str(double 5.000000e+00)
  store i8* %numstr4, i8** @g_NODO_IDENTIFIER, align 8
  %numstr5 = call i8* @__zen_num_to_str(double 6.000000e+00)
  store i8* %numstr5, i8** @g_NODO_BINARY_OP, align 8
  %numstr6 = call i8* @__zen_num_to_str(double 7.000000e+00)
  store i8* %numstr6, i8** @g_NODO_UNARY_OP, align 8
  %numstr7 = call i8* @__zen_num_to_str(double 9.000000e+00)
  store i8* %numstr7, i8** @g_NODO_FUNC_CALL, align 8
  %numstr8 = call i8* @__zen_num_to_str(double 1.000000e+01)
  store i8* %numstr8, i8** @g_NODO_ASSIGN, align 8
  %numstr9 = call i8* @__zen_num_to_str(double 1.200000e+01)
  store i8* %numstr9, i8** @g_NODO_PRINT, align 8
  %numstr10 = call i8* @__zen_num_to_str(double 1.300000e+01)
  store i8* %numstr10, i8** @g_NODO_IF, align 8
  %numstr11 = call i8* @__zen_num_to_str(double 1.600000e+01)
  store i8* %numstr11, i8** @g_NODO_WHILE, align 8
  %numstr12 = call i8* @__zen_num_to_str(double 2.000000e+01)
  store i8* %numstr12, i8** @g_NODO_FUNC_DECL, align 8
  %numstr13 = call i8* @__zen_num_to_str(double 2.100000e+01)
  store i8* %numstr13, i8** @g_NODO_RETURN, align 8
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @10, i32 0, i32 0), i8* getelementptr inbounds ([38 x i8], [38 x i8]* @9, i32 0, i32 0))
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @12, i32 0, i32 0), i8* getelementptr inbounds ([23 x i8], [23 x i8]* @11, i32 0, i32 0))
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @14, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @13, i32 0, i32 0))
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @16, i32 0, i32 0), i8* getelementptr inbounds ([28 x i8], [28 x i8]* @15, i32 0, i32 0))
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @18, i32 0, i32 0), i8* getelementptr inbounds ([48 x i8], [48 x i8]* @17, i32 0, i32 0))
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @20, i32 0, i32 0), i8* getelementptr inbounds ([60 x i8], [60 x i8]* @19, i32 0, i32 0))
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @22, i32 0, i32 0), i8* getelementptr inbounds ([49 x i8], [49 x i8]* @21, i32 0, i32 0))
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @24, i32 0, i32 0), i8* getelementptr inbounds ([54 x i8], [54 x i8]* @23, i32 0, i32 0))
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @26, i32 0, i32 0), i8* getelementptr inbounds ([53 x i8], [53 x i8]* @25, i32 0, i32 0))
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @28, i32 0, i32 0), i8* getelementptr inbounds ([35 x i8], [35 x i8]* @27, i32 0, i32 0))
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @30, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @29, i32 0, i32 0))
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @32, i32 0, i32 0), i8* getelementptr inbounds ([19 x i8], [19 x i8]* @31, i32 0, i32 0))
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @34, i32 0, i32 0), i8* getelementptr inbounds ([29 x i8], [29 x i8]* @33, i32 0, i32 0))
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @36, i32 0, i32 0), i8* getelementptr inbounds ([37 x i8], [37 x i8]* @35, i32 0, i32 0))
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @38, i32 0, i32 0), i8* getelementptr inbounds ([50 x i8], [50 x i8]* @37, i32 0, i32 0))
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @40, i32 0, i32 0), i8* getelementptr inbounds ([36 x i8], [36 x i8]* @39, i32 0, i32 0))
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @42, i32 0, i32 0), i8* getelementptr inbounds ([39 x i8], [39 x i8]* @41, i32 0, i32 0))
  %17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @44, i32 0, i32 0), i8* getelementptr inbounds ([35 x i8], [35 x i8]* @43, i32 0, i32 0))
  %18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @46, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @45, i32 0, i32 0))
  %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @48, i32 0, i32 0), i8* getelementptr inbounds ([27 x i8], [27 x i8]* @47, i32 0, i32 0))
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @50, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @49, i32 0, i32 0))
  %21 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @52, i32 0, i32 0), i8* getelementptr inbounds ([61 x i8], [61 x i8]* @51, i32 0, i32 0))
  %22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @54, i32 0, i32 0), i8* getelementptr inbounds ([37 x i8], [37 x i8]* @53, i32 0, i32 0))
  ret i32 0
}
