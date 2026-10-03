; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@g_czl_archivos = internal global i8* null
@g_czl_codigo = internal global i8* null
@g_czl_comentarios = internal global i8* null
@g_czl_vacias = internal global i8* null
@g_czl_total = internal global i8* null
@3 = private unnamed_addr constant [3 x i8] c"rb\00", align 1
@4 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@5 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@7 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@9 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@10 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@11 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@12 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@13 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@14 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@15 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@16 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@17 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@18 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@19 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@20 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@21 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@22 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@23 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@24 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@25 = private unnamed_addr constant [33 x i8] c"=== CZL - Counting Zen Lines ===\00", align 1
@26 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@27 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@28 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@29 = private unnamed_addr constant [12 x i8] c"src/ast.zen\00", align 1
@30 = private unnamed_addr constant [17 x i8] c"src/builtins.zen\00", align 1
@31 = private unnamed_addr constant [16 x i8] c"src/codegen.zen\00", align 1
@32 = private unnamed_addr constant [14 x i8] c"src/lexer.zen\00", align 1
@33 = private unnamed_addr constant [13 x i8] c"src/main.zen\00", align 1
@34 = private unnamed_addr constant [15 x i8] c"src/parser.zen\00", align 1
@35 = private unnamed_addr constant [16 x i8] c"Archivos:      \00", align 1
@36 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@37 = private unnamed_addr constant [16 x i8] c"Codigo:        \00", align 1
@38 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@39 = private unnamed_addr constant [16 x i8] c"Comentarios:   \00", align 1
@40 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@41 = private unnamed_addr constant [16 x i8] c"Vacias:        \00", align 1
@42 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@43 = private unnamed_addr constant [16 x i8] c"TOTAL:         \00", align 1
@44 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@45 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@46 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@47 = private unnamed_addr constant [15 x i8] c"=== CZL OK ===\00", align 1
@48 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

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

define i8* @contar_archivo(i8* %ruta) {
entry:
  %c = alloca i8*, align 8
  %linicio = alloca i8*, align 8
  %es_vacia = alloca i8*, align 8
  %en_com = alloca i8*, align 8
  %pos = alloca i8*, align 8
  %len7 = alloca i8*, align 8
  %codigo = alloca i8*, align 8
  %ruta1 = alloca i8*, align 8
  store i8* %ruta, i8** %ruta1, align 8
  %ruta2 = load i8*, i8** %ruta1, align 8
  %file = call i8* @fopen(i8* %ruta2, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @3, i32 0, i32 0))
  %isnull = icmp eq i8* %file, null
  br i1 %isnull, label %rf.end, label %rf.open

rf.open:                                          ; preds = %entry
  %0 = call i32 @fseek(i8* %file, i64 0, i32 2)
  %size = call i64 @ftell(i8* %file)
  %1 = call i32 @fseek(i8* %file, i64 0, i32 0)
  %sp1 = add i64 %size, 1
  %buf = call i8* @malloc(i64 %sp1)
  %2 = call i64 @fread(i8* %buf, i64 1, i64 %size, i8* %file)
  %ep = getelementptr inbounds i8, i8* %buf, i64 %size
  store i8 0, i8* %ep, align 1
  %3 = call i32 @fclose(i8* %file)
  br label %rf.end

rf.end:                                           ; preds = %rf.open, %entry
  %rf_result = phi i8* [ getelementptr inbounds ([1 x i8], [1 x i8]* @4, i32 0, i32 0), %entry ], [ %buf, %rf.open ]
  store i8* %rf_result, i8** %codigo, align 8
  %codigo3 = load i8*, i8** %codigo, align 8
  %len = call i64 @strlen(i8* %codigo3)
  %lend = uitofp i64 %len to double
  %cmp = fcmp oeq double %lend, 0.000000e+00
  br i1 %cmp, label %then, label %endif

then:                                             ; preds = %rf.end
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr

endif:                                            ; preds = %rf.end
  %codigo4 = load i8*, i8** %codigo, align 8
  %len5 = call i64 @strlen(i8* %codigo4)
  %lend6 = uitofp i64 %len5 to double
  %numstr8 = call i8* @__zen_num_to_str(double %lend6)
  store i8* %numstr8, i8** %len7, align 8
  %numstr9 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr9, i8** %pos, align 8
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @6, i32 0, i32 0), i8** %en_com, align 8
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @7, i32 0, i32 0), i8** %es_vacia, align 8
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @9, i32 0, i32 0), i8** %linicio, align 8
  br label %while.cond

while.cond:                                       ; preds = %endif38, %endif
  %pos10 = load i8*, i8** %pos, align 8
  %len11 = load i8*, i8** %len7, align 8
  %s2d = call double @strtod(i8* %pos10, i8** null)
  %s2d12 = call double @strtod(i8* %len11, i8** null)
  %cmp13 = fcmp olt double %s2d, %s2d12
  br i1 %cmp13, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %codigo14 = load i8*, i8** %codigo, align 8
  %pos15 = load i8*, i8** %pos, align 8
  %s2d16 = call double @strtod(i8* %pos15, i8** null)
  %idx = fptosi double %s2d16 to i64
  %charptr = getelementptr i8, i8* %codigo14, i64 %idx
  %ch = load i8, i8* %charptr, align 1
  %chd = sitofp i8 %ch to double
  %numstr17 = call i8* @__zen_num_to_str(double %chd)
  store i8* %numstr17, i8** %c, align 8
  %linicio18 = load i8*, i8** %linicio, align 8
  %s2d19 = call double @strtod(i8* %linicio18, i8** null)
  %cmp20 = fcmp oeq double %s2d19, 1.000000e+00
  br i1 %cmp20, label %then21, label %endif22

while.end:                                        ; preds = %while.cond
  %czl_total66 = load i8*, i8** @g_czl_total, align 8
  %s2d67 = call double @strtod(i8* %czl_total66, i8** null)
  %add68 = fadd double %s2d67, 1.000000e+00
  %numstr69 = call i8* @__zen_num_to_str(double %add68)
  store i8* %numstr69, i8** @g_czl_total, align 8
  %en_com70 = load i8*, i8** %en_com, align 8
  %s2d71 = call double @strtod(i8* %en_com70, i8** null)
  %cmp72 = fcmp oeq double %s2d71, 1.000000e+00
  br i1 %cmp72, label %then73, label %else74

then21:                                           ; preds = %while.body
  %c23 = load i8*, i8** %c, align 8
  %s2d24 = call double @strtod(i8* %c23, i8** null)
  %cmp25 = fcmp oeq double %s2d24, 3.500000e+01
  br i1 %cmp25, label %then26, label %else

endif22:                                          ; preds = %endif33, %while.body
  %c34 = load i8*, i8** %c, align 8
  %s2d35 = call double @strtod(i8* %c34, i8** null)
  %cmp36 = fcmp oeq double %s2d35, 1.000000e+01
  br i1 %cmp36, label %then37, label %endif38

then26:                                           ; preds = %then21
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @11, i32 0, i32 0), i8** %en_com, align 8
  br label %endif27

else:                                             ; preds = %then21
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @14, i32 0, i32 0), i8** %en_com, align 8
  br label %endif27

endif27:                                          ; preds = %else, %then26
  %c28 = load i8*, i8** %c, align 8
  %s2d29 = call double @strtod(i8* %c28, i8** null)
  %cmp30 = fcmp oeq double %s2d29, 1.000000e+01
  br i1 %cmp30, label %then31, label %else32

then31:                                           ; preds = %endif27
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @15, i32 0, i32 0), i8** %es_vacia, align 8
  br label %endif33

else32:                                           ; preds = %endif27
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @18, i32 0, i32 0), i8** %es_vacia, align 8
  br label %endif33

endif33:                                          ; preds = %else32, %then31
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @20, i32 0, i32 0), i8** %linicio, align 8
  br label %endif22

then37:                                           ; preds = %endif22
  %czl_total = load i8*, i8** @g_czl_total, align 8
  %s2d39 = call double @strtod(i8* %czl_total, i8** null)
  %add = fadd double %s2d39, 1.000000e+00
  %numstr40 = call i8* @__zen_num_to_str(double %add)
  store i8* %numstr40, i8** @g_czl_total, align 8
  %en_com41 = load i8*, i8** %en_com, align 8
  %s2d42 = call double @strtod(i8* %en_com41, i8** null)
  %cmp43 = fcmp oeq double %s2d42, 1.000000e+00
  br i1 %cmp43, label %then44, label %else45

endif38:                                          ; preds = %endif46, %endif22
  %pos62 = load i8*, i8** %pos, align 8
  %s2d63 = call double @strtod(i8* %pos62, i8** null)
  %add64 = fadd double %s2d63, 1.000000e+00
  %numstr65 = call i8* @__zen_num_to_str(double %add64)
  store i8* %numstr65, i8** %pos, align 8
  br label %while.cond

then44:                                           ; preds = %then37
  %czl_comentarios = load i8*, i8** @g_czl_comentarios, align 8
  %s2d47 = call double @strtod(i8* %czl_comentarios, i8** null)
  %add48 = fadd double %s2d47, 1.000000e+00
  %numstr49 = call i8* @__zen_num_to_str(double %add48)
  store i8* %numstr49, i8** @g_czl_comentarios, align 8
  br label %endif46

else45:                                           ; preds = %then37
  %es_vacia50 = load i8*, i8** %es_vacia, align 8
  %s2d51 = call double @strtod(i8* %es_vacia50, i8** null)
  %cmp52 = fcmp oeq double %s2d51, 1.000000e+00
  br i1 %cmp52, label %then53, label %else54

endif46:                                          ; preds = %endif55, %then44
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @21, i32 0, i32 0), i8** %linicio, align 8
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @23, i32 0, i32 0), i8** %es_vacia, align 8
  br label %endif38

then53:                                           ; preds = %else45
  %czl_vacias = load i8*, i8** @g_czl_vacias, align 8
  %s2d56 = call double @strtod(i8* %czl_vacias, i8** null)
  %add57 = fadd double %s2d56, 1.000000e+00
  %numstr58 = call i8* @__zen_num_to_str(double %add57)
  store i8* %numstr58, i8** @g_czl_vacias, align 8
  br label %endif55

else54:                                           ; preds = %else45
  %czl_codigo = load i8*, i8** @g_czl_codigo, align 8
  %s2d59 = call double @strtod(i8* %czl_codigo, i8** null)
  %add60 = fadd double %s2d59, 1.000000e+00
  %numstr61 = call i8* @__zen_num_to_str(double %add60)
  store i8* %numstr61, i8** @g_czl_codigo, align 8
  br label %endif55

endif55:                                          ; preds = %else54, %then53
  br label %endif46

then73:                                           ; preds = %while.end
  %czl_comentarios76 = load i8*, i8** @g_czl_comentarios, align 8
  %s2d77 = call double @strtod(i8* %czl_comentarios76, i8** null)
  %add78 = fadd double %s2d77, 1.000000e+00
  %numstr79 = call i8* @__zen_num_to_str(double %add78)
  store i8* %numstr79, i8** @g_czl_comentarios, align 8
  br label %endif75

else74:                                           ; preds = %while.end
  %es_vacia80 = load i8*, i8** %es_vacia, align 8
  %s2d81 = call double @strtod(i8* %es_vacia80, i8** null)
  %cmp82 = fcmp oeq double %s2d81, 1.000000e+00
  br i1 %cmp82, label %then83, label %else84

endif75:                                          ; preds = %endif85, %then73
  %czl_archivos = load i8*, i8** @g_czl_archivos, align 8
  %s2d94 = call double @strtod(i8* %czl_archivos, i8** null)
  %add95 = fadd double %s2d94, 1.000000e+00
  %numstr96 = call i8* @__zen_num_to_str(double %add95)
  store i8* %numstr96, i8** @g_czl_archivos, align 8
  ret i8* null

then83:                                           ; preds = %else74
  %czl_vacias86 = load i8*, i8** @g_czl_vacias, align 8
  %s2d87 = call double @strtod(i8* %czl_vacias86, i8** null)
  %add88 = fadd double %s2d87, 1.000000e+00
  %numstr89 = call i8* @__zen_num_to_str(double %add88)
  store i8* %numstr89, i8** @g_czl_vacias, align 8
  br label %endif85

else84:                                           ; preds = %else74
  %czl_codigo90 = load i8*, i8** @g_czl_codigo, align 8
  %s2d91 = call double @strtod(i8* %czl_codigo90, i8** null)
  %add92 = fadd double %s2d91, 1.000000e+00
  %numstr93 = call i8* @__zen_num_to_str(double %add92)
  store i8* %numstr93, i8** @g_czl_codigo, align 8
  br label %endif85

endif85:                                          ; preds = %else84, %then83
  br label %endif75
}

define i32 @main() {
entry:
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr, i8** @g_czl_archivos, align 8
  %numstr1 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr1, i8** @g_czl_codigo, align 8
  %numstr2 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr2, i8** @g_czl_comentarios, align 8
  %numstr3 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr3, i8** @g_czl_vacias, align 8
  %numstr4 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr4, i8** @g_czl_total, align 8
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @26, i32 0, i32 0), i8* getelementptr inbounds ([33 x i8], [33 x i8]* @25, i32 0, i32 0))
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @28, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @27, i32 0, i32 0))
  %call = call i8* @contar_archivo(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @29, i32 0, i32 0))
  %call5 = call i8* @contar_archivo(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @30, i32 0, i32 0))
  %call6 = call i8* @contar_archivo(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @31, i32 0, i32 0))
  %call7 = call i8* @contar_archivo(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @32, i32 0, i32 0))
  %call8 = call i8* @contar_archivo(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @33, i32 0, i32 0))
  %call9 = call i8* @contar_archivo(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @34, i32 0, i32 0))
  %czl_archivos = load i8*, i8** @g_czl_archivos, align 8
  %concat = call i8* @zen_concat(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @35, i32 0, i32 0), i8* %czl_archivos)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @36, i32 0, i32 0), i8* %concat)
  %czl_codigo = load i8*, i8** @g_czl_codigo, align 8
  %concat10 = call i8* @zen_concat(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @37, i32 0, i32 0), i8* %czl_codigo)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @38, i32 0, i32 0), i8* %concat10)
  %czl_comentarios = load i8*, i8** @g_czl_comentarios, align 8
  %concat11 = call i8* @zen_concat(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @39, i32 0, i32 0), i8* %czl_comentarios)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @40, i32 0, i32 0), i8* %concat11)
  %czl_vacias = load i8*, i8** @g_czl_vacias, align 8
  %concat12 = call i8* @zen_concat(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @41, i32 0, i32 0), i8* %czl_vacias)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @42, i32 0, i32 0), i8* %concat12)
  %czl_total = load i8*, i8** @g_czl_total, align 8
  %concat13 = call i8* @zen_concat(i8* getelementptr inbounds ([16 x i8], [16 x i8]* @43, i32 0, i32 0), i8* %czl_total)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @44, i32 0, i32 0), i8* %concat13)
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @46, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @45, i32 0, i32 0))
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @48, i32 0, i32 0), i8* getelementptr inbounds ([15 x i8], [15 x i8]* @47, i32 0, i32 0))
  ret i32 0
}
