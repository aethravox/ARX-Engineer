; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@3 = private unnamed_addr constant [4 x i8] c"zen\00", align 1
@4 = private unnamed_addr constant [7 x i8] c"printf\00", align 1
@5 = private unnamed_addr constant [7 x i8] c"malloc\00", align 1
@6 = private unnamed_addr constant [7 x i8] c"strlen\00", align 1
@7 = private unnamed_addr constant [8 x i8] c"sprintf\00", align 1
@8 = private unnamed_addr constant [7 x i8] c"strtod\00", align 1
@9 = private unnamed_addr constant [7 x i8] c"strcpy\00", align 1
@10 = private unnamed_addr constant [7 x i8] c"strcat\00", align 1
@11 = private unnamed_addr constant [5 x i8] c".str\00", align 1
@12 = private unnamed_addr constant [7 x i8] c"strptr\00", align 1
@13 = private unnamed_addr constant [2 x i8] c"+\00", align 1
@14 = private unnamed_addr constant [4 x i8] c"add\00", align 1
@15 = private unnamed_addr constant [2 x i8] c"-\00", align 1
@16 = private unnamed_addr constant [4 x i8] c"sub\00", align 1
@17 = private unnamed_addr constant [2 x i8] c"*\00", align 1
@18 = private unnamed_addr constant [4 x i8] c"mul\00", align 1
@19 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@20 = private unnamed_addr constant [4 x i8] c"div\00", align 1
@21 = private unnamed_addr constant [2 x i8] c"%\00", align 1
@22 = private unnamed_addr constant [4 x i8] c"mod\00", align 1
@23 = private unnamed_addr constant [3 x i8] c"==\00", align 1
@24 = private unnamed_addr constant [3 x i8] c"eq\00", align 1
@25 = private unnamed_addr constant [3 x i8] c"!=\00", align 1
@26 = private unnamed_addr constant [3 x i8] c"ne\00", align 1
@27 = private unnamed_addr constant [2 x i8] c"<\00", align 1
@28 = private unnamed_addr constant [3 x i8] c"lt\00", align 1
@29 = private unnamed_addr constant [2 x i8] c">\00", align 1
@30 = private unnamed_addr constant [3 x i8] c"gt\00", align 1
@31 = private unnamed_addr constant [3 x i8] c"<=\00", align 1
@32 = private unnamed_addr constant [3 x i8] c"le\00", align 1
@33 = private unnamed_addr constant [3 x i8] c">=\00", align 1
@34 = private unnamed_addr constant [3 x i8] c"ge\00", align 1
@35 = private unnamed_addr constant [2 x i8] c"-\00", align 1
@36 = private unnamed_addr constant [4 x i8] c"neg\00", align 1
@37 = private unnamed_addr constant [8 x i8] c"muestra\00", align 1
@38 = private unnamed_addr constant [5 x i8] c"show\00", align 1
@39 = private unnamed_addr constant [8 x i8] c"mostrar\00", align 1
@40 = private unnamed_addr constant [8 x i8] c"imprime\00", align 1
@41 = private unnamed_addr constant [4 x i8] c"%g\0A\00", align 1
@42 = private unnamed_addr constant [5 x i8] c".fmt\00", align 1
@43 = private unnamed_addr constant [7 x i8] c"fmtptr\00", align 1
@44 = private unnamed_addr constant [7 x i8] c"printf\00", align 1
@45 = private unnamed_addr constant [6 x i8] c"print\00", align 1
@46 = private unnamed_addr constant [9 x i8] c"longitud\00", align 1
@47 = private unnamed_addr constant [7 x i8] c"length\00", align 1
@48 = private unnamed_addr constant [4 x i8] c"len\00", align 1
@49 = private unnamed_addr constant [7 x i8] c"strlen\00", align 1
@50 = private unnamed_addr constant [4 x i8] c"len\00", align 1
@51 = private unnamed_addr constant [5 x i8] c"lend\00", align 1
@52 = private unnamed_addr constant [7 x i8] c"numero\00", align 1
@53 = private unnamed_addr constant [7 x i8] c"number\00", align 1
@54 = private unnamed_addr constant [4 x i8] c"num\00", align 1
@55 = private unnamed_addr constant [7 x i8] c"strtod\00", align 1
@56 = private unnamed_addr constant [4 x i8] c"num\00", align 1
@57 = private unnamed_addr constant [5 x i8] c"main\00", align 1
@58 = private unnamed_addr constant [6 x i8] c"entry\00", align 1
@59 = private unnamed_addr constant [38 x i8] c"=== Codegen de Zen escrito en Zen ===\00", align 1
@60 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@61 = private unnamed_addr constant [65 x i8] c"Nodos: NumberLit, StringLit, BoolLit, NullLit, BinaryOp, UnaryOp\00", align 1
@62 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@63 = private unnamed_addr constant [64 x i8] c"       FuncCall(muestra/longitud/numero), Print, Assign, Return\00", align 1
@64 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@65 = private unnamed_addr constant [57 x i8] c"       FuncDecl, ExprStmt, IfStmt(stub), WhileStmt(stub)\00", align 1
@66 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@67 = private unnamed_addr constant [26 x i8] c"FFI: 50+ funciones LLVM-C\00", align 1
@68 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

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

declare i8* @LLVMVoidType()

declare i8* @LLVMPointerType(i8*, i32)

declare i8* @LLVMFunctionType(i8*, i8*, i32)

declare i8* @LLVMAddFunction(i8*, i8*, i8*)

declare i8* @LLVMGetParam(i8*, i32)

declare i8* @LLVMAppendBasicBlock(i8*, i8*)

declare void @LLVMPositionBuilderAtEnd(i8*, i8*)

declare i8* @LLVMBuildRet(i8*, i8*)

declare i8* @LLVMBuildRetVoid(i8*)

declare i8* @LLVMBuildFAdd(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFSub(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFMul(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFDiv(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFRem(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildAlloca(i8*, i8*, i8*)

declare i8* @LLVMBuildStore(i8*, i8*, i8*)

declare i8* @LLVMBuildLoad(i8*, i8*, i8*)

declare i8* @LLVMBuildCall(i8*, i8*, i8*, i32, i8*)

declare i8* @LLVMConstReal(i8*, double)

declare i8* @LLVMConstString(i8*, i32, i32)

declare i8* @LLVMConstInt(i8*, i32, i32)

declare i8* @LLVMGetNamedFunction(i8*, i8*)

declare i8* @LLVMBuildFCmp(i8*, i32, i8*, i8*, i8*)

declare i8* @LLVMBuildBr(i8*, i8*)

declare i8* @LLVMBuildCondBr(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildNeg(i8*, i8*, i8*)

declare i8* @LLVMBuildNot(i8*, i8*, i8*)

declare i8* @LLVMBuildUIToFP(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildFPToSI(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildIntToPtr(i8*, i8*, i8*, i8*)

declare i8* @LLVMBuildBitCast(i8*, i8*, i8*, i8*)

declare i8* @LLVMConstPointerNull(i8*)

declare i8* @LLVMConstArray(i8*, i8*, i32)

declare i8* @LLVMAddGlobal(i8*, i8*, i8*)

declare void @LLVMSetInitializer(i8*, i8*)

declare void @LLVMSetGlobalConstant(i8*, i32)

declare void @LLVMSetLinkage(i8*, i32)

declare i8* @strcat(i8*, i8*)

declare i32 @sprintf(i8*, i8*, double)

define i8* @estado_modulo(i8* %e) {
entry:
  %e1 = alloca i8*, align 8
  store i8* %e, i8** %e1, align 8
  %e2 = load i8*, i8** %e1, align 8
  %member = call i8* @__zen_struct_get(i8* %e2, i64 0)
  ret i8* %member
}

define i8* @estado_builder(i8* %e) {
entry:
  %e1 = alloca i8*, align 8
  store i8* %e, i8** %e1, align 8
  %e2 = load i8*, i8** %e1, align 8
  %member = call i8* @__zen_struct_get(i8* %e2, i64 1)
  ret i8* %member
}

define i8* @i8ptr_type() {
entry:
  %c_call = call i8* @LLVMInt8Type()
  %c_call1 = call i8* @LLVMPointerType(i8* %c_call, i32 0)
  ret i8* %c_call1
}

define i8* @nodo_tipo(i8* %n) {
entry:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  %n2 = load i8*, i8** %n1, align 8
  %member = call i8* @__zen_struct_get(i8* %n2, i64 0)
  ret i8* %member
}

define i8* @nodo_valor(i8* %n) {
entry:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  %n2 = load i8*, i8** %n1, align 8
  %member = call i8* @__zen_struct_get(i8* %n2, i64 0)
  ret i8* %member
}

define i8* @nodo_nombre(i8* %n) {
entry:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  %n2 = load i8*, i8** %n1, align 8
  %member = call i8* @__zen_struct_get(i8* %n2, i64 1)
  ret i8* %member
}

define i8* @nodo_op(i8* %n) {
entry:
  %n1 = alloca i8*, align 8
  store i8* %n, i8** %n1, align 8
  %n2 = load i8*, i8** %n1, align 8
  %member = call i8* @__zen_struct_get(i8* %n2, i64 1)
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
  %struct = call i8* @__zen_struct_create(i64 2)
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_struct_set(i8* %struct, i64 0, i8* %numstr)
  %numstr1 = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_struct_set(i8* %struct, i64 1, i8* %numstr1)
  store i8* %struct, i8** %estado, align 8
  %estado2 = load i8*, i8** %estado, align 8
  %c_call = call i8* @LLVMModuleCreateWithName(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @3, i32 0, i32 0))
  call void @__zen_struct_set(i8* %estado2, i64 0, i8* %c_call)
  %estado3 = load i8*, i8** %estado, align 8
  %c_call4 = call i8* @LLVMCreateBuilder()
  call void @__zen_struct_set(i8* %estado3, i64 1, i8* %c_call4)
  %estado5 = load i8*, i8** %estado, align 8
  ret i8* %estado5
}

define i8* @codegen_declarar_externs(i8* %estado) {
entry:
  %strcat_type = alloca i8*, align 8
  %strcpy_type = alloca i8*, align 8
  %strtod_type = alloca i8*, align 8
  %sprintf_type = alloca i8*, align 8
  %strlen_type = alloca i8*, align 8
  %malloc_type = alloca i8*, align 8
  %printf_type = alloca i8*, align 8
  %i64 = alloca i8*, align 8
  %i32 = alloca i8*, align 8
  %i8p = alloca i8*, align 8
  %mod = alloca i8*, align 8
  %estado1 = alloca i8*, align 8
  store i8* %estado, i8** %estado1, align 8
  %estado2 = load i8*, i8** %estado1, align 8
  %call = call i8* @estado_modulo(i8* %estado2)
  store i8* %call, i8** %mod, align 8
  %call3 = call i8* @i8ptr_type()
  store i8* %call3, i8** %i8p, align 8
  %c_call = call i8* @LLVMInt32Type()
  store i8* %c_call, i8** %i32, align 8
  %c_call4 = call i8* @LLVMInt64Type()
  store i8* %c_call4, i8** %i64, align 8
  %i325 = load i8*, i8** %i32, align 8
  %i8p6 = load i8*, i8** %i8p, align 8
  %c_call7 = call i8* @LLVMFunctionType(i8* %i325, i8* %i8p6, i32 1)
  store i8* %c_call7, i8** %printf_type, align 8
  %mod8 = load i8*, i8** %mod, align 8
  %printf_type9 = load i8*, i8** %printf_type, align 8
  %c_call10 = call i8* @LLVMAddFunction(i8* %mod8, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @4, i32 0, i32 0), i8* %printf_type9)
  %i8p11 = load i8*, i8** %i8p, align 8
  %i6412 = load i8*, i8** %i64, align 8
  %c_call13 = call i8* @LLVMFunctionType(i8* %i8p11, i8* %i6412, i32 0)
  store i8* %c_call13, i8** %malloc_type, align 8
  %mod14 = load i8*, i8** %mod, align 8
  %malloc_type15 = load i8*, i8** %malloc_type, align 8
  %c_call16 = call i8* @LLVMAddFunction(i8* %mod14, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @5, i32 0, i32 0), i8* %malloc_type15)
  %i6417 = load i8*, i8** %i64, align 8
  %i8p18 = load i8*, i8** %i8p, align 8
  %c_call19 = call i8* @LLVMFunctionType(i8* %i6417, i8* %i8p18, i32 0)
  store i8* %c_call19, i8** %strlen_type, align 8
  %mod20 = load i8*, i8** %mod, align 8
  %strlen_type21 = load i8*, i8** %strlen_type, align 8
  %c_call22 = call i8* @LLVMAddFunction(i8* %mod20, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @6, i32 0, i32 0), i8* %strlen_type21)
  %i3223 = load i8*, i8** %i32, align 8
  %i8p24 = load i8*, i8** %i8p, align 8
  %c_call25 = call i8* @LLVMFunctionType(i8* %i3223, i8* %i8p24, i32 0)
  store i8* %c_call25, i8** %sprintf_type, align 8
  %mod26 = load i8*, i8** %mod, align 8
  %sprintf_type27 = load i8*, i8** %sprintf_type, align 8
  %c_call28 = call i8* @LLVMAddFunction(i8* %mod26, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @7, i32 0, i32 0), i8* %sprintf_type27)
  %c_call29 = call i8* @LLVMDoubleType()
  %i8p30 = load i8*, i8** %i8p, align 8
  %c_call31 = call i8* @LLVMFunctionType(i8* %c_call29, i8* %i8p30, i32 0)
  store i8* %c_call31, i8** %strtod_type, align 8
  %mod32 = load i8*, i8** %mod, align 8
  %strtod_type33 = load i8*, i8** %strtod_type, align 8
  %c_call34 = call i8* @LLVMAddFunction(i8* %mod32, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @8, i32 0, i32 0), i8* %strtod_type33)
  %i8p35 = load i8*, i8** %i8p, align 8
  %i8p36 = load i8*, i8** %i8p, align 8
  %c_call37 = call i8* @LLVMFunctionType(i8* %i8p35, i8* %i8p36, i32 0)
  store i8* %c_call37, i8** %strcpy_type, align 8
  %mod38 = load i8*, i8** %mod, align 8
  %strcpy_type39 = load i8*, i8** %strcpy_type, align 8
  %c_call40 = call i8* @LLVMAddFunction(i8* %mod38, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @9, i32 0, i32 0), i8* %strcpy_type39)
  %i8p41 = load i8*, i8** %i8p, align 8
  %i8p42 = load i8*, i8** %i8p, align 8
  %c_call43 = call i8* @LLVMFunctionType(i8* %i8p41, i8* %i8p42, i32 0)
  store i8* %c_call43, i8** %strcat_type, align 8
  %mod44 = load i8*, i8** %mod, align 8
  %strcat_type45 = load i8*, i8** %strcat_type, align 8
  %c_call46 = call i8* @LLVMAddFunction(i8* %mod44, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @10, i32 0, i32 0), i8* %strcat_type45)
  ret i8* null
}

define i8* @codegen_expresion(i8* %estado, i8* %nodo) {
entry:
  %nullp = alloca i8*, align 8
  %strtod_fn = alloca i8*, align 8
  %strlen_fn = alloca i8*, align 8
  %args = alloca i8*, align 8
  %printf_fn = alloca i8*, align 8
  %fmt_ptr = alloca i8*, align 8
  %fmt = alloca i8*, align 8
  %arg = alloca i8*, align 8
  %nombre = alloca i8*, align 8
  %operand = alloca i8*, align 8
  %b = alloca i8*, align 8
  %right = alloca i8*, align 8
  %left = alloca i8*, align 8
  %hijos = alloca i8*, align 8
  %op = alloca i8*, align 8
  %val = alloca i8*, align 8
  %gvar = alloca i8*, align 8
  %ret = alloca i8*, align 8
  %str_val = alloca i8*, align 8
  %tipo = alloca i8*, align 8
  %estado1 = alloca i8*, align 8
  store i8* %estado, i8** %estado1, align 8
  %nodo2 = alloca i8*, align 8
  store i8* %nodo, i8** %nodo2, align 8
  %nodo3 = load i8*, i8** %nodo2, align 8
  %call = call i8* @nodo_tipo(i8* %nodo3)
  store i8* %call, i8** %tipo, align 8
  %tipo4 = load i8*, i8** %tipo, align 8
  %s2d = call double @strtod(i8* %tipo4, i8** null)
  %cmp = fcmp oeq double %s2d, 1.000000e+00
  br i1 %cmp, label %then, label %endif

then:                                             ; preds = %entry
  %c_call = call i8* @LLVMDoubleType()
  %nodo5 = load i8*, i8** %nodo2, align 8
  %call6 = call i8* @nodo_valor(i8* %nodo5)
  %num = call double @strtod(i8* %call6, i8** null)
  %c_call7 = call i8* @LLVMConstReal(i8* %c_call, double %num)
  ret i8* %c_call7

endif:                                            ; preds = %entry
  %tipo8 = load i8*, i8** %tipo, align 8
  %s2d9 = call double @strtod(i8* %tipo8, i8** null)
  %cmp10 = fcmp oeq double %s2d9, 2.000000e+00
  br i1 %cmp10, label %then11, label %endif12

then11:                                           ; preds = %endif
  %nodo13 = load i8*, i8** %nodo2, align 8
  %call14 = call i8* @nodo_valor(i8* %nodo13)
  store i8* %call14, i8** %str_val, align 8
  %str_val15 = load i8*, i8** %str_val, align 8
  %c_call16 = call i8* @LLVMConstString(i8* %str_val15, i32 0, i32 0)
  store i8* %c_call16, i8** %ret, align 8
  %estado17 = load i8*, i8** %estado1, align 8
  %call18 = call i8* @estado_modulo(i8* %estado17)
  %c_call19 = call i8* @LLVMInt8Type()
  %c_call20 = call i8* @LLVMAddGlobal(i8* %call18, i8* %c_call19, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @11, i32 0, i32 0))
  store i8* %c_call20, i8** %gvar, align 8
  %gvar21 = load i8*, i8** %gvar, align 8
  %ret22 = load i8*, i8** %ret, align 8
  call void @LLVMSetInitializer(i8* %gvar21, i8* %ret22)
  %gvar23 = load i8*, i8** %gvar, align 8
  call void @LLVMSetGlobalConstant(i8* %gvar23, i32 1)
  %gvar24 = load i8*, i8** %gvar, align 8
  call void @LLVMSetLinkage(i8* %gvar24, i32 0)
  %estado25 = load i8*, i8** %estado1, align 8
  %call26 = call i8* @estado_builder(i8* %estado25)
  %gvar27 = load i8*, i8** %gvar, align 8
  %call28 = call i8* @i8ptr_type()
  %c_call29 = call i8* @LLVMBuildBitCast(i8* %call26, i8* %gvar27, i8* %call28, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @12, i32 0, i32 0))
  ret i8* %c_call29

endif12:                                          ; preds = %endif
  %tipo30 = load i8*, i8** %tipo, align 8
  %s2d31 = call double @strtod(i8* %tipo30, i8** null)
  %cmp32 = fcmp oeq double %s2d31, 3.000000e+00
  br i1 %cmp32, label %then33, label %endif34

then33:                                           ; preds = %endif12
  %nodo35 = load i8*, i8** %nodo2, align 8
  %call36 = call i8* @nodo_valor(i8* %nodo35)
  %num37 = call double @strtod(i8* %call36, i8** null)
  %numstr = call i8* @__zen_num_to_str(double %num37)
  store i8* %numstr, i8** %val, align 8
  %val38 = load i8*, i8** %val, align 8
  %s2d39 = call double @strtod(i8* %val38, i8** null)
  %cmp40 = fcmp oeq double %s2d39, 0.000000e+00
  br i1 %cmp40, label %then41, label %endif42

endif34:                                          ; preds = %endif12
  %tipo47 = load i8*, i8** %tipo, align 8
  %s2d48 = call double @strtod(i8* %tipo47, i8** null)
  %cmp49 = fcmp oeq double %s2d48, 4.000000e+00
  br i1 %cmp49, label %then50, label %endif51

then41:                                           ; preds = %then33
  %c_call43 = call i8* @LLVMInt1Type()
  %c_call44 = call i8* @LLVMConstInt(i8* %c_call43, i32 0, i32 0)
  ret i8* %c_call44

endif42:                                          ; preds = %then33
  %c_call45 = call i8* @LLVMInt1Type()
  %c_call46 = call i8* @LLVMConstInt(i8* %c_call45, i32 1, i32 0)
  ret i8* %c_call46

then50:                                           ; preds = %endif34
  %call52 = call i8* @i8ptr_type()
  %c_call53 = call i8* @LLVMConstPointerNull(i8* %call52)
  ret i8* %c_call53

endif51:                                          ; preds = %endif34
  %tipo54 = load i8*, i8** %tipo, align 8
  %s2d55 = call double @strtod(i8* %tipo54, i8** null)
  %cmp56 = fcmp oeq double %s2d55, 5.000000e+00
  br i1 %cmp56, label %then57, label %endif58

then57:                                           ; preds = %endif51
  %numstr59 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr59

endif58:                                          ; preds = %endif51
  %tipo60 = load i8*, i8** %tipo, align 8
  %s2d61 = call double @strtod(i8* %tipo60, i8** null)
  %cmp62 = fcmp oeq double %s2d61, 6.000000e+00
  br i1 %cmp62, label %then63, label %endif64

then63:                                           ; preds = %endif58
  %nodo65 = load i8*, i8** %nodo2, align 8
  %call66 = call i8* @nodo_op(i8* %nodo65)
  store i8* %call66, i8** %op, align 8
  %nodo67 = load i8*, i8** %nodo2, align 8
  %member = call i8* @__zen_struct_get(i8* %nodo67, i64 1)
  store i8* %member, i8** %hijos, align 8
  %estado68 = load i8*, i8** %estado1, align 8
  %hijos69 = load i8*, i8** %hijos, align 8
  %elem = call i8* @__zen_list_get(i8* %hijos69, i64 0)
  %call70 = call i8* @codegen_expresion(i8* %estado68, i8* %elem)
  store i8* %call70, i8** %left, align 8
  %estado71 = load i8*, i8** %estado1, align 8
  %hijos72 = load i8*, i8** %hijos, align 8
  %elem73 = call i8* @__zen_list_get(i8* %hijos72, i64 1)
  %call74 = call i8* @codegen_expresion(i8* %estado71, i8* %elem73)
  store i8* %call74, i8** %right, align 8
  %estado75 = load i8*, i8** %estado1, align 8
  %call76 = call i8* @estado_builder(i8* %estado75)
  store i8* %call76, i8** %b, align 8
  %op77 = load i8*, i8** %op, align 8
  %strcmp = call i32 @strcmp(i8* %op77, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @13, i32 0, i32 0))
  %scmp = icmp eq i32 %strcmp, 0
  br i1 %scmp, label %then78, label %endif79

endif64:                                          ; preds = %endif58
  %tipo175 = load i8*, i8** %tipo, align 8
  %s2d176 = call double @strtod(i8* %tipo175, i8** null)
  %cmp177 = fcmp oeq double %s2d176, 7.000000e+00
  br i1 %cmp177, label %then178, label %endif179

then78:                                           ; preds = %then63
  %b80 = load i8*, i8** %b, align 8
  %left81 = load i8*, i8** %left, align 8
  %right82 = load i8*, i8** %right, align 8
  %c_call83 = call i8* @LLVMBuildFAdd(i8* %b80, i8* %left81, i8* %right82, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @14, i32 0, i32 0))
  ret i8* %c_call83

endif79:                                          ; preds = %then63
  %op84 = load i8*, i8** %op, align 8
  %strcmp85 = call i32 @strcmp(i8* %op84, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @15, i32 0, i32 0))
  %scmp86 = icmp eq i32 %strcmp85, 0
  br i1 %scmp86, label %then87, label %endif88

then87:                                           ; preds = %endif79
  %b89 = load i8*, i8** %b, align 8
  %left90 = load i8*, i8** %left, align 8
  %right91 = load i8*, i8** %right, align 8
  %c_call92 = call i8* @LLVMBuildFSub(i8* %b89, i8* %left90, i8* %right91, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @16, i32 0, i32 0))
  ret i8* %c_call92

endif88:                                          ; preds = %endif79
  %op93 = load i8*, i8** %op, align 8
  %strcmp94 = call i32 @strcmp(i8* %op93, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @17, i32 0, i32 0))
  %scmp95 = icmp eq i32 %strcmp94, 0
  br i1 %scmp95, label %then96, label %endif97

then96:                                           ; preds = %endif88
  %b98 = load i8*, i8** %b, align 8
  %left99 = load i8*, i8** %left, align 8
  %right100 = load i8*, i8** %right, align 8
  %c_call101 = call i8* @LLVMBuildFMul(i8* %b98, i8* %left99, i8* %right100, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @18, i32 0, i32 0))
  ret i8* %c_call101

endif97:                                          ; preds = %endif88
  %op102 = load i8*, i8** %op, align 8
  %strcmp103 = call i32 @strcmp(i8* %op102, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @19, i32 0, i32 0))
  %scmp104 = icmp eq i32 %strcmp103, 0
  br i1 %scmp104, label %then105, label %endif106

then105:                                          ; preds = %endif97
  %b107 = load i8*, i8** %b, align 8
  %left108 = load i8*, i8** %left, align 8
  %right109 = load i8*, i8** %right, align 8
  %c_call110 = call i8* @LLVMBuildFDiv(i8* %b107, i8* %left108, i8* %right109, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @20, i32 0, i32 0))
  ret i8* %c_call110

endif106:                                         ; preds = %endif97
  %op111 = load i8*, i8** %op, align 8
  %strcmp112 = call i32 @strcmp(i8* %op111, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @21, i32 0, i32 0))
  %scmp113 = icmp eq i32 %strcmp112, 0
  br i1 %scmp113, label %then114, label %endif115

then114:                                          ; preds = %endif106
  %b116 = load i8*, i8** %b, align 8
  %left117 = load i8*, i8** %left, align 8
  %right118 = load i8*, i8** %right, align 8
  %c_call119 = call i8* @LLVMBuildFRem(i8* %b116, i8* %left117, i8* %right118, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @22, i32 0, i32 0))
  ret i8* %c_call119

endif115:                                         ; preds = %endif106
  %op120 = load i8*, i8** %op, align 8
  %strcmp121 = call i32 @strcmp(i8* %op120, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @23, i32 0, i32 0))
  %scmp122 = icmp eq i32 %strcmp121, 0
  br i1 %scmp122, label %then123, label %endif124

then123:                                          ; preds = %endif115
  %b125 = load i8*, i8** %b, align 8
  %left126 = load i8*, i8** %left, align 8
  %right127 = load i8*, i8** %right, align 8
  %c_call128 = call i8* @LLVMBuildFCmp(i8* %b125, i32 1, i8* %left126, i8* %right127, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @24, i32 0, i32 0))
  ret i8* %c_call128

endif124:                                         ; preds = %endif115
  %op129 = load i8*, i8** %op, align 8
  %strcmp130 = call i32 @strcmp(i8* %op129, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @25, i32 0, i32 0))
  %scmp131 = icmp eq i32 %strcmp130, 0
  br i1 %scmp131, label %then132, label %endif133

then132:                                          ; preds = %endif124
  %b134 = load i8*, i8** %b, align 8
  %left135 = load i8*, i8** %left, align 8
  %right136 = load i8*, i8** %right, align 8
  %c_call137 = call i8* @LLVMBuildFCmp(i8* %b134, i32 4, i8* %left135, i8* %right136, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @26, i32 0, i32 0))
  ret i8* %c_call137

endif133:                                         ; preds = %endif124
  %op138 = load i8*, i8** %op, align 8
  %strcmp139 = call i32 @strcmp(i8* %op138, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @27, i32 0, i32 0))
  %scmp140 = icmp eq i32 %strcmp139, 0
  br i1 %scmp140, label %then141, label %endif142

then141:                                          ; preds = %endif133
  %b143 = load i8*, i8** %b, align 8
  %left144 = load i8*, i8** %left, align 8
  %right145 = load i8*, i8** %right, align 8
  %c_call146 = call i8* @LLVMBuildFCmp(i8* %b143, i32 2, i8* %left144, i8* %right145, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @28, i32 0, i32 0))
  ret i8* %c_call146

endif142:                                         ; preds = %endif133
  %op147 = load i8*, i8** %op, align 8
  %strcmp148 = call i32 @strcmp(i8* %op147, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @29, i32 0, i32 0))
  %scmp149 = icmp eq i32 %strcmp148, 0
  br i1 %scmp149, label %then150, label %endif151

then150:                                          ; preds = %endif142
  %b152 = load i8*, i8** %b, align 8
  %left153 = load i8*, i8** %left, align 8
  %right154 = load i8*, i8** %right, align 8
  %c_call155 = call i8* @LLVMBuildFCmp(i8* %b152, i32 3, i8* %left153, i8* %right154, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @30, i32 0, i32 0))
  ret i8* %c_call155

endif151:                                         ; preds = %endif142
  %op156 = load i8*, i8** %op, align 8
  %strcmp157 = call i32 @strcmp(i8* %op156, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @31, i32 0, i32 0))
  %scmp158 = icmp eq i32 %strcmp157, 0
  br i1 %scmp158, label %then159, label %endif160

then159:                                          ; preds = %endif151
  %b161 = load i8*, i8** %b, align 8
  %left162 = load i8*, i8** %left, align 8
  %right163 = load i8*, i8** %right, align 8
  %c_call164 = call i8* @LLVMBuildFCmp(i8* %b161, i32 6, i8* %left162, i8* %right163, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @32, i32 0, i32 0))
  ret i8* %c_call164

endif160:                                         ; preds = %endif151
  %op165 = load i8*, i8** %op, align 8
  %strcmp166 = call i32 @strcmp(i8* %op165, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @33, i32 0, i32 0))
  %scmp167 = icmp eq i32 %strcmp166, 0
  br i1 %scmp167, label %then168, label %endif169

then168:                                          ; preds = %endif160
  %b170 = load i8*, i8** %b, align 8
  %left171 = load i8*, i8** %left, align 8
  %right172 = load i8*, i8** %right, align 8
  %c_call173 = call i8* @LLVMBuildFCmp(i8* %b170, i32 5, i8* %left171, i8* %right172, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @34, i32 0, i32 0))
  ret i8* %c_call173

endif169:                                         ; preds = %endif160
  %numstr174 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr174

then178:                                          ; preds = %endif64
  %nodo180 = load i8*, i8** %nodo2, align 8
  %call181 = call i8* @nodo_op(i8* %nodo180)
  store i8* %call181, i8** %op, align 8
  %nodo182 = load i8*, i8** %nodo2, align 8
  %member183 = call i8* @__zen_struct_get(i8* %nodo182, i64 1)
  store i8* %member183, i8** %hijos, align 8
  %estado184 = load i8*, i8** %estado1, align 8
  %hijos185 = load i8*, i8** %hijos, align 8
  %elem186 = call i8* @__zen_list_get(i8* %hijos185, i64 0)
  %call187 = call i8* @codegen_expresion(i8* %estado184, i8* %elem186)
  store i8* %call187, i8** %operand, align 8
  %estado188 = load i8*, i8** %estado1, align 8
  %call189 = call i8* @estado_builder(i8* %estado188)
  store i8* %call189, i8** %b, align 8
  %op190 = load i8*, i8** %op, align 8
  %strcmp191 = call i32 @strcmp(i8* %op190, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @35, i32 0, i32 0))
  %scmp192 = icmp eq i32 %strcmp191, 0
  br i1 %scmp192, label %then193, label %endif194

endif179:                                         ; preds = %endif64
  %tipo199 = load i8*, i8** %tipo, align 8
  %s2d200 = call double @strtod(i8* %tipo199, i8** null)
  %cmp201 = fcmp oeq double %s2d200, 9.000000e+00
  br i1 %cmp201, label %then202, label %endif203

then193:                                          ; preds = %then178
  %b195 = load i8*, i8** %b, align 8
  %operand196 = load i8*, i8** %operand, align 8
  %c_call197 = call i8* @LLVMBuildNeg(i8* %b195, i8* %operand196, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @36, i32 0, i32 0))
  ret i8* %c_call197

endif194:                                         ; preds = %then178
  %operand198 = load i8*, i8** %operand, align 8
  ret i8* %operand198

then202:                                          ; preds = %endif179
  %nodo204 = load i8*, i8** %nodo2, align 8
  %call205 = call i8* @nodo_nombre(i8* %nodo204)
  store i8* %call205, i8** %nombre, align 8
  %nodo206 = load i8*, i8** %nodo2, align 8
  %member207 = call i8* @__zen_struct_get(i8* %nodo206, i64 1)
  store i8* %member207, i8** %hijos, align 8
  %estado208 = load i8*, i8** %estado1, align 8
  %call209 = call i8* @estado_builder(i8* %estado208)
  store i8* %call209, i8** %b, align 8
  %nombre210 = load i8*, i8** %nombre, align 8
  %strcmp211 = call i32 @strcmp(i8* %nombre210, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @37, i32 0, i32 0))
  %scmp212 = icmp eq i32 %strcmp211, 0
  %nombre213 = load i8*, i8** %nombre, align 8
  %strcmp214 = call i32 @strcmp(i8* %nombre213, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @38, i32 0, i32 0))
  %scmp215 = icmp eq i32 %strcmp214, 0
  %or = or i1 %scmp212, %scmp215
  %nombre216 = load i8*, i8** %nombre, align 8
  %strcmp217 = call i32 @strcmp(i8* %nombre216, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @39, i32 0, i32 0))
  %scmp218 = icmp eq i32 %strcmp217, 0
  %or219 = or i1 %or, %scmp218
  %nombre220 = load i8*, i8** %nombre, align 8
  %strcmp221 = call i32 @strcmp(i8* %nombre220, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @40, i32 0, i32 0))
  %scmp222 = icmp eq i32 %strcmp221, 0
  %or223 = or i1 %or219, %scmp222
  br i1 %or223, label %then224, label %endif225

endif203:                                         ; preds = %endif179
  %numstr313 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr313

then224:                                          ; preds = %then202
  %estado226 = load i8*, i8** %estado1, align 8
  %hijos227 = load i8*, i8** %hijos, align 8
  %elem228 = call i8* @__zen_list_get(i8* %hijos227, i64 0)
  %call229 = call i8* @codegen_expresion(i8* %estado226, i8* %elem228)
  store i8* %call229, i8** %arg, align 8
  %c_call230 = call i8* @LLVMConstString(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @41, i32 0, i32 0), i32 0, i32 0)
  store i8* %c_call230, i8** %fmt, align 8
  %estado231 = load i8*, i8** %estado1, align 8
  %call232 = call i8* @estado_modulo(i8* %estado231)
  %c_call233 = call i8* @LLVMInt8Type()
  %c_call234 = call i8* @LLVMAddGlobal(i8* %call232, i8* %c_call233, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @42, i32 0, i32 0))
  store i8* %c_call234, i8** %gvar, align 8
  %gvar235 = load i8*, i8** %gvar, align 8
  %fmt236 = load i8*, i8** %fmt, align 8
  call void @LLVMSetInitializer(i8* %gvar235, i8* %fmt236)
  %gvar237 = load i8*, i8** %gvar, align 8
  call void @LLVMSetGlobalConstant(i8* %gvar237, i32 1)
  %gvar238 = load i8*, i8** %gvar, align 8
  call void @LLVMSetLinkage(i8* %gvar238, i32 0)
  %b239 = load i8*, i8** %b, align 8
  %gvar240 = load i8*, i8** %gvar, align 8
  %call241 = call i8* @i8ptr_type()
  %c_call242 = call i8* @LLVMBuildBitCast(i8* %b239, i8* %gvar240, i8* %call241, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @43, i32 0, i32 0))
  store i8* %c_call242, i8** %fmt_ptr, align 8
  %estado243 = load i8*, i8** %estado1, align 8
  %call244 = call i8* @estado_modulo(i8* %estado243)
  %c_call245 = call i8* @LLVMGetNamedFunction(i8* %call244, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @44, i32 0, i32 0))
  store i8* %c_call245, i8** %printf_fn, align 8
  %list = call i8* @__zen_list_create(i64 4)
  %fmt_ptr246 = load i8*, i8** %fmt_ptr, align 8
  call void @__zen_list_push(i8* %list, i8* %fmt_ptr246)
  %arg247 = load i8*, i8** %arg, align 8
  call void @__zen_list_push(i8* %list, i8* %arg247)
  store i8* %list, i8** %args, align 8
  %b248 = load i8*, i8** %b, align 8
  %printf_fn249 = load i8*, i8** %printf_fn, align 8
  %args250 = load i8*, i8** %args, align 8
  %c_call251 = call i8* @LLVMBuildCall(i8* %b248, i8* %printf_fn249, i8* %args250, i32 2, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @45, i32 0, i32 0))
  ret i8* %c_call251

endif225:                                         ; preds = %then202
  %nombre252 = load i8*, i8** %nombre, align 8
  %strcmp253 = call i32 @strcmp(i8* %nombre252, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @46, i32 0, i32 0))
  %scmp254 = icmp eq i32 %strcmp253, 0
  %nombre255 = load i8*, i8** %nombre, align 8
  %strcmp256 = call i32 @strcmp(i8* %nombre255, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @47, i32 0, i32 0))
  %scmp257 = icmp eq i32 %strcmp256, 0
  %or258 = or i1 %scmp254, %scmp257
  %nombre259 = load i8*, i8** %nombre, align 8
  %strcmp260 = call i32 @strcmp(i8* %nombre259, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @48, i32 0, i32 0))
  %scmp261 = icmp eq i32 %strcmp260, 0
  %or262 = or i1 %or258, %scmp261
  br i1 %or262, label %then263, label %endif264

then263:                                          ; preds = %endif225
  %estado265 = load i8*, i8** %estado1, align 8
  %hijos266 = load i8*, i8** %hijos, align 8
  %elem267 = call i8* @__zen_list_get(i8* %hijos266, i64 0)
  %call268 = call i8* @codegen_expresion(i8* %estado265, i8* %elem267)
  store i8* %call268, i8** %arg, align 8
  %estado269 = load i8*, i8** %estado1, align 8
  %call270 = call i8* @estado_modulo(i8* %estado269)
  %c_call271 = call i8* @LLVMGetNamedFunction(i8* %call270, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @49, i32 0, i32 0))
  store i8* %c_call271, i8** %strlen_fn, align 8
  %list272 = call i8* @__zen_list_create(i64 4)
  %arg273 = load i8*, i8** %arg, align 8
  call void @__zen_list_push(i8* %list272, i8* %arg273)
  store i8* %list272, i8** %args, align 8
  %b274 = load i8*, i8** %b, align 8
  %strlen_fn275 = load i8*, i8** %strlen_fn, align 8
  %args276 = load i8*, i8** %args, align 8
  %c_call277 = call i8* @LLVMBuildCall(i8* %b274, i8* %strlen_fn275, i8* %args276, i32 1, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @50, i32 0, i32 0))
  store i8* %c_call277, i8** %ret, align 8
  %b278 = load i8*, i8** %b, align 8
  %ret279 = load i8*, i8** %ret, align 8
  %c_call280 = call i8* @LLVMDoubleType()
  %c_call281 = call i8* @LLVMBuildUIToFP(i8* %b278, i8* %ret279, i8* %c_call280, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @51, i32 0, i32 0))
  ret i8* %c_call281

endif264:                                         ; preds = %endif225
  %nombre282 = load i8*, i8** %nombre, align 8
  %strcmp283 = call i32 @strcmp(i8* %nombre282, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @52, i32 0, i32 0))
  %scmp284 = icmp eq i32 %strcmp283, 0
  %nombre285 = load i8*, i8** %nombre, align 8
  %strcmp286 = call i32 @strcmp(i8* %nombre285, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @53, i32 0, i32 0))
  %scmp287 = icmp eq i32 %strcmp286, 0
  %or288 = or i1 %scmp284, %scmp287
  %nombre289 = load i8*, i8** %nombre, align 8
  %strcmp290 = call i32 @strcmp(i8* %nombre289, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @54, i32 0, i32 0))
  %scmp291 = icmp eq i32 %strcmp290, 0
  %or292 = or i1 %or288, %scmp291
  br i1 %or292, label %then293, label %endif294

then293:                                          ; preds = %endif264
  %estado295 = load i8*, i8** %estado1, align 8
  %hijos296 = load i8*, i8** %hijos, align 8
  %elem297 = call i8* @__zen_list_get(i8* %hijos296, i64 0)
  %call298 = call i8* @codegen_expresion(i8* %estado295, i8* %elem297)
  store i8* %call298, i8** %arg, align 8
  %estado299 = load i8*, i8** %estado1, align 8
  %call300 = call i8* @estado_modulo(i8* %estado299)
  %c_call301 = call i8* @LLVMGetNamedFunction(i8* %call300, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @55, i32 0, i32 0))
  store i8* %c_call301, i8** %strtod_fn, align 8
  %call302 = call i8* @i8ptr_type()
  %c_call303 = call i8* @LLVMPointerType(i8* %call302, i32 0)
  %c_call304 = call i8* @LLVMConstPointerNull(i8* %c_call303)
  store i8* %c_call304, i8** %nullp, align 8
  %list305 = call i8* @__zen_list_create(i64 4)
  %arg306 = load i8*, i8** %arg, align 8
  call void @__zen_list_push(i8* %list305, i8* %arg306)
  %nullp307 = load i8*, i8** %nullp, align 8
  call void @__zen_list_push(i8* %list305, i8* %nullp307)
  store i8* %list305, i8** %args, align 8
  %b308 = load i8*, i8** %b, align 8
  %strtod_fn309 = load i8*, i8** %strtod_fn, align 8
  %args310 = load i8*, i8** %args, align 8
  %c_call311 = call i8* @LLVMBuildCall(i8* %b308, i8* %strtod_fn309, i8* %args310, i32 2, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @56, i32 0, i32 0))
  ret i8* %c_call311

endif294:                                         ; preds = %endif264
  %numstr312 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr312
}

define i8* @codegen_statement(i8* %estado, i8* %nodo) {
entry:
  %alloca = alloca i8*, align 8
  %nombre = alloca i8*, align 8
  %val = alloca i8*, align 8
  %hijos = alloca i8*, align 8
  %mod = alloca i8*, align 8
  %b = alloca i8*, align 8
  %tipo = alloca i8*, align 8
  %estado1 = alloca i8*, align 8
  store i8* %estado, i8** %estado1, align 8
  %nodo2 = alloca i8*, align 8
  store i8* %nodo, i8** %nodo2, align 8
  %nodo3 = load i8*, i8** %nodo2, align 8
  %call = call i8* @nodo_tipo(i8* %nodo3)
  store i8* %call, i8** %tipo, align 8
  %estado4 = load i8*, i8** %estado1, align 8
  %call5 = call i8* @estado_builder(i8* %estado4)
  store i8* %call5, i8** %b, align 8
  %estado6 = load i8*, i8** %estado1, align 8
  %call7 = call i8* @estado_modulo(i8* %estado6)
  store i8* %call7, i8** %mod, align 8
  %tipo8 = load i8*, i8** %tipo, align 8
  %s2d = call double @strtod(i8* %tipo8, i8** null)
  %cmp = fcmp oeq double %s2d, 1.200000e+01
  br i1 %cmp, label %then, label %endif

then:                                             ; preds = %entry
  %nodo9 = load i8*, i8** %nodo2, align 8
  %member = call i8* @__zen_struct_get(i8* %nodo9, i64 1)
  store i8* %member, i8** %hijos, align 8
  %estado10 = load i8*, i8** %estado1, align 8
  %hijos11 = load i8*, i8** %hijos, align 8
  %elem = call i8* @__zen_list_get(i8* %hijos11, i64 0)
  %call12 = call i8* @codegen_expresion(i8* %estado10, i8* %elem)
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr

endif:                                            ; preds = %entry
  %tipo13 = load i8*, i8** %tipo, align 8
  %s2d14 = call double @strtod(i8* %tipo13, i8** null)
  %cmp15 = fcmp oeq double %s2d14, 1.000000e+01
  br i1 %cmp15, label %then16, label %endif17

then16:                                           ; preds = %endif
  %nodo18 = load i8*, i8** %nodo2, align 8
  %member19 = call i8* @__zen_struct_get(i8* %nodo18, i64 1)
  store i8* %member19, i8** %hijos, align 8
  %estado20 = load i8*, i8** %estado1, align 8
  %hijos21 = load i8*, i8** %hijos, align 8
  %elem22 = call i8* @__zen_list_get(i8* %hijos21, i64 0)
  %call23 = call i8* @codegen_expresion(i8* %estado20, i8* %elem22)
  store i8* %call23, i8** %val, align 8
  %nodo24 = load i8*, i8** %nodo2, align 8
  %call25 = call i8* @nodo_nombre(i8* %nodo24)
  store i8* %call25, i8** %nombre, align 8
  %b26 = load i8*, i8** %b, align 8
  %c_call = call i8* @LLVMDoubleType()
  %nombre27 = load i8*, i8** %nombre, align 8
  %c_call28 = call i8* @LLVMBuildAlloca(i8* %b26, i8* %c_call, i8* %nombre27)
  store i8* %c_call28, i8** %alloca, align 8
  %b29 = load i8*, i8** %b, align 8
  %val30 = load i8*, i8** %val, align 8
  %alloca31 = load i8*, i8** %alloca, align 8
  %c_call32 = call i8* @LLVMBuildStore(i8* %b29, i8* %val30, i8* %alloca31)
  %numstr33 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr33

endif17:                                          ; preds = %endif
  %tipo34 = load i8*, i8** %tipo, align 8
  %s2d35 = call double @strtod(i8* %tipo34, i8** null)
  %cmp36 = fcmp oeq double %s2d35, 2.100000e+01
  br i1 %cmp36, label %then37, label %endif38

then37:                                           ; preds = %endif17
  %nodo39 = load i8*, i8** %nodo2, align 8
  %member40 = call i8* @__zen_struct_get(i8* %nodo39, i64 1)
  store i8* %member40, i8** %hijos, align 8
  %estado41 = load i8*, i8** %estado1, align 8
  %hijos42 = load i8*, i8** %hijos, align 8
  %elem43 = call i8* @__zen_list_get(i8* %hijos42, i64 0)
  %call44 = call i8* @codegen_expresion(i8* %estado41, i8* %elem43)
  store i8* %call44, i8** %val, align 8
  %b45 = load i8*, i8** %b, align 8
  %val46 = load i8*, i8** %val, align 8
  %c_call47 = call i8* @LLVMBuildRet(i8* %b45, i8* %val46)
  %numstr48 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr48

endif38:                                          ; preds = %endif17
  %tipo49 = load i8*, i8** %tipo, align 8
  %s2d50 = call double @strtod(i8* %tipo49, i8** null)
  %cmp51 = fcmp oeq double %s2d50, 2.000000e+01
  br i1 %cmp51, label %then52, label %endif53

then52:                                           ; preds = %endif38
  %numstr54 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr54

endif53:                                          ; preds = %endif38
  %tipo55 = load i8*, i8** %tipo, align 8
  %s2d56 = call double @strtod(i8* %tipo55, i8** null)
  %cmp57 = fcmp oeq double %s2d56, 2.200000e+01
  br i1 %cmp57, label %then58, label %endif59

then58:                                           ; preds = %endif53
  %nodo60 = load i8*, i8** %nodo2, align 8
  %member61 = call i8* @__zen_struct_get(i8* %nodo60, i64 1)
  store i8* %member61, i8** %hijos, align 8
  %estado62 = load i8*, i8** %estado1, align 8
  %hijos63 = load i8*, i8** %hijos, align 8
  %elem64 = call i8* @__zen_list_get(i8* %hijos63, i64 0)
  %call65 = call i8* @codegen_expresion(i8* %estado62, i8* %elem64)
  %numstr66 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr66

endif59:                                          ; preds = %endif53
  %tipo67 = load i8*, i8** %tipo, align 8
  %s2d68 = call double @strtod(i8* %tipo67, i8** null)
  %cmp69 = fcmp oeq double %s2d68, 1.300000e+01
  br i1 %cmp69, label %then70, label %endif71

then70:                                           ; preds = %endif59
  %numstr72 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr72

endif71:                                          ; preds = %endif59
  %tipo73 = load i8*, i8** %tipo, align 8
  %s2d74 = call double @strtod(i8* %tipo73, i8** null)
  %cmp75 = fcmp oeq double %s2d74, 1.600000e+01
  br i1 %cmp75, label %then76, label %endif77

then76:                                           ; preds = %endif71
  %numstr78 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr78

endif77:                                          ; preds = %endif71
  %tipo79 = load i8*, i8** %tipo, align 8
  %s2d80 = call double @strtod(i8* %tipo79, i8** null)
  %cmp81 = fcmp oeq double %s2d80, 1.400000e+01
  br i1 %cmp81, label %then82, label %endif83

then82:                                           ; preds = %endif77
  %numstr84 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr84

endif83:                                          ; preds = %endif77
  %tipo85 = load i8*, i8** %tipo, align 8
  %s2d86 = call double @strtod(i8* %tipo85, i8** null)
  %cmp87 = fcmp oeq double %s2d86, 1.800000e+01
  br i1 %cmp87, label %then88, label %endif89

then88:                                           ; preds = %endif83
  %numstr90 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr90

endif89:                                          ; preds = %endif83
  %tipo91 = load i8*, i8** %tipo, align 8
  %s2d92 = call double @strtod(i8* %tipo91, i8** null)
  %cmp93 = fcmp oeq double %s2d92, 1.900000e+01
  br i1 %cmp93, label %then94, label %endif95

then94:                                           ; preds = %endif89
  %numstr96 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr96

endif95:                                          ; preds = %endif89
  %numstr97 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr97
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
  %call7 = call i8* @estado_modulo(i8* %estado6)
  %main_type8 = load i8*, i8** %main_type, align 8
  %c_call9 = call i8* @LLVMAddFunction(i8* %call7, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @57, i32 0, i32 0), i8* %main_type8)
  store i8* %c_call9, i8** %main_func, align 8
  %main_func10 = load i8*, i8** %main_func, align 8
  %c_call11 = call i8* @LLVMAppendBasicBlock(i8* %main_func10, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @58, i32 0, i32 0))
  store i8* %c_call11, i8** %entry12, align 8
  %estado13 = load i8*, i8** %estado1, align 8
  %call14 = call i8* @estado_builder(i8* %estado13)
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
  %call22 = call i8* @estado_builder(i8* %estado21)
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
  %call = call i8* @estado_modulo(i8* %estado3)
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
  %call = call i8* @estado_builder(i8* %estado2)
  call void @LLVMDisposeBuilder(i8* %call)
  %estado3 = load i8*, i8** %estado1, align 8
  %call4 = call i8* @estado_modulo(i8* %estado3)
  call void @LLVMDisposeModule(i8* %call4)
  ret i8* null
}

define i32 @main() {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @60, i32 0, i32 0), i8* getelementptr inbounds ([38 x i8], [38 x i8]* @59, i32 0, i32 0))
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @62, i32 0, i32 0), i8* getelementptr inbounds ([65 x i8], [65 x i8]* @61, i32 0, i32 0))
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @64, i32 0, i32 0), i8* getelementptr inbounds ([64 x i8], [64 x i8]* @63, i32 0, i32 0))
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @66, i32 0, i32 0), i8* getelementptr inbounds ([57 x i8], [57 x i8]* @65, i32 0, i32 0))
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @68, i32 0, i32 0), i8* getelementptr inbounds ([26 x i8], [26 x i8]* @67, i32 0, i32 0))
  ret i32 0
}
