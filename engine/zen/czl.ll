; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@__zen_argc = internal global i32 0
@__zen_argv = internal global i8** null
@g_ESC = internal global i8* null
@g_czl_archivos = internal global i8* null
@g_czl_codigo = internal global i8* null
@g_czl_comentarios = internal global i8* null
@g_czl_vacias = internal global i8* null
@g_czl_total = internal global i8* null
@g_czl_dir = internal global i8* null
@g_czl_excluir = internal global i8* null
@g_header = internal global i8* null
@g_title = internal global i8* null
@g_sep = internal global i8* null
@g_lang_hdr = internal global i8* null
@g_reset = internal global i8* null
@g_green = internal global i8* null
@g_cyan = internal global i8* null
@g_n = internal global i8* null
@g_idx = internal global i8* null
@3 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@5 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@7 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@9 = private unnamed_addr constant [3 x i8] c"rb\00", align 1
@10 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@11 = private unnamed_addr constant [9 x i8] c"/ast.zen\00", align 1
@12 = private unnamed_addr constant [14 x i8] c"/builtins.zen\00", align 1
@13 = private unnamed_addr constant [13 x i8] c"/codegen.zen\00", align 1
@14 = private unnamed_addr constant [11 x i8] c"/lexer.zen\00", align 1
@15 = private unnamed_addr constant [10 x i8] c"/main.zen\00", align 1
@16 = private unnamed_addr constant [12 x i8] c"/parser.zen\00", align 1
@17 = private unnamed_addr constant [18 x i8] c"/zen_compiler.zen\00", align 1
@18 = private unnamed_addr constant [4 x i8] c"src\00", align 1
@19 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@20 = private unnamed_addr constant [47 x i8] c"[1;36m========================================\00", align 1
@21 = private unnamed_addr constant [4 x i8] c"[0m\00", align 1
@22 = private unnamed_addr constant [32 x i8] c"[1;36m CZL - Counting Zen Lines\00", align 1
@23 = private unnamed_addr constant [4 x i8] c"[0m\00", align 1
@24 = private unnamed_addr constant [47 x i8] c"[0;37m----------------------------------------\00", align 1
@25 = private unnamed_addr constant [4 x i8] c"[0m\00", align 1
@26 = private unnamed_addr constant [68 x i8] c"[1;33m Lang        Files      Lines       Code   Comments    Blanks\00", align 1
@27 = private unnamed_addr constant [4 x i8] c"[0m\00", align 1
@28 = private unnamed_addr constant [4 x i8] c"[0m\00", align 1
@29 = private unnamed_addr constant [7 x i8] c"[1;32m\00", align 1
@30 = private unnamed_addr constant [7 x i8] c"[1;36m\00", align 1
@31 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@32 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@33 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@34 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@35 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@36 = private unnamed_addr constant [3 x i8] c"-e\00", align 1
@37 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@38 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@39 = private unnamed_addr constant [17 x i8] c" Zen            \00", align 1
@40 = private unnamed_addr constant [9 x i8] c"        \00", align 1
@41 = private unnamed_addr constant [8 x i8] c"       \00", align 1
@42 = private unnamed_addr constant [8 x i8] c"       \00", align 1
@43 = private unnamed_addr constant [9 x i8] c"        \00", align 1
@44 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@45 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@46 = private unnamed_addr constant [17 x i8] c" Total          \00", align 1
@47 = private unnamed_addr constant [9 x i8] c"        \00", align 1
@48 = private unnamed_addr constant [8 x i8] c"       \00", align 1
@49 = private unnamed_addr constant [8 x i8] c"       \00", align 1
@50 = private unnamed_addr constant [9 x i8] c"        \00", align 1
@51 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@52 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

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

define i8* @contiene_barra(i8* %s, i8* %sub) {
entry:
  %s1 = alloca i8*, align 8
  store i8* %s, i8** %s1, align 8
  %sub2 = alloca i8*, align 8
  store i8* %sub, i8** %sub2, align 8
  %sub3 = load i8*, i8** %sub2, align 8
  %len = call i64 @strlen(i8* %sub3)
  %lend = uitofp i64 %len to double
  %cmp = fcmp oeq double %lend, 0.000000e+00
  br i1 %cmp, label %then, label %endif

then:                                             ; preds = %entry
  ret i8* getelementptr inbounds ([3 x i8], [3 x i8]* @4, i32 0, i32 0)

endif:                                            ; preds = %entry
  %sub4 = load i8*, i8** %sub2, align 8
  %s5 = load i8*, i8** %s1, align 8
  %strcmp = call i32 @strcmp(i8* %sub4, i8* %s5)
  %scmp = icmp eq i32 %strcmp, 0
  br i1 %scmp, label %then6, label %endif7

then6:                                            ; preds = %endif
  ret i8* getelementptr inbounds ([4 x i8], [4 x i8]* @5, i32 0, i32 0)

endif7:                                           ; preds = %endif
  ret i8* getelementptr inbounds ([3 x i8], [3 x i8]* @8, i32 0, i32 0)
}

define i8* @contar_archivo(i8* %ruta) {
entry:
  %c = alloca double, align 8
  %tiene_cod = alloca i1, align 1
  %es_com = alloca i1, align 1
  %linicio = alloca i1, align 1
  %pos = alloca double, align 8
  %len12 = alloca double, align 8
  %codigo = alloca i8*, align 8
  %ruta1 = alloca i8*, align 8
  store i8* %ruta, i8** %ruta1, align 8
  %ruta2 = load i8*, i8** %ruta1, align 8
  %czl_excluir = load i8*, i8** @g_czl_excluir, align 8
  %call = call i8* @contiene_barra(i8* %ruta2, i8* %czl_excluir)
  %s2d = call double @strtod(i8* %call, i8** null)
  %cmp = fcmp oeq double %s2d, 1.000000e+00
  br i1 %cmp, label %then, label %endif

then:                                             ; preds = %entry
  %numstr = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr

endif:                                            ; preds = %entry
  %ruta3 = load i8*, i8** %ruta1, align 8
  %file = call i8* @fopen(i8* %ruta3, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @9, i32 0, i32 0))
  %isnull = icmp eq i8* %file, null
  br i1 %isnull, label %rf.end, label %rf.open

rf.open:                                          ; preds = %endif
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

rf.end:                                           ; preds = %rf.open, %endif
  %rf_result = phi i8* [ getelementptr inbounds ([1 x i8], [1 x i8]* @10, i32 0, i32 0), %endif ], [ %buf, %rf.open ]
  store i8* %rf_result, i8** %codigo, align 8
  %codigo4 = load i8*, i8** %codigo, align 8
  %len = call i64 @strlen(i8* %codigo4)
  %lend = uitofp i64 %len to double
  %cmp5 = fcmp oeq double %lend, 0.000000e+00
  br i1 %cmp5, label %then6, label %endif7

then6:                                            ; preds = %rf.end
  %numstr8 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr8

endif7:                                           ; preds = %rf.end
  %codigo9 = load i8*, i8** %codigo, align 8
  %len10 = call i64 @strlen(i8* %codigo9)
  %lend11 = uitofp i64 %len10 to double
  store double %lend11, double* %len12, align 8
  store double 0.000000e+00, double* %pos, align 8
  store i1 true, i1* %linicio, align 1
  store i1 false, i1* %es_com, align 1
  store i1 false, i1* %tiene_cod, align 1
  br label %while.cond

while.cond:                                       ; preds = %endif29, %endif7
  %pos13 = load double, double* %pos, align 8
  %len14 = load double, double* %len12, align 8
  %cmp15 = fcmp olt double %pos13, %len14
  br i1 %cmp15, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %codigo16 = load i8*, i8** %codigo, align 8
  %pos17 = load double, double* %pos, align 8
  %idx = fptosi double %pos17 to i64
  %charptr = getelementptr i8, i8* %codigo16, i64 %idx
  %ch = load i8, i8* %charptr, align 1
  %chd = sitofp i8 %ch to double
  store double %chd, double* %c, align 8
  %linicio18 = load i1, i1* %linicio, align 1
  %tof = uitofp i1 %linicio18 to double
  %cmp19 = fcmp oeq double %tof, 1.000000e+00
  br i1 %cmp19, label %then20, label %endif21

while.end:                                        ; preds = %while.cond
  %czl_total68 = load i8*, i8** @g_czl_total, align 8
  %s2d69 = call double @strtod(i8* %czl_total68, i8** null)
  %add70 = fadd double %s2d69, 1.000000e+00
  %numstr71 = call i8* @__zen_num_to_str(double %add70)
  store i8* %numstr71, i8** @g_czl_total, align 8
  %es_com72 = load i1, i1* %es_com, align 1
  %tof73 = uitofp i1 %es_com72 to double
  %cmp74 = fcmp oeq double %tof73, 1.000000e+00
  br i1 %cmp74, label %then75, label %else76

then20:                                           ; preds = %while.body
  store i1 false, i1* %es_com, align 1
  store i1 false, i1* %tiene_cod, align 1
  store i1 false, i1* %linicio, align 1
  br label %endif21

endif21:                                          ; preds = %then20, %while.body
  %c22 = load double, double* %c, align 8
  %cmp23 = fcmp oeq double %c22, 3.500000e+01
  br i1 %cmp23, label %then24, label %endif25

then24:                                           ; preds = %endif21
  store i1 true, i1* %es_com, align 1
  br label %endif25

endif25:                                          ; preds = %then24, %endif21
  %c26 = load double, double* %c, align 8
  %cmp27 = fcmp oeq double %c26, 1.000000e+01
  br i1 %cmp27, label %then28, label %else

then28:                                           ; preds = %endif25
  %czl_total = load i8*, i8** @g_czl_total, align 8
  %s2d30 = call double @strtod(i8* %czl_total, i8** null)
  %add = fadd double %s2d30, 1.000000e+00
  %numstr31 = call i8* @__zen_num_to_str(double %add)
  store i8* %numstr31, i8** @g_czl_total, align 8
  %es_com32 = load i1, i1* %es_com, align 1
  %tof33 = uitofp i1 %es_com32 to double
  %cmp34 = fcmp oeq double %tof33, 1.000000e+00
  br i1 %cmp34, label %then35, label %else36

else:                                             ; preds = %endif25
  %c53 = load double, double* %c, align 8
  %cmp54 = fcmp one double %c53, 3.200000e+01
  br i1 %cmp54, label %then55, label %endif56

endif29:                                          ; preds = %endif56, %endif37
  %pos66 = load double, double* %pos, align 8
  %add67 = fadd double %pos66, 1.000000e+00
  store double %add67, double* %pos, align 8
  br label %while.cond

then35:                                           ; preds = %then28
  %czl_comentarios = load i8*, i8** @g_czl_comentarios, align 8
  %s2d38 = call double @strtod(i8* %czl_comentarios, i8** null)
  %add39 = fadd double %s2d38, 1.000000e+00
  %numstr40 = call i8* @__zen_num_to_str(double %add39)
  store i8* %numstr40, i8** @g_czl_comentarios, align 8
  br label %endif37

else36:                                           ; preds = %then28
  %tiene_cod41 = load i1, i1* %tiene_cod, align 1
  %tof42 = uitofp i1 %tiene_cod41 to double
  %cmp43 = fcmp oeq double %tof42, 0.000000e+00
  br i1 %cmp43, label %then44, label %else45

endif37:                                          ; preds = %endif46, %then35
  store i1 true, i1* %linicio, align 1
  store i1 false, i1* %es_com, align 1
  store i1 false, i1* %tiene_cod, align 1
  br label %endif29

then44:                                           ; preds = %else36
  %czl_vacias = load i8*, i8** @g_czl_vacias, align 8
  %s2d47 = call double @strtod(i8* %czl_vacias, i8** null)
  %add48 = fadd double %s2d47, 1.000000e+00
  %numstr49 = call i8* @__zen_num_to_str(double %add48)
  store i8* %numstr49, i8** @g_czl_vacias, align 8
  br label %endif46

else45:                                           ; preds = %else36
  %czl_codigo = load i8*, i8** @g_czl_codigo, align 8
  %s2d50 = call double @strtod(i8* %czl_codigo, i8** null)
  %add51 = fadd double %s2d50, 1.000000e+00
  %numstr52 = call i8* @__zen_num_to_str(double %add51)
  store i8* %numstr52, i8** @g_czl_codigo, align 8
  br label %endif46

endif46:                                          ; preds = %else45, %then44
  br label %endif37

then55:                                           ; preds = %else
  %c57 = load double, double* %c, align 8
  %cmp58 = fcmp one double %c57, 9.000000e+00
  br i1 %cmp58, label %then59, label %endif60

endif56:                                          ; preds = %endif60, %else
  br label %endif29

then59:                                           ; preds = %then55
  %es_com61 = load i1, i1* %es_com, align 1
  %tof62 = uitofp i1 %es_com61 to double
  %cmp63 = fcmp oeq double %tof62, 0.000000e+00
  br i1 %cmp63, label %then64, label %endif65

endif60:                                          ; preds = %endif65, %then55
  br label %endif56

then64:                                           ; preds = %then59
  store i1 true, i1* %tiene_cod, align 1
  br label %endif65

endif65:                                          ; preds = %then64, %then59
  br label %endif60

then75:                                           ; preds = %while.end
  %czl_comentarios78 = load i8*, i8** @g_czl_comentarios, align 8
  %s2d79 = call double @strtod(i8* %czl_comentarios78, i8** null)
  %add80 = fadd double %s2d79, 1.000000e+00
  %numstr81 = call i8* @__zen_num_to_str(double %add80)
  store i8* %numstr81, i8** @g_czl_comentarios, align 8
  br label %endif77

else76:                                           ; preds = %while.end
  %tiene_cod82 = load i1, i1* %tiene_cod, align 1
  %tof83 = uitofp i1 %tiene_cod82 to double
  %cmp84 = fcmp oeq double %tof83, 0.000000e+00
  br i1 %cmp84, label %then85, label %else86

endif77:                                          ; preds = %endif87, %then75
  %czl_archivos = load i8*, i8** @g_czl_archivos, align 8
  %s2d96 = call double @strtod(i8* %czl_archivos, i8** null)
  %add97 = fadd double %s2d96, 1.000000e+00
  %numstr98 = call i8* @__zen_num_to_str(double %add97)
  store i8* %numstr98, i8** @g_czl_archivos, align 8
  ret i8* null

then85:                                           ; preds = %else76
  %czl_vacias88 = load i8*, i8** @g_czl_vacias, align 8
  %s2d89 = call double @strtod(i8* %czl_vacias88, i8** null)
  %add90 = fadd double %s2d89, 1.000000e+00
  %numstr91 = call i8* @__zen_num_to_str(double %add90)
  store i8* %numstr91, i8** @g_czl_vacias, align 8
  br label %endif87

else86:                                           ; preds = %else76
  %czl_codigo92 = load i8*, i8** @g_czl_codigo, align 8
  %s2d93 = call double @strtod(i8* %czl_codigo92, i8** null)
  %add94 = fadd double %s2d93, 1.000000e+00
  %numstr95 = call i8* @__zen_num_to_str(double %add94)
  store i8* %numstr95, i8** @g_czl_codigo, align 8
  br label %endif87

endif87:                                          ; preds = %else86, %then85
  br label %endif77
}

define i8* @contar_dir(i8* %d) {
entry:
  %d1 = alloca i8*, align 8
  store i8* %d, i8** %d1, align 8
  %d2 = load i8*, i8** %d1, align 8
  %concat = call i8* @zen_concat(i8* %d2, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @11, i32 0, i32 0))
  %call = call i8* @contar_archivo(i8* %concat)
  %d3 = load i8*, i8** %d1, align 8
  %concat4 = call i8* @zen_concat(i8* %d3, i8* getelementptr inbounds ([14 x i8], [14 x i8]* @12, i32 0, i32 0))
  %call5 = call i8* @contar_archivo(i8* %concat4)
  %d6 = load i8*, i8** %d1, align 8
  %concat7 = call i8* @zen_concat(i8* %d6, i8* getelementptr inbounds ([13 x i8], [13 x i8]* @13, i32 0, i32 0))
  %call8 = call i8* @contar_archivo(i8* %concat7)
  %d9 = load i8*, i8** %d1, align 8
  %concat10 = call i8* @zen_concat(i8* %d9, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @14, i32 0, i32 0))
  %call11 = call i8* @contar_archivo(i8* %concat10)
  %d12 = load i8*, i8** %d1, align 8
  %concat13 = call i8* @zen_concat(i8* %d12, i8* getelementptr inbounds ([10 x i8], [10 x i8]* @15, i32 0, i32 0))
  %call14 = call i8* @contar_archivo(i8* %concat13)
  %d15 = load i8*, i8** %d1, align 8
  %concat16 = call i8* @zen_concat(i8* %d15, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @16, i32 0, i32 0))
  %call17 = call i8* @contar_archivo(i8* %concat16)
  %d18 = load i8*, i8** %d1, align 8
  %concat19 = call i8* @zen_concat(i8* %d18, i8* getelementptr inbounds ([18 x i8], [18 x i8]* @17, i32 0, i32 0))
  %call20 = call i8* @contar_archivo(i8* %concat19)
  ret i8* null
}

define i32 @main(i32 %argc, i8** %argv) {
entry:
  %arg = alloca i8*, align 8
  store i32 %argc, i32* @__zen_argc, align 4
  store i8** %argv, i8*** @__zen_argv, align 8
  %chrbuf = call i8* @malloc(i64 2)
  store i8 27, i8* %chrbuf, align 1
  %nullptr = getelementptr i8, i8* %chrbuf, i64 1
  store i8 0, i8* %nullptr, align 1
  store i8* %chrbuf, i8** @g_ESC, align 8
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
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @18, i32 0, i32 0), i8** @g_czl_dir, align 8
  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @19, i32 0, i32 0), i8** @g_czl_excluir, align 8
  %ESC = load i8*, i8** @g_ESC, align 8
  %concat = call i8* @zen_concat(i8* %ESC, i8* getelementptr inbounds ([47 x i8], [47 x i8]* @20, i32 0, i32 0))
  %ESC5 = load i8*, i8** @g_ESC, align 8
  %concat6 = call i8* @zen_concat(i8* %concat, i8* %ESC5)
  %concat7 = call i8* @zen_concat(i8* %concat6, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @21, i32 0, i32 0))
  store i8* %concat7, i8** @g_header, align 8
  %ESC8 = load i8*, i8** @g_ESC, align 8
  %concat9 = call i8* @zen_concat(i8* %ESC8, i8* getelementptr inbounds ([32 x i8], [32 x i8]* @22, i32 0, i32 0))
  %ESC10 = load i8*, i8** @g_ESC, align 8
  %concat11 = call i8* @zen_concat(i8* %concat9, i8* %ESC10)
  %concat12 = call i8* @zen_concat(i8* %concat11, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @23, i32 0, i32 0))
  store i8* %concat12, i8** @g_title, align 8
  %ESC13 = load i8*, i8** @g_ESC, align 8
  %concat14 = call i8* @zen_concat(i8* %ESC13, i8* getelementptr inbounds ([47 x i8], [47 x i8]* @24, i32 0, i32 0))
  %ESC15 = load i8*, i8** @g_ESC, align 8
  %concat16 = call i8* @zen_concat(i8* %concat14, i8* %ESC15)
  %concat17 = call i8* @zen_concat(i8* %concat16, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @25, i32 0, i32 0))
  store i8* %concat17, i8** @g_sep, align 8
  %ESC18 = load i8*, i8** @g_ESC, align 8
  %concat19 = call i8* @zen_concat(i8* %ESC18, i8* getelementptr inbounds ([68 x i8], [68 x i8]* @26, i32 0, i32 0))
  %ESC20 = load i8*, i8** @g_ESC, align 8
  %concat21 = call i8* @zen_concat(i8* %concat19, i8* %ESC20)
  %concat22 = call i8* @zen_concat(i8* %concat21, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @27, i32 0, i32 0))
  store i8* %concat22, i8** @g_lang_hdr, align 8
  %ESC23 = load i8*, i8** @g_ESC, align 8
  %concat24 = call i8* @zen_concat(i8* %ESC23, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @28, i32 0, i32 0))
  store i8* %concat24, i8** @g_reset, align 8
  %ESC25 = load i8*, i8** @g_ESC, align 8
  %concat26 = call i8* @zen_concat(i8* %ESC25, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @29, i32 0, i32 0))
  store i8* %concat26, i8** @g_green, align 8
  %ESC27 = load i8*, i8** @g_ESC, align 8
  %concat28 = call i8* @zen_concat(i8* %ESC27, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @30, i32 0, i32 0))
  store i8* %concat28, i8** @g_cyan, align 8
  %header = load i8*, i8** @g_header, align 8
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @31, i32 0, i32 0), i8* %header)
  %title = load i8*, i8** @g_title, align 8
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @32, i32 0, i32 0), i8* %title)
  %header29 = load i8*, i8** @g_header, align 8
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @33, i32 0, i32 0), i8* %header29)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @35, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @34, i32 0, i32 0))
  %argc30 = load i32, i32* @__zen_argc, align 4
  %argcd = sitofp i32 %argc30 to double
  %numstr31 = call i8* @__zen_num_to_str(double %argcd)
  store i8* %numstr31, i8** @g_n, align 8
  %numstr32 = call i8* @__zen_num_to_str(double 1.000000e+00)
  store i8* %numstr32, i8** @g_idx, align 8
  br label %while.cond

while.cond:                                       ; preds = %endif, %entry
  %idx = load i8*, i8** @g_idx, align 8
  %n = load i8*, i8** @g_n, align 8
  %s2d = call double @strtod(i8* %idx, i8** null)
  %s2d33 = call double @strtod(i8* %n, i8** null)
  %cmp = fcmp olt double %s2d, %s2d33
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %idx34 = load i8*, i8** @g_idx, align 8
  %s2d35 = call double @strtod(i8* %idx34, i8** null)
  %argidx = fptosi double %s2d35 to i32
  %argv36 = load i8**, i8*** @__zen_argv, align 8
  %arggep = getelementptr i8*, i8** %argv36, i32 %argidx
  %argval = load i8*, i8** %arggep, align 8
  store i8* %argval, i8** %arg, align 8
  %arg37 = load i8*, i8** %arg, align 8
  %strcmp = call i32 @strcmp(i8* %arg37, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @36, i32 0, i32 0))
  %scmp = icmp eq i32 %strcmp, 0
  br i1 %scmp, label %then, label %else

while.end:                                        ; preds = %while.cond
  %czl_dir = load i8*, i8** @g_czl_dir, align 8
  %call = call i8* @contar_dir(i8* %czl_dir)
  %lang_hdr = load i8*, i8** @g_lang_hdr, align 8
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @37, i32 0, i32 0), i8* %lang_hdr)
  %sep = load i8*, i8** @g_sep, align 8
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @38, i32 0, i32 0), i8* %sep)
  %green = load i8*, i8** @g_green, align 8
  %concat59 = call i8* @zen_concat(i8* %green, i8* getelementptr inbounds ([17 x i8], [17 x i8]* @39, i32 0, i32 0))
  %czl_archivos = load i8*, i8** @g_czl_archivos, align 8
  %concat60 = call i8* @zen_concat(i8* %concat59, i8* %czl_archivos)
  %concat61 = call i8* @zen_concat(i8* %concat60, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @40, i32 0, i32 0))
  %czl_total = load i8*, i8** @g_czl_total, align 8
  %concat62 = call i8* @zen_concat(i8* %concat61, i8* %czl_total)
  %concat63 = call i8* @zen_concat(i8* %concat62, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @41, i32 0, i32 0))
  %czl_codigo = load i8*, i8** @g_czl_codigo, align 8
  %concat64 = call i8* @zen_concat(i8* %concat63, i8* %czl_codigo)
  %concat65 = call i8* @zen_concat(i8* %concat64, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @42, i32 0, i32 0))
  %czl_comentarios = load i8*, i8** @g_czl_comentarios, align 8
  %concat66 = call i8* @zen_concat(i8* %concat65, i8* %czl_comentarios)
  %concat67 = call i8* @zen_concat(i8* %concat66, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @43, i32 0, i32 0))
  %czl_vacias = load i8*, i8** @g_czl_vacias, align 8
  %concat68 = call i8* @zen_concat(i8* %concat67, i8* %czl_vacias)
  %reset = load i8*, i8** @g_reset, align 8
  %concat69 = call i8* @zen_concat(i8* %concat68, i8* %reset)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @44, i32 0, i32 0), i8* %concat69)
  %sep70 = load i8*, i8** @g_sep, align 8
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @45, i32 0, i32 0), i8* %sep70)
  %cyan = load i8*, i8** @g_cyan, align 8
  %concat71 = call i8* @zen_concat(i8* %cyan, i8* getelementptr inbounds ([17 x i8], [17 x i8]* @46, i32 0, i32 0))
  %czl_archivos72 = load i8*, i8** @g_czl_archivos, align 8
  %concat73 = call i8* @zen_concat(i8* %concat71, i8* %czl_archivos72)
  %concat74 = call i8* @zen_concat(i8* %concat73, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @47, i32 0, i32 0))
  %czl_total75 = load i8*, i8** @g_czl_total, align 8
  %concat76 = call i8* @zen_concat(i8* %concat74, i8* %czl_total75)
  %concat77 = call i8* @zen_concat(i8* %concat76, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @48, i32 0, i32 0))
  %czl_codigo78 = load i8*, i8** @g_czl_codigo, align 8
  %concat79 = call i8* @zen_concat(i8* %concat77, i8* %czl_codigo78)
  %concat80 = call i8* @zen_concat(i8* %concat79, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @49, i32 0, i32 0))
  %czl_comentarios81 = load i8*, i8** @g_czl_comentarios, align 8
  %concat82 = call i8* @zen_concat(i8* %concat80, i8* %czl_comentarios81)
  %concat83 = call i8* @zen_concat(i8* %concat82, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @50, i32 0, i32 0))
  %czl_vacias84 = load i8*, i8** @g_czl_vacias, align 8
  %concat85 = call i8* @zen_concat(i8* %concat83, i8* %czl_vacias84)
  %reset86 = load i8*, i8** @g_reset, align 8
  %concat87 = call i8* @zen_concat(i8* %concat85, i8* %reset86)
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @51, i32 0, i32 0), i8* %concat87)
  %header88 = load i8*, i8** @g_header, align 8
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @52, i32 0, i32 0), i8* %header88)
  ret i32 0

then:                                             ; preds = %while.body
  %idx38 = load i8*, i8** @g_idx, align 8
  %s2d39 = call double @strtod(i8* %idx38, i8** null)
  %add = fadd double %s2d39, 1.000000e+00
  %numstr40 = call i8* @__zen_num_to_str(double %add)
  store i8* %numstr40, i8** @g_idx, align 8
  %idx41 = load i8*, i8** @g_idx, align 8
  %n42 = load i8*, i8** @g_n, align 8
  %s2d43 = call double @strtod(i8* %idx41, i8** null)
  %s2d44 = call double @strtod(i8* %n42, i8** null)
  %cmp45 = fcmp olt double %s2d43, %s2d44
  br i1 %cmp45, label %then46, label %endif47

else:                                             ; preds = %while.body
  %arg54 = load i8*, i8** %arg, align 8
  store i8* %arg54, i8** @g_czl_dir, align 8
  br label %endif

endif:                                            ; preds = %else, %endif47
  %idx55 = load i8*, i8** @g_idx, align 8
  %s2d56 = call double @strtod(i8* %idx55, i8** null)
  %add57 = fadd double %s2d56, 1.000000e+00
  %numstr58 = call i8* @__zen_num_to_str(double %add57)
  store i8* %numstr58, i8** @g_idx, align 8
  br label %while.cond

then46:                                           ; preds = %then
  %idx48 = load i8*, i8** @g_idx, align 8
  %s2d49 = call double @strtod(i8* %idx48, i8** null)
  %argidx50 = fptosi double %s2d49 to i32
  %argv51 = load i8**, i8*** @__zen_argv, align 8
  %arggep52 = getelementptr i8*, i8** %argv51, i32 %argidx50
  %argval53 = load i8*, i8** %arggep52, align 8
  store i8* %argval53, i8** @g_czl_excluir, align 8
  br label %endif47

endif47:                                          ; preds = %then46, %then
  br label %endif
}
