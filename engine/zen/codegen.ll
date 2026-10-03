; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@__zen_argc = internal global i32 0
@__zen_argv = internal global i8** null
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
@13 = private unnamed_addr constant [5 x i8] c"call\00", align 1
@14 = private unnamed_addr constant [2 x i8] c"+\00", align 1
@15 = private unnamed_addr constant [4 x i8] c"add\00", align 1
@16 = private unnamed_addr constant [2 x i8] c"-\00", align 1
@17 = private unnamed_addr constant [4 x i8] c"sub\00", align 1
@18 = private unnamed_addr constant [2 x i8] c"*\00", align 1
@19 = private unnamed_addr constant [4 x i8] c"mul\00", align 1
@20 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@21 = private unnamed_addr constant [4 x i8] c"div\00", align 1
@22 = private unnamed_addr constant [2 x i8] c"%\00", align 1
@23 = private unnamed_addr constant [4 x i8] c"mod\00", align 1
@24 = private unnamed_addr constant [3 x i8] c"==\00", align 1
@25 = private unnamed_addr constant [3 x i8] c"eq\00", align 1
@26 = private unnamed_addr constant [3 x i8] c"!=\00", align 1
@27 = private unnamed_addr constant [3 x i8] c"ne\00", align 1
@28 = private unnamed_addr constant [2 x i8] c"<\00", align 1
@29 = private unnamed_addr constant [3 x i8] c"lt\00", align 1
@30 = private unnamed_addr constant [2 x i8] c">\00", align 1
@31 = private unnamed_addr constant [3 x i8] c"gt\00", align 1
@32 = private unnamed_addr constant [3 x i8] c"<=\00", align 1
@33 = private unnamed_addr constant [3 x i8] c"le\00", align 1
@34 = private unnamed_addr constant [3 x i8] c">=\00", align 1
@35 = private unnamed_addr constant [3 x i8] c"ge\00", align 1
@36 = private unnamed_addr constant [2 x i8] c"-\00", align 1
@37 = private unnamed_addr constant [4 x i8] c"neg\00", align 1
@38 = private unnamed_addr constant [8 x i8] c"muestra\00", align 1
@39 = private unnamed_addr constant [5 x i8] c"show\00", align 1
@40 = private unnamed_addr constant [8 x i8] c"mostrar\00", align 1
@41 = private unnamed_addr constant [8 x i8] c"imprime\00", align 1
@42 = private unnamed_addr constant [4 x i8] c"%g\0A\00", align 1
@43 = private unnamed_addr constant [5 x i8] c".fmt\00", align 1
@44 = private unnamed_addr constant [7 x i8] c"fmtptr\00", align 1
@45 = private unnamed_addr constant [7 x i8] c"printf\00", align 1
@46 = private unnamed_addr constant [6 x i8] c"print\00", align 1
@47 = private unnamed_addr constant [9 x i8] c"longitud\00", align 1
@48 = private unnamed_addr constant [7 x i8] c"length\00", align 1
@49 = private unnamed_addr constant [4 x i8] c"len\00", align 1
@50 = private unnamed_addr constant [7 x i8] c"strlen\00", align 1
@51 = private unnamed_addr constant [4 x i8] c"len\00", align 1
@52 = private unnamed_addr constant [5 x i8] c"lend\00", align 1
@53 = private unnamed_addr constant [7 x i8] c"numero\00", align 1
@54 = private unnamed_addr constant [7 x i8] c"number\00", align 1
@55 = private unnamed_addr constant [4 x i8] c"num\00", align 1
@56 = private unnamed_addr constant [7 x i8] c"strtod\00", align 1
@57 = private unnamed_addr constant [4 x i8] c"num\00", align 1
@58 = private unnamed_addr constant [6 x i8] c"entry\00", align 1
@59 = private unnamed_addr constant [5 x i8] c"main\00", align 1
@60 = private unnamed_addr constant [7 x i8] c"resume\00", align 1
@61 = private unnamed_addr constant [5 x i8] c"main\00", align 1
@62 = private unnamed_addr constant [5 x i8] c"then\00", align 1
@63 = private unnamed_addr constant [5 x i8] c"else\00", align 1
@64 = private unnamed_addr constant [6 x i8] c"merge\00", align 1
@65 = private unnamed_addr constant [5 x i8] c"main\00", align 1
@66 = private unnamed_addr constant [11 x i8] c"while.cond\00", align 1
@67 = private unnamed_addr constant [11 x i8] c"while.body\00", align 1
@68 = private unnamed_addr constant [10 x i8] c"while.end\00", align 1
@69 = private unnamed_addr constant [5 x i8] c"main\00", align 1
@70 = private unnamed_addr constant [6 x i8] c"entry\00", align 1
@71 = private unnamed_addr constant [38 x i8] c"=== Codegen de Zen escrito en Zen ===\00", align 1
@72 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@73 = private unnamed_addr constant [65 x i8] c"Nodos: NumberLit, StringLit, BoolLit, NullLit, BinaryOp, UnaryOp\00", align 1
@74 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@75 = private unnamed_addr constant [64 x i8] c"       FuncCall(muestra/longitud/numero), Print, Assign, Return\00", align 1
@76 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@77 = private unnamed_addr constant [72 x i8] c"       FuncDecl(crear+body), ExprStmt, IfStmt(COND BR), WhileStmt(LOOP)\00", align 1
@78 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@79 = private unnamed_addr constant [26 x i8] c"FFI: 50+ funciones LLVM-C\00", align 1
@80 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

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

declare i8* @LLVMBuildPhi(i8*, i8*)

declare void @LLVMAddIncoming(i8*, i8*, i8*, i32, i8*)

declare i8* @LLVMGetInsertBlock(i8*)

declare i8* @LLVMBuildCast(i8*, i32, i8*, i8*)

declare i8* @LLVMConstBitCast(i8*, i8*)

declare i8* @LLVMBuildGEP(i8*, i8*, i8*, i32, i8*)

declare i8* @LLVMTypeOf(i8*)

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
  %printf_fn = alloca i8*, align 8
  %fmt_ptr = alloca i8*, align 8
  %fmt = alloca i8*, align 8
  %arg = alloca i8*, align 8
  %operand = alloca i8*, align 8
  %b = alloca i8*, align 8
  %right = alloca i8*, align 8
  %left = alloca i8*, align 8
  %hijos = alloca i8*, align 8
  %op = alloca i8*, align 8
  %args = alloca i8*, align 8
  %func = alloca i8*, align 8
  %nombre = alloca i8*, align 8
  %val = alloca double, align 8
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
  store double %num37, double* %val, align 8
  %val38 = load double, double* %val, align 8
  %cmp39 = fcmp oeq double %val38, 0.000000e+00
  br i1 %cmp39, label %then40, label %endif41

endif34:                                          ; preds = %endif12
  %tipo46 = load i8*, i8** %tipo, align 8
  %s2d47 = call double @strtod(i8* %tipo46, i8** null)
  %cmp48 = fcmp oeq double %s2d47, 4.000000e+00
  br i1 %cmp48, label %then49, label %endif50

then40:                                           ; preds = %then33
  %c_call42 = call i8* @LLVMInt1Type()
  %c_call43 = call i8* @LLVMConstInt(i8* %c_call42, i32 0, i32 0)
  ret i8* %c_call43

endif41:                                          ; preds = %then33
  %c_call44 = call i8* @LLVMInt1Type()
  %c_call45 = call i8* @LLVMConstInt(i8* %c_call44, i32 1, i32 0)
  ret i8* %c_call45

then49:                                           ; preds = %endif34
  %call51 = call i8* @i8ptr_type()
  %c_call52 = call i8* @LLVMConstPointerNull(i8* %call51)
  ret i8* %c_call52

endif50:                                          ; preds = %endif34
  %tipo53 = load i8*, i8** %tipo, align 8
  %s2d54 = call double @strtod(i8* %tipo53, i8** null)
  %cmp55 = fcmp oeq double %s2d54, 5.000000e+00
  br i1 %cmp55, label %then56, label %endif57

then56:                                           ; preds = %endif50
  %nodo58 = load i8*, i8** %nodo2, align 8
  %call59 = call i8* @nodo_valor(i8* %nodo58)
  store i8* %call59, i8** %nombre, align 8
  %estado60 = load i8*, i8** %estado1, align 8
  %call61 = call i8* @estado_modulo(i8* %estado60)
  %nombre62 = load i8*, i8** %nombre, align 8
  %c_call63 = call i8* @LLVMGetNamedFunction(i8* %call61, i8* %nombre62)
  store i8* %c_call63, i8** %func, align 8
  %func64 = load i8*, i8** %func, align 8
  %s2d65 = call double @strtod(i8* %func64, i8** null)
  %cmp66 = fcmp one double %s2d65, 0.000000e+00
  br i1 %cmp66, label %then67, label %endif68

endif57:                                          ; preds = %endif50
  %tipo75 = load i8*, i8** %tipo, align 8
  %s2d76 = call double @strtod(i8* %tipo75, i8** null)
  %cmp77 = fcmp oeq double %s2d76, 6.000000e+00
  br i1 %cmp77, label %then78, label %endif79

then67:                                           ; preds = %then56
  %list = call i8* @__zen_list_create(i64 4)
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_list_push(i8* %list, i8* %numstr)
  store i8* %list, i8** %args, align 8
  %estado69 = load i8*, i8** %estado1, align 8
  %call70 = call i8* @estado_builder(i8* %estado69)
  %func71 = load i8*, i8** %func, align 8
  %args72 = load i8*, i8** %args, align 8
  %c_call73 = call i8* @LLVMBuildCall(i8* %call70, i8* %func71, i8* %args72, i32 0, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @13, i32 0, i32 0))
  ret i8* %c_call73

endif68:                                          ; preds = %then56
  %numstr74 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr74

then78:                                           ; preds = %endif57
  %nodo80 = load i8*, i8** %nodo2, align 8
  %call81 = call i8* @nodo_op(i8* %nodo80)
  store i8* %call81, i8** %op, align 8
  %nodo82 = load i8*, i8** %nodo2, align 8
  %member = call i8* @__zen_struct_get(i8* %nodo82, i64 1)
  store i8* %member, i8** %hijos, align 8
  %estado83 = load i8*, i8** %estado1, align 8
  %hijos84 = load i8*, i8** %hijos, align 8
  %elem = call i8* @__zen_list_get(i8* %hijos84, i64 0)
  %call85 = call i8* @codegen_expresion(i8* %estado83, i8* %elem)
  store i8* %call85, i8** %left, align 8
  %estado86 = load i8*, i8** %estado1, align 8
  %hijos87 = load i8*, i8** %hijos, align 8
  %elem88 = call i8* @__zen_list_get(i8* %hijos87, i64 1)
  %call89 = call i8* @codegen_expresion(i8* %estado86, i8* %elem88)
  store i8* %call89, i8** %right, align 8
  %estado90 = load i8*, i8** %estado1, align 8
  %call91 = call i8* @estado_builder(i8* %estado90)
  store i8* %call91, i8** %b, align 8
  %op92 = load i8*, i8** %op, align 8
  %strcmp = call i32 @strcmp(i8* %op92, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @14, i32 0, i32 0))
  %scmp = icmp eq i32 %strcmp, 0
  br i1 %scmp, label %then93, label %endif94

endif79:                                          ; preds = %endif57
  %tipo190 = load i8*, i8** %tipo, align 8
  %s2d191 = call double @strtod(i8* %tipo190, i8** null)
  %cmp192 = fcmp oeq double %s2d191, 7.000000e+00
  br i1 %cmp192, label %then193, label %endif194

then93:                                           ; preds = %then78
  %b95 = load i8*, i8** %b, align 8
  %left96 = load i8*, i8** %left, align 8
  %right97 = load i8*, i8** %right, align 8
  %c_call98 = call i8* @LLVMBuildFAdd(i8* %b95, i8* %left96, i8* %right97, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @15, i32 0, i32 0))
  ret i8* %c_call98

endif94:                                          ; preds = %then78
  %op99 = load i8*, i8** %op, align 8
  %strcmp100 = call i32 @strcmp(i8* %op99, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @16, i32 0, i32 0))
  %scmp101 = icmp eq i32 %strcmp100, 0
  br i1 %scmp101, label %then102, label %endif103

then102:                                          ; preds = %endif94
  %b104 = load i8*, i8** %b, align 8
  %left105 = load i8*, i8** %left, align 8
  %right106 = load i8*, i8** %right, align 8
  %c_call107 = call i8* @LLVMBuildFSub(i8* %b104, i8* %left105, i8* %right106, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @17, i32 0, i32 0))
  ret i8* %c_call107

endif103:                                         ; preds = %endif94
  %op108 = load i8*, i8** %op, align 8
  %strcmp109 = call i32 @strcmp(i8* %op108, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @18, i32 0, i32 0))
  %scmp110 = icmp eq i32 %strcmp109, 0
  br i1 %scmp110, label %then111, label %endif112

then111:                                          ; preds = %endif103
  %b113 = load i8*, i8** %b, align 8
  %left114 = load i8*, i8** %left, align 8
  %right115 = load i8*, i8** %right, align 8
  %c_call116 = call i8* @LLVMBuildFMul(i8* %b113, i8* %left114, i8* %right115, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @19, i32 0, i32 0))
  ret i8* %c_call116

endif112:                                         ; preds = %endif103
  %op117 = load i8*, i8** %op, align 8
  %strcmp118 = call i32 @strcmp(i8* %op117, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @20, i32 0, i32 0))
  %scmp119 = icmp eq i32 %strcmp118, 0
  br i1 %scmp119, label %then120, label %endif121

then120:                                          ; preds = %endif112
  %b122 = load i8*, i8** %b, align 8
  %left123 = load i8*, i8** %left, align 8
  %right124 = load i8*, i8** %right, align 8
  %c_call125 = call i8* @LLVMBuildFDiv(i8* %b122, i8* %left123, i8* %right124, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @21, i32 0, i32 0))
  ret i8* %c_call125

endif121:                                         ; preds = %endif112
  %op126 = load i8*, i8** %op, align 8
  %strcmp127 = call i32 @strcmp(i8* %op126, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @22, i32 0, i32 0))
  %scmp128 = icmp eq i32 %strcmp127, 0
  br i1 %scmp128, label %then129, label %endif130

then129:                                          ; preds = %endif121
  %b131 = load i8*, i8** %b, align 8
  %left132 = load i8*, i8** %left, align 8
  %right133 = load i8*, i8** %right, align 8
  %c_call134 = call i8* @LLVMBuildFRem(i8* %b131, i8* %left132, i8* %right133, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @23, i32 0, i32 0))
  ret i8* %c_call134

endif130:                                         ; preds = %endif121
  %op135 = load i8*, i8** %op, align 8
  %strcmp136 = call i32 @strcmp(i8* %op135, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @24, i32 0, i32 0))
  %scmp137 = icmp eq i32 %strcmp136, 0
  br i1 %scmp137, label %then138, label %endif139

then138:                                          ; preds = %endif130
  %b140 = load i8*, i8** %b, align 8
  %left141 = load i8*, i8** %left, align 8
  %right142 = load i8*, i8** %right, align 8
  %c_call143 = call i8* @LLVMBuildFCmp(i8* %b140, i32 1, i8* %left141, i8* %right142, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @25, i32 0, i32 0))
  ret i8* %c_call143

endif139:                                         ; preds = %endif130
  %op144 = load i8*, i8** %op, align 8
  %strcmp145 = call i32 @strcmp(i8* %op144, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @26, i32 0, i32 0))
  %scmp146 = icmp eq i32 %strcmp145, 0
  br i1 %scmp146, label %then147, label %endif148

then147:                                          ; preds = %endif139
  %b149 = load i8*, i8** %b, align 8
  %left150 = load i8*, i8** %left, align 8
  %right151 = load i8*, i8** %right, align 8
  %c_call152 = call i8* @LLVMBuildFCmp(i8* %b149, i32 4, i8* %left150, i8* %right151, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @27, i32 0, i32 0))
  ret i8* %c_call152

endif148:                                         ; preds = %endif139
  %op153 = load i8*, i8** %op, align 8
  %strcmp154 = call i32 @strcmp(i8* %op153, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @28, i32 0, i32 0))
  %scmp155 = icmp eq i32 %strcmp154, 0
  br i1 %scmp155, label %then156, label %endif157

then156:                                          ; preds = %endif148
  %b158 = load i8*, i8** %b, align 8
  %left159 = load i8*, i8** %left, align 8
  %right160 = load i8*, i8** %right, align 8
  %c_call161 = call i8* @LLVMBuildFCmp(i8* %b158, i32 2, i8* %left159, i8* %right160, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @29, i32 0, i32 0))
  ret i8* %c_call161

endif157:                                         ; preds = %endif148
  %op162 = load i8*, i8** %op, align 8
  %strcmp163 = call i32 @strcmp(i8* %op162, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @30, i32 0, i32 0))
  %scmp164 = icmp eq i32 %strcmp163, 0
  br i1 %scmp164, label %then165, label %endif166

then165:                                          ; preds = %endif157
  %b167 = load i8*, i8** %b, align 8
  %left168 = load i8*, i8** %left, align 8
  %right169 = load i8*, i8** %right, align 8
  %c_call170 = call i8* @LLVMBuildFCmp(i8* %b167, i32 3, i8* %left168, i8* %right169, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @31, i32 0, i32 0))
  ret i8* %c_call170

endif166:                                         ; preds = %endif157
  %op171 = load i8*, i8** %op, align 8
  %strcmp172 = call i32 @strcmp(i8* %op171, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @32, i32 0, i32 0))
  %scmp173 = icmp eq i32 %strcmp172, 0
  br i1 %scmp173, label %then174, label %endif175

then174:                                          ; preds = %endif166
  %b176 = load i8*, i8** %b, align 8
  %left177 = load i8*, i8** %left, align 8
  %right178 = load i8*, i8** %right, align 8
  %c_call179 = call i8* @LLVMBuildFCmp(i8* %b176, i32 6, i8* %left177, i8* %right178, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @33, i32 0, i32 0))
  ret i8* %c_call179

endif175:                                         ; preds = %endif166
  %op180 = load i8*, i8** %op, align 8
  %strcmp181 = call i32 @strcmp(i8* %op180, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @34, i32 0, i32 0))
  %scmp182 = icmp eq i32 %strcmp181, 0
  br i1 %scmp182, label %then183, label %endif184

then183:                                          ; preds = %endif175
  %b185 = load i8*, i8** %b, align 8
  %left186 = load i8*, i8** %left, align 8
  %right187 = load i8*, i8** %right, align 8
  %c_call188 = call i8* @LLVMBuildFCmp(i8* %b185, i32 5, i8* %left186, i8* %right187, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @35, i32 0, i32 0))
  ret i8* %c_call188

endif184:                                         ; preds = %endif175
  %numstr189 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr189

then193:                                          ; preds = %endif79
  %nodo195 = load i8*, i8** %nodo2, align 8
  %call196 = call i8* @nodo_op(i8* %nodo195)
  store i8* %call196, i8** %op, align 8
  %nodo197 = load i8*, i8** %nodo2, align 8
  %member198 = call i8* @__zen_struct_get(i8* %nodo197, i64 1)
  store i8* %member198, i8** %hijos, align 8
  %estado199 = load i8*, i8** %estado1, align 8
  %hijos200 = load i8*, i8** %hijos, align 8
  %elem201 = call i8* @__zen_list_get(i8* %hijos200, i64 0)
  %call202 = call i8* @codegen_expresion(i8* %estado199, i8* %elem201)
  store i8* %call202, i8** %operand, align 8
  %estado203 = load i8*, i8** %estado1, align 8
  %call204 = call i8* @estado_builder(i8* %estado203)
  store i8* %call204, i8** %b, align 8
  %op205 = load i8*, i8** %op, align 8
  %strcmp206 = call i32 @strcmp(i8* %op205, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @36, i32 0, i32 0))
  %scmp207 = icmp eq i32 %strcmp206, 0
  br i1 %scmp207, label %then208, label %endif209

endif194:                                         ; preds = %endif79
  %tipo214 = load i8*, i8** %tipo, align 8
  %s2d215 = call double @strtod(i8* %tipo214, i8** null)
  %cmp216 = fcmp oeq double %s2d215, 9.000000e+00
  br i1 %cmp216, label %then217, label %endif218

then208:                                          ; preds = %then193
  %b210 = load i8*, i8** %b, align 8
  %operand211 = load i8*, i8** %operand, align 8
  %c_call212 = call i8* @LLVMBuildNeg(i8* %b210, i8* %operand211, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @37, i32 0, i32 0))
  ret i8* %c_call212

endif209:                                         ; preds = %then193
  %operand213 = load i8*, i8** %operand, align 8
  ret i8* %operand213

then217:                                          ; preds = %endif194
  %nodo219 = load i8*, i8** %nodo2, align 8
  %call220 = call i8* @nodo_nombre(i8* %nodo219)
  store i8* %call220, i8** %nombre, align 8
  %nodo221 = load i8*, i8** %nodo2, align 8
  %member222 = call i8* @__zen_struct_get(i8* %nodo221, i64 1)
  store i8* %member222, i8** %hijos, align 8
  %estado223 = load i8*, i8** %estado1, align 8
  %call224 = call i8* @estado_builder(i8* %estado223)
  store i8* %call224, i8** %b, align 8
  %nombre225 = load i8*, i8** %nombre, align 8
  %strcmp226 = call i32 @strcmp(i8* %nombre225, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @38, i32 0, i32 0))
  %scmp227 = icmp eq i32 %strcmp226, 0
  %nombre228 = load i8*, i8** %nombre, align 8
  %strcmp229 = call i32 @strcmp(i8* %nombre228, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @39, i32 0, i32 0))
  %scmp230 = icmp eq i32 %strcmp229, 0
  %or = or i1 %scmp227, %scmp230
  %nombre231 = load i8*, i8** %nombre, align 8
  %strcmp232 = call i32 @strcmp(i8* %nombre231, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @40, i32 0, i32 0))
  %scmp233 = icmp eq i32 %strcmp232, 0
  %or234 = or i1 %or, %scmp233
  %nombre235 = load i8*, i8** %nombre, align 8
  %strcmp236 = call i32 @strcmp(i8* %nombre235, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @41, i32 0, i32 0))
  %scmp237 = icmp eq i32 %strcmp236, 0
  %or238 = or i1 %or234, %scmp237
  br i1 %or238, label %then239, label %endif240

endif218:                                         ; preds = %endif194
  %numstr329 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr329

then239:                                          ; preds = %then217
  %estado241 = load i8*, i8** %estado1, align 8
  %hijos242 = load i8*, i8** %hijos, align 8
  %elem243 = call i8* @__zen_list_get(i8* %hijos242, i64 0)
  %call244 = call i8* @codegen_expresion(i8* %estado241, i8* %elem243)
  store i8* %call244, i8** %arg, align 8
  %c_call245 = call i8* @LLVMConstString(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @42, i32 0, i32 0), i32 0, i32 0)
  store i8* %c_call245, i8** %fmt, align 8
  %estado246 = load i8*, i8** %estado1, align 8
  %call247 = call i8* @estado_modulo(i8* %estado246)
  %c_call248 = call i8* @LLVMInt8Type()
  %c_call249 = call i8* @LLVMAddGlobal(i8* %call247, i8* %c_call248, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @43, i32 0, i32 0))
  store i8* %c_call249, i8** %gvar, align 8
  %gvar250 = load i8*, i8** %gvar, align 8
  %fmt251 = load i8*, i8** %fmt, align 8
  call void @LLVMSetInitializer(i8* %gvar250, i8* %fmt251)
  %gvar252 = load i8*, i8** %gvar, align 8
  call void @LLVMSetGlobalConstant(i8* %gvar252, i32 1)
  %gvar253 = load i8*, i8** %gvar, align 8
  call void @LLVMSetLinkage(i8* %gvar253, i32 0)
  %b254 = load i8*, i8** %b, align 8
  %gvar255 = load i8*, i8** %gvar, align 8
  %call256 = call i8* @i8ptr_type()
  %c_call257 = call i8* @LLVMBuildBitCast(i8* %b254, i8* %gvar255, i8* %call256, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @44, i32 0, i32 0))
  store i8* %c_call257, i8** %fmt_ptr, align 8
  %estado258 = load i8*, i8** %estado1, align 8
  %call259 = call i8* @estado_modulo(i8* %estado258)
  %c_call260 = call i8* @LLVMGetNamedFunction(i8* %call259, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @45, i32 0, i32 0))
  store i8* %c_call260, i8** %printf_fn, align 8
  %list261 = call i8* @__zen_list_create(i64 4)
  %fmt_ptr262 = load i8*, i8** %fmt_ptr, align 8
  call void @__zen_list_push(i8* %list261, i8* %fmt_ptr262)
  %arg263 = load i8*, i8** %arg, align 8
  call void @__zen_list_push(i8* %list261, i8* %arg263)
  store i8* %list261, i8** %args, align 8
  %b264 = load i8*, i8** %b, align 8
  %printf_fn265 = load i8*, i8** %printf_fn, align 8
  %args266 = load i8*, i8** %args, align 8
  %c_call267 = call i8* @LLVMBuildCall(i8* %b264, i8* %printf_fn265, i8* %args266, i32 2, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @46, i32 0, i32 0))
  ret i8* %c_call267

endif240:                                         ; preds = %then217
  %nombre268 = load i8*, i8** %nombre, align 8
  %strcmp269 = call i32 @strcmp(i8* %nombre268, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @47, i32 0, i32 0))
  %scmp270 = icmp eq i32 %strcmp269, 0
  %nombre271 = load i8*, i8** %nombre, align 8
  %strcmp272 = call i32 @strcmp(i8* %nombre271, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @48, i32 0, i32 0))
  %scmp273 = icmp eq i32 %strcmp272, 0
  %or274 = or i1 %scmp270, %scmp273
  %nombre275 = load i8*, i8** %nombre, align 8
  %strcmp276 = call i32 @strcmp(i8* %nombre275, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @49, i32 0, i32 0))
  %scmp277 = icmp eq i32 %strcmp276, 0
  %or278 = or i1 %or274, %scmp277
  br i1 %or278, label %then279, label %endif280

then279:                                          ; preds = %endif240
  %estado281 = load i8*, i8** %estado1, align 8
  %hijos282 = load i8*, i8** %hijos, align 8
  %elem283 = call i8* @__zen_list_get(i8* %hijos282, i64 0)
  %call284 = call i8* @codegen_expresion(i8* %estado281, i8* %elem283)
  store i8* %call284, i8** %arg, align 8
  %estado285 = load i8*, i8** %estado1, align 8
  %call286 = call i8* @estado_modulo(i8* %estado285)
  %c_call287 = call i8* @LLVMGetNamedFunction(i8* %call286, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @50, i32 0, i32 0))
  store i8* %c_call287, i8** %strlen_fn, align 8
  %list288 = call i8* @__zen_list_create(i64 4)
  %arg289 = load i8*, i8** %arg, align 8
  call void @__zen_list_push(i8* %list288, i8* %arg289)
  store i8* %list288, i8** %args, align 8
  %b290 = load i8*, i8** %b, align 8
  %strlen_fn291 = load i8*, i8** %strlen_fn, align 8
  %args292 = load i8*, i8** %args, align 8
  %c_call293 = call i8* @LLVMBuildCall(i8* %b290, i8* %strlen_fn291, i8* %args292, i32 1, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @51, i32 0, i32 0))
  store i8* %c_call293, i8** %ret, align 8
  %b294 = load i8*, i8** %b, align 8
  %ret295 = load i8*, i8** %ret, align 8
  %c_call296 = call i8* @LLVMDoubleType()
  %c_call297 = call i8* @LLVMBuildUIToFP(i8* %b294, i8* %ret295, i8* %c_call296, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @52, i32 0, i32 0))
  ret i8* %c_call297

endif280:                                         ; preds = %endif240
  %nombre298 = load i8*, i8** %nombre, align 8
  %strcmp299 = call i32 @strcmp(i8* %nombre298, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @53, i32 0, i32 0))
  %scmp300 = icmp eq i32 %strcmp299, 0
  %nombre301 = load i8*, i8** %nombre, align 8
  %strcmp302 = call i32 @strcmp(i8* %nombre301, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @54, i32 0, i32 0))
  %scmp303 = icmp eq i32 %strcmp302, 0
  %or304 = or i1 %scmp300, %scmp303
  %nombre305 = load i8*, i8** %nombre, align 8
  %strcmp306 = call i32 @strcmp(i8* %nombre305, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @55, i32 0, i32 0))
  %scmp307 = icmp eq i32 %strcmp306, 0
  %or308 = or i1 %or304, %scmp307
  br i1 %or308, label %then309, label %endif310

then309:                                          ; preds = %endif280
  %estado311 = load i8*, i8** %estado1, align 8
  %hijos312 = load i8*, i8** %hijos, align 8
  %elem313 = call i8* @__zen_list_get(i8* %hijos312, i64 0)
  %call314 = call i8* @codegen_expresion(i8* %estado311, i8* %elem313)
  store i8* %call314, i8** %arg, align 8
  %estado315 = load i8*, i8** %estado1, align 8
  %call316 = call i8* @estado_modulo(i8* %estado315)
  %c_call317 = call i8* @LLVMGetNamedFunction(i8* %call316, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @56, i32 0, i32 0))
  store i8* %c_call317, i8** %strtod_fn, align 8
  %call318 = call i8* @i8ptr_type()
  %c_call319 = call i8* @LLVMPointerType(i8* %call318, i32 0)
  %c_call320 = call i8* @LLVMConstPointerNull(i8* %c_call319)
  store i8* %c_call320, i8** %nullp, align 8
  %list321 = call i8* @__zen_list_create(i64 4)
  %arg322 = load i8*, i8** %arg, align 8
  call void @__zen_list_push(i8* %list321, i8* %arg322)
  %nullp323 = load i8*, i8** %nullp, align 8
  call void @__zen_list_push(i8* %list321, i8* %nullp323)
  store i8* %list321, i8** %args, align 8
  %b324 = load i8*, i8** %b, align 8
  %strtod_fn325 = load i8*, i8** %strtod_fn, align 8
  %args326 = load i8*, i8** %args, align 8
  %c_call327 = call i8* @LLVMBuildCall(i8* %b324, i8* %strtod_fn325, i8* %args326, i32 2, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @57, i32 0, i32 0))
  ret i8* %c_call327

endif310:                                         ; preds = %endif280
  %numstr328 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr328
}

define i8* @codegen_statement(i8* %estado, i8* %nodo) {
entry:
  %end_bb = alloca i8*, align 8
  %body_bb = alloca i8*, align 8
  %cond_bb = alloca i8*, align 8
  %stmt = alloca i8*, align 8
  %merge_bb = alloca i8*, align 8
  %else_bb = alloca i8*, align 8
  %then_bb = alloca i8*, align 8
  %main_fn = alloca i8*, align 8
  %cond = alloca i8*, align 8
  %body_idx = alloca double, align 8
  %entry_bb = alloca i8*, align 8
  %func = alloca i8*, align 8
  %func_type = alloca i8*, align 8
  %double_ty = alloca i8*, align 8
  %idx = alloca double, align 8
  %num_params = alloca double, align 8
  %num_hijos = alloca double, align 8
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
  %nodo54 = load i8*, i8** %nodo2, align 8
  %call55 = call i8* @nodo_nombre(i8* %nodo54)
  store i8* %call55, i8** %nombre, align 8
  %nodo56 = load i8*, i8** %nodo2, align 8
  %member57 = call i8* @__zen_struct_get(i8* %nodo56, i64 1)
  store i8* %member57, i8** %hijos, align 8
  %hijos58 = load i8*, i8** %hijos, align 8
  %len = call i64 @strlen(i8* %hijos58)
  %lend = uitofp i64 %len to double
  store double %lend, double* %num_hijos, align 8
  store double 0.000000e+00, double* %num_params, align 8
  store double 0.000000e+00, double* %idx, align 8
  br label %while.cond

endif53:                                          ; preds = %endif38
  %tipo106 = load i8*, i8** %tipo, align 8
  %s2d107 = call double @strtod(i8* %tipo106, i8** null)
  %cmp108 = fcmp oeq double %s2d107, 2.200000e+01
  br i1 %cmp108, label %then109, label %endif110

while.cond:                                       ; preds = %endif70, %then52
  %idx59 = load double, double* %idx, align 8
  %num_hijos60 = load double, double* %num_hijos, align 8
  %cmp61 = fcmp olt double %idx59, %num_hijos60
  br i1 %cmp61, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %hijos62 = load i8*, i8** %hijos, align 8
  %idx63 = load double, double* %idx, align 8
  %idx64 = fptosi double %idx63 to i64
  %elem65 = call i8* @__zen_list_get(i8* %hijos62, i64 %idx64)
  %call66 = call i8* @nodo_tipo(i8* %elem65)
  %s2d67 = call double @strtod(i8* %call66, i8** null)
  %cmp68 = fcmp oeq double %s2d67, 5.000000e+00
  br i1 %cmp68, label %then69, label %else

while.end:                                        ; preds = %else, %while.cond
  %c_call74 = call i8* @LLVMDoubleType()
  store i8* %c_call74, i8** %double_ty, align 8
  %double_ty75 = load i8*, i8** %double_ty, align 8
  %numstr76 = call i8* @__zen_num_to_str(double 0.000000e+00)
  %c_call77 = call i8* @LLVMFunctionType(i8* %double_ty75, i8* %numstr76, i32 0)
  store i8* %c_call77, i8** %func_type, align 8
  %mod78 = load i8*, i8** %mod, align 8
  %nombre79 = load i8*, i8** %nombre, align 8
  %func_type80 = load i8*, i8** %func_type, align 8
  %c_call81 = call i8* @LLVMAddFunction(i8* %mod78, i8* %nombre79, i8* %func_type80)
  store i8* %c_call81, i8** %func, align 8
  %func82 = load i8*, i8** %func, align 8
  %c_call83 = call i8* @LLVMAppendBasicBlock(i8* %func82, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @58, i32 0, i32 0))
  store i8* %c_call83, i8** %entry_bb, align 8
  %b84 = load i8*, i8** %b, align 8
  %entry_bb85 = load i8*, i8** %entry_bb, align 8
  call void @LLVMPositionBuilderAtEnd(i8* %b84, i8* %entry_bb85)
  %num_params86 = load double, double* %num_params, align 8
  store double %num_params86, double* %body_idx, align 8
  br label %while.cond87

then69:                                           ; preds = %while.body
  %num_params71 = load double, double* %num_params, align 8
  %add = fadd double %num_params71, 1.000000e+00
  store double %add, double* %num_params, align 8
  %idx72 = load double, double* %idx, align 8
  %add73 = fadd double %idx72, 1.000000e+00
  store double %add73, double* %idx, align 8
  br label %endif70

else:                                             ; preds = %while.body
  br label %while.end

endif70:                                          ; preds = %then69
  br label %while.cond

while.cond87:                                     ; preds = %while.body88, %while.end
  %body_idx90 = load double, double* %body_idx, align 8
  %num_hijos91 = load double, double* %num_hijos, align 8
  %cmp92 = fcmp olt double %body_idx90, %num_hijos91
  br i1 %cmp92, label %while.body88, label %while.end89

while.body88:                                     ; preds = %while.cond87
  %estado93 = load i8*, i8** %estado1, align 8
  %hijos94 = load i8*, i8** %hijos, align 8
  %body_idx95 = load double, double* %body_idx, align 8
  %idx96 = fptosi double %body_idx95 to i64
  %elem97 = call i8* @__zen_list_get(i8* %hijos94, i64 %idx96)
  %call98 = call i8* @codegen_statement(i8* %estado93, i8* %elem97)
  %body_idx99 = load double, double* %body_idx, align 8
  %add100 = fadd double %body_idx99, 1.000000e+00
  store double %add100, double* %body_idx, align 8
  br label %while.cond87

while.end89:                                      ; preds = %while.cond87
  %b101 = load i8*, i8** %b, align 8
  %mod102 = load i8*, i8** %mod, align 8
  %c_call103 = call i8* @LLVMGetNamedFunction(i8* %mod102, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @59, i32 0, i32 0))
  %c_call104 = call i8* @LLVMAppendBasicBlock(i8* %c_call103, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @60, i32 0, i32 0))
  call void @LLVMPositionBuilderAtEnd(i8* %b101, i8* %c_call104)
  %numstr105 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr105

then109:                                          ; preds = %endif53
  %nodo111 = load i8*, i8** %nodo2, align 8
  %member112 = call i8* @__zen_struct_get(i8* %nodo111, i64 1)
  store i8* %member112, i8** %hijos, align 8
  %estado113 = load i8*, i8** %estado1, align 8
  %hijos114 = load i8*, i8** %hijos, align 8
  %elem115 = call i8* @__zen_list_get(i8* %hijos114, i64 0)
  %call116 = call i8* @codegen_expresion(i8* %estado113, i8* %elem115)
  %numstr117 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr117

endif110:                                         ; preds = %endif53
  %tipo118 = load i8*, i8** %tipo, align 8
  %s2d119 = call double @strtod(i8* %tipo118, i8** null)
  %cmp120 = fcmp oeq double %s2d119, 1.300000e+01
  br i1 %cmp120, label %then121, label %endif122

then121:                                          ; preds = %endif110
  %nodo123 = load i8*, i8** %nodo2, align 8
  %member124 = call i8* @__zen_struct_get(i8* %nodo123, i64 1)
  store i8* %member124, i8** %hijos, align 8
  %estado125 = load i8*, i8** %estado1, align 8
  %hijos126 = load i8*, i8** %hijos, align 8
  %elem127 = call i8* @__zen_list_get(i8* %hijos126, i64 0)
  %call128 = call i8* @codegen_expresion(i8* %estado125, i8* %elem127)
  store i8* %call128, i8** %cond, align 8
  %mod129 = load i8*, i8** %mod, align 8
  %c_call130 = call i8* @LLVMGetNamedFunction(i8* %mod129, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @61, i32 0, i32 0))
  store i8* %c_call130, i8** %main_fn, align 8
  %main_fn131 = load i8*, i8** %main_fn, align 8
  %c_call132 = call i8* @LLVMAppendBasicBlock(i8* %main_fn131, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @62, i32 0, i32 0))
  store i8* %c_call132, i8** %then_bb, align 8
  %main_fn133 = load i8*, i8** %main_fn, align 8
  %c_call134 = call i8* @LLVMAppendBasicBlock(i8* %main_fn133, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @63, i32 0, i32 0))
  store i8* %c_call134, i8** %else_bb, align 8
  %main_fn135 = load i8*, i8** %main_fn, align 8
  %c_call136 = call i8* @LLVMAppendBasicBlock(i8* %main_fn135, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @64, i32 0, i32 0))
  store i8* %c_call136, i8** %merge_bb, align 8
  %b137 = load i8*, i8** %b, align 8
  %cond138 = load i8*, i8** %cond, align 8
  %then_bb139 = load i8*, i8** %then_bb, align 8
  %else_bb140 = load i8*, i8** %else_bb, align 8
  %c_call141 = call i8* @LLVMBuildCondBr(i8* %b137, i8* %cond138, i8* %then_bb139, i8* %else_bb140)
  %b142 = load i8*, i8** %b, align 8
  %then_bb143 = load i8*, i8** %then_bb, align 8
  call void @LLVMPositionBuilderAtEnd(i8* %b142, i8* %then_bb143)
  store double 1.000000e+00, double* %idx, align 8
  %hijos144 = load i8*, i8** %hijos, align 8
  %len145 = call i64 @strlen(i8* %hijos144)
  %lend146 = uitofp i64 %len145 to double
  store double %lend146, double* %num_hijos, align 8
  br label %while.cond147

endif122:                                         ; preds = %endif110
  %tipo193 = load i8*, i8** %tipo, align 8
  %s2d194 = call double @strtod(i8* %tipo193, i8** null)
  %cmp195 = fcmp oeq double %s2d194, 1.600000e+01
  br i1 %cmp195, label %then196, label %endif197

while.cond147:                                    ; preds = %endif162, %then121
  %idx150 = load double, double* %idx, align 8
  %num_hijos151 = load double, double* %num_hijos, align 8
  %cmp152 = fcmp olt double %idx150, %num_hijos151
  br i1 %cmp152, label %while.body148, label %while.end149

while.body148:                                    ; preds = %while.cond147
  %hijos153 = load i8*, i8** %hijos, align 8
  %idx154 = load double, double* %idx, align 8
  %idx155 = fptosi double %idx154 to i64
  %elem156 = call i8* @__zen_list_get(i8* %hijos153, i64 %idx155)
  store i8* %elem156, i8** %stmt, align 8
  %stmt157 = load i8*, i8** %stmt, align 8
  %call158 = call i8* @nodo_tipo(i8* %stmt157)
  %s2d159 = call double @strtod(i8* %call158, i8** null)
  %cmp160 = fcmp oeq double %s2d159, 1.300000e+01
  br i1 %cmp160, label %then161, label %endif162

while.end149:                                     ; preds = %then161, %while.cond147
  %b168 = load i8*, i8** %b, align 8
  %merge_bb169 = load i8*, i8** %merge_bb, align 8
  %c_call170 = call i8* @LLVMBuildBr(i8* %b168, i8* %merge_bb169)
  %b171 = load i8*, i8** %b, align 8
  %else_bb172 = load i8*, i8** %else_bb, align 8
  call void @LLVMPositionBuilderAtEnd(i8* %b171, i8* %else_bb172)
  br label %while.cond173

then161:                                          ; preds = %while.body148
  br label %while.end149

endif162:                                         ; preds = %while.body148
  %estado163 = load i8*, i8** %estado1, align 8
  %stmt164 = load i8*, i8** %stmt, align 8
  %call165 = call i8* @codegen_statement(i8* %estado163, i8* %stmt164)
  %idx166 = load double, double* %idx, align 8
  %add167 = fadd double %idx166, 1.000000e+00
  store double %add167, double* %idx, align 8
  br label %while.cond147

while.cond173:                                    ; preds = %while.body174, %while.end149
  %idx176 = load double, double* %idx, align 8
  %num_hijos177 = load double, double* %num_hijos, align 8
  %cmp178 = fcmp olt double %idx176, %num_hijos177
  br i1 %cmp178, label %while.body174, label %while.end175

while.body174:                                    ; preds = %while.cond173
  %estado179 = load i8*, i8** %estado1, align 8
  %hijos180 = load i8*, i8** %hijos, align 8
  %idx181 = load double, double* %idx, align 8
  %idx182 = fptosi double %idx181 to i64
  %elem183 = call i8* @__zen_list_get(i8* %hijos180, i64 %idx182)
  %call184 = call i8* @codegen_statement(i8* %estado179, i8* %elem183)
  %idx185 = load double, double* %idx, align 8
  %add186 = fadd double %idx185, 1.000000e+00
  store double %add186, double* %idx, align 8
  br label %while.cond173

while.end175:                                     ; preds = %while.cond173
  %b187 = load i8*, i8** %b, align 8
  %merge_bb188 = load i8*, i8** %merge_bb, align 8
  %c_call189 = call i8* @LLVMBuildBr(i8* %b187, i8* %merge_bb188)
  %b190 = load i8*, i8** %b, align 8
  %merge_bb191 = load i8*, i8** %merge_bb, align 8
  call void @LLVMPositionBuilderAtEnd(i8* %b190, i8* %merge_bb191)
  %numstr192 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr192

then196:                                          ; preds = %endif122
  %nodo198 = load i8*, i8** %nodo2, align 8
  %member199 = call i8* @__zen_struct_get(i8* %nodo198, i64 1)
  store i8* %member199, i8** %hijos, align 8
  %mod200 = load i8*, i8** %mod, align 8
  %c_call201 = call i8* @LLVMGetNamedFunction(i8* %mod200, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @65, i32 0, i32 0))
  store i8* %c_call201, i8** %main_fn, align 8
  %main_fn202 = load i8*, i8** %main_fn, align 8
  %c_call203 = call i8* @LLVMAppendBasicBlock(i8* %main_fn202, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @66, i32 0, i32 0))
  store i8* %c_call203, i8** %cond_bb, align 8
  %main_fn204 = load i8*, i8** %main_fn, align 8
  %c_call205 = call i8* @LLVMAppendBasicBlock(i8* %main_fn204, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @67, i32 0, i32 0))
  store i8* %c_call205, i8** %body_bb, align 8
  %main_fn206 = load i8*, i8** %main_fn, align 8
  %c_call207 = call i8* @LLVMAppendBasicBlock(i8* %main_fn206, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @68, i32 0, i32 0))
  store i8* %c_call207, i8** %end_bb, align 8
  %b208 = load i8*, i8** %b, align 8
  %cond_bb209 = load i8*, i8** %cond_bb, align 8
  %c_call210 = call i8* @LLVMBuildBr(i8* %b208, i8* %cond_bb209)
  %b211 = load i8*, i8** %b, align 8
  %cond_bb212 = load i8*, i8** %cond_bb, align 8
  call void @LLVMPositionBuilderAtEnd(i8* %b211, i8* %cond_bb212)
  %estado213 = load i8*, i8** %estado1, align 8
  %hijos214 = load i8*, i8** %hijos, align 8
  %elem215 = call i8* @__zen_list_get(i8* %hijos214, i64 0)
  %call216 = call i8* @codegen_expresion(i8* %estado213, i8* %elem215)
  store i8* %call216, i8** %cond, align 8
  %b217 = load i8*, i8** %b, align 8
  %cond218 = load i8*, i8** %cond, align 8
  %body_bb219 = load i8*, i8** %body_bb, align 8
  %end_bb220 = load i8*, i8** %end_bb, align 8
  %c_call221 = call i8* @LLVMBuildCondBr(i8* %b217, i8* %cond218, i8* %body_bb219, i8* %end_bb220)
  %b222 = load i8*, i8** %b, align 8
  %body_bb223 = load i8*, i8** %body_bb, align 8
  call void @LLVMPositionBuilderAtEnd(i8* %b222, i8* %body_bb223)
  store double 1.000000e+00, double* %idx, align 8
  %hijos224 = load i8*, i8** %hijos, align 8
  %len225 = call i64 @strlen(i8* %hijos224)
  %lend226 = uitofp i64 %len225 to double
  store double %lend226, double* %num_hijos, align 8
  br label %while.cond227

endif197:                                         ; preds = %endif122
  %tipo247 = load i8*, i8** %tipo, align 8
  %s2d248 = call double @strtod(i8* %tipo247, i8** null)
  %cmp249 = fcmp oeq double %s2d248, 1.400000e+01
  br i1 %cmp249, label %then250, label %endif251

while.cond227:                                    ; preds = %while.body228, %then196
  %idx230 = load double, double* %idx, align 8
  %num_hijos231 = load double, double* %num_hijos, align 8
  %cmp232 = fcmp olt double %idx230, %num_hijos231
  br i1 %cmp232, label %while.body228, label %while.end229

while.body228:                                    ; preds = %while.cond227
  %estado233 = load i8*, i8** %estado1, align 8
  %hijos234 = load i8*, i8** %hijos, align 8
  %idx235 = load double, double* %idx, align 8
  %idx236 = fptosi double %idx235 to i64
  %elem237 = call i8* @__zen_list_get(i8* %hijos234, i64 %idx236)
  %call238 = call i8* @codegen_statement(i8* %estado233, i8* %elem237)
  %idx239 = load double, double* %idx, align 8
  %add240 = fadd double %idx239, 1.000000e+00
  store double %add240, double* %idx, align 8
  br label %while.cond227

while.end229:                                     ; preds = %while.cond227
  %b241 = load i8*, i8** %b, align 8
  %cond_bb242 = load i8*, i8** %cond_bb, align 8
  %c_call243 = call i8* @LLVMBuildBr(i8* %b241, i8* %cond_bb242)
  %b244 = load i8*, i8** %b, align 8
  %end_bb245 = load i8*, i8** %end_bb, align 8
  call void @LLVMPositionBuilderAtEnd(i8* %b244, i8* %end_bb245)
  %numstr246 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr246

then250:                                          ; preds = %endif197
  %numstr252 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr252

endif251:                                         ; preds = %endif197
  %tipo253 = load i8*, i8** %tipo, align 8
  %s2d254 = call double @strtod(i8* %tipo253, i8** null)
  %cmp255 = fcmp oeq double %s2d254, 1.800000e+01
  br i1 %cmp255, label %then256, label %endif257

then256:                                          ; preds = %endif251
  %numstr258 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr258

endif257:                                         ; preds = %endif251
  %tipo259 = load i8*, i8** %tipo, align 8
  %s2d260 = call double @strtod(i8* %tipo259, i8** null)
  %cmp261 = fcmp oeq double %s2d260, 1.900000e+01
  br i1 %cmp261, label %then262, label %endif263

then262:                                          ; preds = %endif257
  %numstr264 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr264

endif263:                                         ; preds = %endif257
  %numstr265 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr265
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
  %c_call9 = call i8* @LLVMAddFunction(i8* %call7, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @69, i32 0, i32 0), i8* %main_type8)
  store i8* %c_call9, i8** %main_func, align 8
  %main_func10 = load i8*, i8** %main_func, align 8
  %c_call11 = call i8* @LLVMAppendBasicBlock(i8* %main_func10, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @70, i32 0, i32 0))
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

define i32 @main(i32 %argc, i8** %argv) {
entry:
  store i32 %argc, i32* @__zen_argc, align 4
  store i8** %argv, i8*** @__zen_argv, align 8
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @72, i32 0, i32 0), i8* getelementptr inbounds ([38 x i8], [38 x i8]* @71, i32 0, i32 0))
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @74, i32 0, i32 0), i8* getelementptr inbounds ([65 x i8], [65 x i8]* @73, i32 0, i32 0))
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @76, i32 0, i32 0), i8* getelementptr inbounds ([64 x i8], [64 x i8]* @75, i32 0, i32 0))
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @78, i32 0, i32 0), i8* getelementptr inbounds ([72 x i8], [72 x i8]* @77, i32 0, i32 0))
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @80, i32 0, i32 0), i8* getelementptr inbounds ([26 x i8], [26 x i8]* @79, i32 0, i32 0))
  ret i32 0
}
