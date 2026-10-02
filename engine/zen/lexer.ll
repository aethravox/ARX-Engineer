; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@g_codigo = internal global i8* null
@g_tokens = internal global i8* null
@3 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@5 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@7 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@9 = private unnamed_addr constant [3 x i8] c"si\00", align 1
@10 = private unnamed_addr constant [3 x i8] c"if\00", align 1
@11 = private unnamed_addr constant [5 x i8] c"sino\00", align 1
@12 = private unnamed_addr constant [5 x i8] c"else\00", align 1
@13 = private unnamed_addr constant [9 x i8] c"mientras\00", align 1
@14 = private unnamed_addr constant [6 x i8] c"while\00", align 1
@15 = private unnamed_addr constant [5 x i8] c"para\00", align 1
@16 = private unnamed_addr constant [4 x i8] c"for\00", align 1
@17 = private unnamed_addr constant [8 x i8] c"funcion\00", align 1
@18 = private unnamed_addr constant [9 x i8] c"function\00", align 1
@19 = private unnamed_addr constant [8 x i8] c"retorna\00", align 1
@20 = private unnamed_addr constant [7 x i8] c"return\00", align 1
@21 = private unnamed_addr constant [8 x i8] c"muestra\00", align 1
@22 = private unnamed_addr constant [5 x i8] c"show\00", align 1
@23 = private unnamed_addr constant [6 x i8] c"print\00", align 1
@24 = private unnamed_addr constant [7 x i8] c"verdad\00", align 1
@25 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@26 = private unnamed_addr constant [6 x i8] c"falso\00", align 1
@27 = private unnamed_addr constant [5 x i8] c"nada\00", align 1
@28 = private unnamed_addr constant [8 x i8] c"nothing\00", align 1
@29 = private unnamed_addr constant [8 x i8] c"NEWLINE\00", align 1
@30 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@31 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@32 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@33 = private unnamed_addr constant [3 x i8] c"==\00", align 1
@34 = private unnamed_addr constant [3 x i8] c"!=\00", align 1
@35 = private unnamed_addr constant [3 x i8] c"<=\00", align 1
@36 = private unnamed_addr constant [3 x i8] c">=\00", align 1
@37 = private unnamed_addr constant [3 x i8] c"->\00", align 1
@38 = private unnamed_addr constant [2 x i8] c"+\00", align 1
@39 = private unnamed_addr constant [2 x i8] c"-\00", align 1
@40 = private unnamed_addr constant [2 x i8] c"*\00", align 1
@41 = private unnamed_addr constant [2 x i8] c"/\00", align 1
@42 = private unnamed_addr constant [2 x i8] c"=\00", align 1
@43 = private unnamed_addr constant [2 x i8] c"<\00", align 1
@44 = private unnamed_addr constant [2 x i8] c">\00", align 1
@45 = private unnamed_addr constant [2 x i8] c"(\00", align 1
@46 = private unnamed_addr constant [2 x i8] c")\00", align 1
@47 = private unnamed_addr constant [2 x i8] c",\00", align 1
@48 = private unnamed_addr constant [2 x i8] c".\00", align 1
@49 = private unnamed_addr constant [2 x i8] c":\00", align 1
@50 = private unnamed_addr constant [2 x i8] c"[\00", align 1
@51 = private unnamed_addr constant [2 x i8] c"]\00", align 1
@52 = private unnamed_addr constant [2 x i8] c"{\00", align 1
@53 = private unnamed_addr constant [2 x i8] c"}\00", align 1
@54 = private unnamed_addr constant [4 x i8] c"EOF\00", align 1
@55 = private unnamed_addr constant [36 x i8] c"=== Lexer de Zen escrito en Zen ===\00", align 1
@56 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@57 = private unnamed_addr constant [11 x i8] c"muestra 42\00", align 1
@58 = private unnamed_addr constant [14 x i8] c"Tokenizando: \00", align 1
@59 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@60 = private unnamed_addr constant [19 x i8] c"Tokens generados: \00", align 1
@61 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@62 = private unnamed_addr constant [26 x i8] c"Lexer en Zen funcionando!\00", align 1
@63 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

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

define i8* @es_digito(i8* %c) {
entry:
  %c1 = alloca i8*, align 8
  store i8* %c, i8** %c1, align 8
  %c2 = load i8*, i8** %c1, align 8
  %s2d = call double @strtod(i8* %c2, i8** null)
  %cmp = fcmp oge double %s2d, 4.800000e+01
  %c3 = load i8*, i8** %c1, align 8
  %s2d4 = call double @strtod(i8* %c3, i8** null)
  %cmp5 = fcmp ole double %s2d4, 5.700000e+01
  %and = and i1 %cmp, %cmp5
  %boolstr = select i1 %and, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @3, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @4, i32 0, i32 0)
  ret i8* %boolstr
}

define i8* @es_letra(i8* %c) {
entry:
  %c1 = alloca i8*, align 8
  store i8* %c, i8** %c1, align 8
  %c2 = load i8*, i8** %c1, align 8
  %s2d = call double @strtod(i8* %c2, i8** null)
  %cmp = fcmp oge double %s2d, 9.700000e+01
  %c3 = load i8*, i8** %c1, align 8
  %s2d4 = call double @strtod(i8* %c3, i8** null)
  %cmp5 = fcmp ole double %s2d4, 1.220000e+02
  %and = and i1 %cmp, %cmp5
  %c6 = load i8*, i8** %c1, align 8
  %s2d7 = call double @strtod(i8* %c6, i8** null)
  %cmp8 = fcmp oge double %s2d7, 6.500000e+01
  %c9 = load i8*, i8** %c1, align 8
  %s2d10 = call double @strtod(i8* %c9, i8** null)
  %cmp11 = fcmp ole double %s2d10, 9.000000e+01
  %and12 = and i1 %cmp8, %cmp11
  %or = or i1 %and, %and12
  %c13 = load i8*, i8** %c1, align 8
  %s2d14 = call double @strtod(i8* %c13, i8** null)
  %cmp15 = fcmp oeq double %s2d14, 9.500000e+01
  %or16 = or i1 %or, %cmp15
  %boolstr = select i1 %or16, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @5, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @6, i32 0, i32 0)
  ret i8* %boolstr
}

define i8* @es_alfa_num(i8* %c) {
entry:
  %c1 = alloca i8*, align 8
  store i8* %c, i8** %c1, align 8
  %c2 = load i8*, i8** %c1, align 8
  %call = call i8* @es_letra(i8* %c2)
  %c3 = load i8*, i8** %c1, align 8
  %call4 = call i8* @es_digito(i8* %c3)
  %tobool = icmp ne i8* %call4, null
  %tobool5 = icmp ne i8* %call, null
  %or = or i1 %tobool5, %tobool
  %boolstr = select i1 %or, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @7, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @8, i32 0, i32 0)
  ret i8* %boolstr
}

define i8* @tipo_keyword(i8* %palabra) {
entry:
  %palabra1 = alloca i8*, align 8
  store i8* %palabra, i8** %palabra1, align 8
  %palabra2 = load i8*, i8** %palabra1, align 8
  %strcmp = call i32 @strcmp(i8* %palabra2, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @9, i32 0, i32 0))
  %scmp = icmp eq i32 %strcmp, 0
  %palabra3 = load i8*, i8** %palabra1, align 8
  %strcmp4 = call i32 @strcmp(i8* %palabra3, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @10, i32 0, i32 0))
  %scmp5 = icmp eq i32 %strcmp4, 0
  %or = or i1 %scmp, %scmp5
  br i1 %or, label %then, label %endif

then:                                             ; preds = %entry
  %numstr = call i8* @__zen_num_to_str(double 4.000000e+00)
  ret i8* %numstr

endif:                                            ; preds = %entry
  %palabra6 = load i8*, i8** %palabra1, align 8
  %strcmp7 = call i32 @strcmp(i8* %palabra6, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @11, i32 0, i32 0))
  %scmp8 = icmp eq i32 %strcmp7, 0
  %palabra9 = load i8*, i8** %palabra1, align 8
  %strcmp10 = call i32 @strcmp(i8* %palabra9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @12, i32 0, i32 0))
  %scmp11 = icmp eq i32 %strcmp10, 0
  %or12 = or i1 %scmp8, %scmp11
  br i1 %or12, label %then13, label %endif14

then13:                                           ; preds = %endif
  %numstr15 = call i8* @__zen_num_to_str(double 5.000000e+00)
  ret i8* %numstr15

endif14:                                          ; preds = %endif
  %palabra16 = load i8*, i8** %palabra1, align 8
  %strcmp17 = call i32 @strcmp(i8* %palabra16, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @13, i32 0, i32 0))
  %scmp18 = icmp eq i32 %strcmp17, 0
  %palabra19 = load i8*, i8** %palabra1, align 8
  %strcmp20 = call i32 @strcmp(i8* %palabra19, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @14, i32 0, i32 0))
  %scmp21 = icmp eq i32 %strcmp20, 0
  %or22 = or i1 %scmp18, %scmp21
  br i1 %or22, label %then23, label %endif24

then23:                                           ; preds = %endif14
  %numstr25 = call i8* @__zen_num_to_str(double 1.000000e+01)
  ret i8* %numstr25

endif24:                                          ; preds = %endif14
  %palabra26 = load i8*, i8** %palabra1, align 8
  %strcmp27 = call i32 @strcmp(i8* %palabra26, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @15, i32 0, i32 0))
  %scmp28 = icmp eq i32 %strcmp27, 0
  %palabra29 = load i8*, i8** %palabra1, align 8
  %strcmp30 = call i32 @strcmp(i8* %palabra29, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @16, i32 0, i32 0))
  %scmp31 = icmp eq i32 %strcmp30, 0
  %or32 = or i1 %scmp28, %scmp31
  br i1 %or32, label %then33, label %endif34

then33:                                           ; preds = %endif24
  %numstr35 = call i8* @__zen_num_to_str(double 7.000000e+00)
  ret i8* %numstr35

endif34:                                          ; preds = %endif24
  %palabra36 = load i8*, i8** %palabra1, align 8
  %strcmp37 = call i32 @strcmp(i8* %palabra36, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @17, i32 0, i32 0))
  %scmp38 = icmp eq i32 %strcmp37, 0
  %palabra39 = load i8*, i8** %palabra1, align 8
  %strcmp40 = call i32 @strcmp(i8* %palabra39, i8* getelementptr inbounds ([9 x i8], [9 x i8]* @18, i32 0, i32 0))
  %scmp41 = icmp eq i32 %strcmp40, 0
  %or42 = or i1 %scmp38, %scmp41
  br i1 %or42, label %then43, label %endif44

then43:                                           ; preds = %endif34
  %numstr45 = call i8* @__zen_num_to_str(double 1.700000e+01)
  ret i8* %numstr45

endif44:                                          ; preds = %endif34
  %palabra46 = load i8*, i8** %palabra1, align 8
  %strcmp47 = call i32 @strcmp(i8* %palabra46, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @19, i32 0, i32 0))
  %scmp48 = icmp eq i32 %strcmp47, 0
  %palabra49 = load i8*, i8** %palabra1, align 8
  %strcmp50 = call i32 @strcmp(i8* %palabra49, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @20, i32 0, i32 0))
  %scmp51 = icmp eq i32 %strcmp50, 0
  %or52 = or i1 %scmp48, %scmp51
  br i1 %or52, label %then53, label %endif54

then53:                                           ; preds = %endif44
  %numstr55 = call i8* @__zen_num_to_str(double 1.800000e+01)
  ret i8* %numstr55

endif54:                                          ; preds = %endif44
  %palabra56 = load i8*, i8** %palabra1, align 8
  %strcmp57 = call i32 @strcmp(i8* %palabra56, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @21, i32 0, i32 0))
  %scmp58 = icmp eq i32 %strcmp57, 0
  %palabra59 = load i8*, i8** %palabra1, align 8
  %strcmp60 = call i32 @strcmp(i8* %palabra59, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @22, i32 0, i32 0))
  %scmp61 = icmp eq i32 %strcmp60, 0
  %or62 = or i1 %scmp58, %scmp61
  %palabra63 = load i8*, i8** %palabra1, align 8
  %strcmp64 = call i32 @strcmp(i8* %palabra63, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @23, i32 0, i32 0))
  %scmp65 = icmp eq i32 %strcmp64, 0
  %or66 = or i1 %or62, %scmp65
  br i1 %or66, label %then67, label %endif68

then67:                                           ; preds = %endif54
  %numstr69 = call i8* @__zen_num_to_str(double 1.900000e+01)
  ret i8* %numstr69

endif68:                                          ; preds = %endif54
  %palabra70 = load i8*, i8** %palabra1, align 8
  %strcmp71 = call i32 @strcmp(i8* %palabra70, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @24, i32 0, i32 0))
  %scmp72 = icmp eq i32 %strcmp71, 0
  %palabra73 = load i8*, i8** %palabra1, align 8
  %strcmp74 = call i32 @strcmp(i8* %palabra73, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @25, i32 0, i32 0))
  %scmp75 = icmp eq i32 %strcmp74, 0
  %or76 = or i1 %scmp72, %scmp75
  br i1 %or76, label %then77, label %endif78

then77:                                           ; preds = %endif68
  %numstr79 = call i8* @__zen_num_to_str(double 3.400000e+01)
  ret i8* %numstr79

endif78:                                          ; preds = %endif68
  %palabra80 = load i8*, i8** %palabra1, align 8
  %strcmp81 = call i32 @strcmp(i8* %palabra80, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @26, i32 0, i32 0))
  %scmp82 = icmp eq i32 %strcmp81, 0
  br i1 %scmp82, label %then83, label %endif84

then83:                                           ; preds = %endif78
  %numstr85 = call i8* @__zen_num_to_str(double 3.500000e+01)
  ret i8* %numstr85

endif84:                                          ; preds = %endif78
  %palabra86 = load i8*, i8** %palabra1, align 8
  %strcmp87 = call i32 @strcmp(i8* %palabra86, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @27, i32 0, i32 0))
  %scmp88 = icmp eq i32 %strcmp87, 0
  %palabra89 = load i8*, i8** %palabra1, align 8
  %strcmp90 = call i32 @strcmp(i8* %palabra89, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @28, i32 0, i32 0))
  %scmp91 = icmp eq i32 %strcmp90, 0
  %or92 = or i1 %scmp88, %scmp91
  br i1 %or92, label %then93, label %endif94

then93:                                           ; preds = %endif84
  %numstr95 = call i8* @__zen_num_to_str(double 3.600000e+01)
  ret i8* %numstr95

endif94:                                          ; preds = %endif84
  %numstr96 = call i8* @__zen_num_to_str(double 3.000000e+00)
  ret i8* %numstr96
}

define i8* @tokenizar(i8* %codigo) {
entry:
  %c2 = alloca i8*, align 8
  %tipo = alloca i8*, align 8
  %ch92 = alloca i8*, align 8
  %valor = alloca i8*, align 8
  %comilla = alloca i8*, align 8
  %c = alloca i8*, align 8
  %linea = alloca i8*, align 8
  %pos = alloca i8*, align 8
  %len3 = alloca i8*, align 8
  %codigo1 = alloca i8*, align 8
  store i8* %codigo, i8** %codigo1, align 8
  %list = call i8* @__zen_list_create(i64 4)
  store i8* %list, i8** @g_tokens, align 8
  %codigo2 = load i8*, i8** %codigo1, align 8
  %len = call i64 @strlen(i8* %codigo2)
  %lend = uitofp i64 %len to double
  %numstr = call i8* @__zen_num_to_str(double %lend)
  store i8* %numstr, i8** %len3, align 8
  %numstr4 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr4, i8** %pos, align 8
  %numstr5 = call i8* @__zen_num_to_str(double 1.000000e+00)
  store i8* %numstr5, i8** %linea, align 8
  br label %while.cond

while.cond:                                       ; preds = %endif321, %then305, %then287, %then269, %then251, %then233, %while.end170, %while.end123, %while.end79, %while.end45, %then25, %then, %entry
  %pos6 = load i8*, i8** %pos, align 8
  %len7 = load i8*, i8** %len3, align 8
  %s2d = call double @strtod(i8* %pos6, i8** null)
  %s2d8 = call double @strtod(i8* %len7, i8** null)
  %cmp = fcmp olt double %s2d, %s2d8
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %codigo9 = load i8*, i8** %codigo1, align 8
  %pos10 = load i8*, i8** %pos, align 8
  %s2d11 = call double @strtod(i8* %pos10, i8** null)
  %idx = fptosi double %s2d11 to i64
  %charptr = getelementptr i8, i8* %codigo9, i64 %idx
  %ch = load i8, i8* %charptr, align 1
  %chd = sitofp i8 %ch to double
  %numstr12 = call i8* @__zen_num_to_str(double %chd)
  store i8* %numstr12, i8** %c, align 8
  %c13 = load i8*, i8** %c, align 8
  %s2d14 = call double @strtod(i8* %c13, i8** null)
  %cmp15 = fcmp oeq double %s2d14, 3.200000e+01
  %c16 = load i8*, i8** %c, align 8
  %s2d17 = call double @strtod(i8* %c16, i8** null)
  %cmp18 = fcmp oeq double %s2d17, 9.000000e+00
  %or = or i1 %cmp15, %cmp18
  br i1 %or, label %then, label %endif

while.end:                                        ; preds = %while.cond
  %tokens495 = load i8*, i8** @g_tokens, align 8
  %struct496 = call i8* @__zen_struct_create(i64 4)
  %numstr497 = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_struct_set(i8* %struct496, i64 0, i8* %numstr497)
  call void @__zen_struct_set(i8* %struct496, i64 1, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @54, i32 0, i32 0))
  %linea498 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct496, i64 2, i8* %linea498)
  %pos499 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct496, i64 3, i8* %pos499)
  call void @__zen_list_push(i8* %tokens495, i8* %struct496)
  %tokens500 = load i8*, i8** @g_tokens, align 8
  ret i8* %tokens500

then:                                             ; preds = %while.body
  %pos19 = load i8*, i8** %pos, align 8
  %s2d20 = call double @strtod(i8* %pos19, i8** null)
  %add = fadd double %s2d20, 1.000000e+00
  %numstr21 = call i8* @__zen_num_to_str(double %add)
  store i8* %numstr21, i8** %pos, align 8
  br label %while.cond

endif:                                            ; preds = %while.body
  %c22 = load i8*, i8** %c, align 8
  %s2d23 = call double @strtod(i8* %c22, i8** null)
  %cmp24 = fcmp oeq double %s2d23, 1.000000e+01
  br i1 %cmp24, label %then25, label %endif26

then25:                                           ; preds = %endif
  %tokens = load i8*, i8** @g_tokens, align 8
  %struct = call i8* @__zen_struct_create(i64 4)
  %numstr27 = call i8* @__zen_num_to_str(double 6.700000e+01)
  call void @__zen_struct_set(i8* %struct, i64 0, i8* %numstr27)
  call void @__zen_struct_set(i8* %struct, i64 1, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @29, i32 0, i32 0))
  %linea28 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct, i64 2, i8* %linea28)
  %pos29 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct, i64 3, i8* %pos29)
  call void @__zen_list_push(i8* %tokens, i8* %struct)
  %pos30 = load i8*, i8** %pos, align 8
  %s2d31 = call double @strtod(i8* %pos30, i8** null)
  %add32 = fadd double %s2d31, 1.000000e+00
  %numstr33 = call i8* @__zen_num_to_str(double %add32)
  store i8* %numstr33, i8** %pos, align 8
  %linea34 = load i8*, i8** %linea, align 8
  %s2d35 = call double @strtod(i8* %linea34, i8** null)
  %add36 = fadd double %s2d35, 1.000000e+00
  %numstr37 = call i8* @__zen_num_to_str(double %add36)
  store i8* %numstr37, i8** %linea, align 8
  br label %while.cond

endif26:                                          ; preds = %endif
  %c38 = load i8*, i8** %c, align 8
  %s2d39 = call double @strtod(i8* %c38, i8** null)
  %cmp40 = fcmp oeq double %s2d39, 3.500000e+01
  br i1 %cmp40, label %then41, label %endif42

then41:                                           ; preds = %endif26
  br label %while.cond43

endif42:                                          ; preds = %endif26
  %c63 = load i8*, i8** %c, align 8
  %s2d64 = call double @strtod(i8* %c63, i8** null)
  %cmp65 = fcmp oeq double %s2d64, 3.400000e+01
  %c66 = load i8*, i8** %c, align 8
  %s2d67 = call double @strtod(i8* %c66, i8** null)
  %cmp68 = fcmp oeq double %s2d67, 3.900000e+01
  %or69 = or i1 %cmp65, %cmp68
  br i1 %or69, label %then70, label %endif71

while.cond43:                                     ; preds = %while.body44, %then41
  %pos46 = load i8*, i8** %pos, align 8
  %len47 = load i8*, i8** %len3, align 8
  %s2d48 = call double @strtod(i8* %pos46, i8** null)
  %s2d49 = call double @strtod(i8* %len47, i8** null)
  %cmp50 = fcmp olt double %s2d48, %s2d49
  %codigo51 = load i8*, i8** %codigo1, align 8
  %pos52 = load i8*, i8** %pos, align 8
  %s2d53 = call double @strtod(i8* %pos52, i8** null)
  %idx54 = fptosi double %s2d53 to i64
  %charptr55 = getelementptr i8, i8* %codigo51, i64 %idx54
  %ch56 = load i8, i8* %charptr55, align 1
  %chd57 = sitofp i8 %ch56 to double
  %cmp58 = fcmp one double %chd57, 1.000000e+01
  %and = and i1 %cmp50, %cmp58
  br i1 %and, label %while.body44, label %while.end45

while.body44:                                     ; preds = %while.cond43
  %pos59 = load i8*, i8** %pos, align 8
  %s2d60 = call double @strtod(i8* %pos59, i8** null)
  %add61 = fadd double %s2d60, 1.000000e+00
  %numstr62 = call i8* @__zen_num_to_str(double %add61)
  store i8* %numstr62, i8** %pos, align 8
  br label %while.cond43

while.end45:                                      ; preds = %while.cond43
  br label %while.cond

then70:                                           ; preds = %endif42
  %c72 = load i8*, i8** %c, align 8
  store i8* %c72, i8** %comilla, align 8
  %pos73 = load i8*, i8** %pos, align 8
  %s2d74 = call double @strtod(i8* %pos73, i8** null)
  %add75 = fadd double %s2d74, 1.000000e+00
  %numstr76 = call i8* @__zen_num_to_str(double %add75)
  store i8* %numstr76, i8** %pos, align 8
  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @30, i32 0, i32 0), i8** %valor, align 8
  br label %while.cond77

endif71:                                          ; preds = %endif42
  %c118 = load i8*, i8** %c, align 8
  %call = call i8* @es_digito(i8* %c118)
  %tobool = icmp ne i8* %call, null
  br i1 %tobool, label %then119, label %endif120

while.cond77:                                     ; preds = %endif100, %then70
  %pos80 = load i8*, i8** %pos, align 8
  %len81 = load i8*, i8** %len3, align 8
  %s2d82 = call double @strtod(i8* %pos80, i8** null)
  %s2d83 = call double @strtod(i8* %len81, i8** null)
  %cmp84 = fcmp olt double %s2d82, %s2d83
  br i1 %cmp84, label %while.body78, label %while.end79

while.body78:                                     ; preds = %while.cond77
  %codigo85 = load i8*, i8** %codigo1, align 8
  %pos86 = load i8*, i8** %pos, align 8
  %s2d87 = call double @strtod(i8* %pos86, i8** null)
  %idx88 = fptosi double %s2d87 to i64
  %charptr89 = getelementptr i8, i8* %codigo85, i64 %idx88
  %ch90 = load i8, i8* %charptr89, align 1
  %chd91 = sitofp i8 %ch90 to double
  %numstr93 = call i8* @__zen_num_to_str(double %chd91)
  store i8* %numstr93, i8** %ch92, align 8
  %ch94 = load i8*, i8** %ch92, align 8
  %comilla95 = load i8*, i8** %comilla, align 8
  %s2d96 = call double @strtod(i8* %ch94, i8** null)
  %s2d97 = call double @strtod(i8* %comilla95, i8** null)
  %cmp98 = fcmp oeq double %s2d96, %s2d97
  br i1 %cmp98, label %then99, label %endif100

while.end79:                                      ; preds = %then99, %while.cond77
  %tokens112 = load i8*, i8** @g_tokens, align 8
  %struct113 = call i8* @__zen_struct_create(i64 4)
  %numstr114 = call i8* @__zen_num_to_str(double 2.000000e+00)
  call void @__zen_struct_set(i8* %struct113, i64 0, i8* %numstr114)
  %valor115 = load i8*, i8** %valor, align 8
  call void @__zen_struct_set(i8* %struct113, i64 1, i8* %valor115)
  %linea116 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct113, i64 2, i8* %linea116)
  %pos117 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct113, i64 3, i8* %pos117)
  call void @__zen_list_push(i8* %tokens112, i8* %struct113)
  br label %while.cond

then99:                                           ; preds = %while.body78
  %pos101 = load i8*, i8** %pos, align 8
  %s2d102 = call double @strtod(i8* %pos101, i8** null)
  %add103 = fadd double %s2d102, 1.000000e+00
  %numstr104 = call i8* @__zen_num_to_str(double %add103)
  store i8* %numstr104, i8** %pos, align 8
  br label %while.end79

endif100:                                         ; preds = %while.body78
  %valor105 = load i8*, i8** %valor, align 8
  %ch106 = load i8*, i8** %ch92, align 8
  %s2d107 = call double @strtod(i8* %ch106, i8** null)
  %chr = fptosi double %s2d107 to i8
  %chrbuf = call i8* @malloc(i64 2)
  store i8 %chr, i8* %chrbuf, align 1
  %nullptr = getelementptr i8, i8* %chrbuf, i64 1
  store i8 0, i8* %nullptr, align 1
  %concat = call i8* @zen_concat(i8* %valor105, i8* %chrbuf)
  store i8* %concat, i8** %valor, align 8
  %pos108 = load i8*, i8** %pos, align 8
  %s2d109 = call double @strtod(i8* %pos108, i8** null)
  %add110 = fadd double %s2d109, 1.000000e+00
  %numstr111 = call i8* @__zen_num_to_str(double %add110)
  store i8* %numstr111, i8** %pos, align 8
  br label %while.cond77

then119:                                          ; preds = %endif71
  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @31, i32 0, i32 0), i8** %valor, align 8
  br label %while.cond121

endif120:                                         ; preds = %endif71
  %c163 = load i8*, i8** %c, align 8
  %call164 = call i8* @es_letra(i8* %c163)
  %tobool165 = icmp ne i8* %call164, null
  br i1 %tobool165, label %then166, label %endif167

while.cond121:                                    ; preds = %endif145, %then119
  %pos124 = load i8*, i8** %pos, align 8
  %len125 = load i8*, i8** %len3, align 8
  %s2d126 = call double @strtod(i8* %pos124, i8** null)
  %s2d127 = call double @strtod(i8* %len125, i8** null)
  %cmp128 = fcmp olt double %s2d126, %s2d127
  br i1 %cmp128, label %while.body122, label %while.end123

while.body122:                                    ; preds = %while.cond121
  %codigo129 = load i8*, i8** %codigo1, align 8
  %pos130 = load i8*, i8** %pos, align 8
  %s2d131 = call double @strtod(i8* %pos130, i8** null)
  %idx132 = fptosi double %s2d131 to i64
  %charptr133 = getelementptr i8, i8* %codigo129, i64 %idx132
  %ch134 = load i8, i8* %charptr133, align 1
  %chd135 = sitofp i8 %ch134 to double
  %numstr136 = call i8* @__zen_num_to_str(double %chd135)
  store i8* %numstr136, i8** %ch92, align 8
  %ch137 = load i8*, i8** %ch92, align 8
  %call138 = call i8* @es_digito(i8* %ch137)
  %ch139 = load i8*, i8** %ch92, align 8
  %s2d140 = call double @strtod(i8* %ch139, i8** null)
  %cmp141 = fcmp oeq double %s2d140, 4.600000e+01
  %tobool142 = icmp ne i8* %call138, null
  %or143 = or i1 %tobool142, %cmp141
  br i1 %or143, label %then144, label %else

while.end123:                                     ; preds = %else, %while.cond121
  %tokens157 = load i8*, i8** @g_tokens, align 8
  %struct158 = call i8* @__zen_struct_create(i64 4)
  %numstr159 = call i8* @__zen_num_to_str(double 1.000000e+00)
  call void @__zen_struct_set(i8* %struct158, i64 0, i8* %numstr159)
  %valor160 = load i8*, i8** %valor, align 8
  call void @__zen_struct_set(i8* %struct158, i64 1, i8* %valor160)
  %linea161 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct158, i64 2, i8* %linea161)
  %pos162 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct158, i64 3, i8* %pos162)
  call void @__zen_list_push(i8* %tokens157, i8* %struct158)
  br label %while.cond

then144:                                          ; preds = %while.body122
  %valor146 = load i8*, i8** %valor, align 8
  %ch147 = load i8*, i8** %ch92, align 8
  %s2d148 = call double @strtod(i8* %ch147, i8** null)
  %chr149 = fptosi double %s2d148 to i8
  %chrbuf150 = call i8* @malloc(i64 2)
  store i8 %chr149, i8* %chrbuf150, align 1
  %nullptr151 = getelementptr i8, i8* %chrbuf150, i64 1
  store i8 0, i8* %nullptr151, align 1
  %concat152 = call i8* @zen_concat(i8* %valor146, i8* %chrbuf150)
  store i8* %concat152, i8** %valor, align 8
  %pos153 = load i8*, i8** %pos, align 8
  %s2d154 = call double @strtod(i8* %pos153, i8** null)
  %add155 = fadd double %s2d154, 1.000000e+00
  %numstr156 = call i8* @__zen_num_to_str(double %add155)
  store i8* %numstr156, i8** %pos, align 8
  br label %endif145

else:                                             ; preds = %while.body122
  br label %while.end123

endif145:                                         ; preds = %then144
  br label %while.cond121

then166:                                          ; preds = %endif120
  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @32, i32 0, i32 0), i8** %valor, align 8
  br label %while.cond168

endif167:                                         ; preds = %endif120
  %pos209 = load i8*, i8** %pos, align 8
  %s2d210 = call double @strtod(i8* %pos209, i8** null)
  %add211 = fadd double %s2d210, 1.000000e+00
  %len212 = load i8*, i8** %len3, align 8
  %s2d213 = call double @strtod(i8* %len212, i8** null)
  %cmp214 = fcmp olt double %add211, %s2d213
  br i1 %cmp214, label %then215, label %endif216

while.cond168:                                    ; preds = %endif189, %then166
  %pos171 = load i8*, i8** %pos, align 8
  %len172 = load i8*, i8** %len3, align 8
  %s2d173 = call double @strtod(i8* %pos171, i8** null)
  %s2d174 = call double @strtod(i8* %len172, i8** null)
  %cmp175 = fcmp olt double %s2d173, %s2d174
  br i1 %cmp175, label %while.body169, label %while.end170

while.body169:                                    ; preds = %while.cond168
  %codigo176 = load i8*, i8** %codigo1, align 8
  %pos177 = load i8*, i8** %pos, align 8
  %s2d178 = call double @strtod(i8* %pos177, i8** null)
  %idx179 = fptosi double %s2d178 to i64
  %charptr180 = getelementptr i8, i8* %codigo176, i64 %idx179
  %ch181 = load i8, i8* %charptr180, align 1
  %chd182 = sitofp i8 %ch181 to double
  %numstr183 = call i8* @__zen_num_to_str(double %chd182)
  store i8* %numstr183, i8** %ch92, align 8
  %ch184 = load i8*, i8** %ch92, align 8
  %call185 = call i8* @es_alfa_num(i8* %ch184)
  %tobool186 = icmp ne i8* %call185, null
  br i1 %tobool186, label %then187, label %else188

while.end170:                                     ; preds = %else188, %while.cond168
  %valor201 = load i8*, i8** %valor, align 8
  %call202 = call i8* @tipo_keyword(i8* %valor201)
  store i8* %call202, i8** %tipo, align 8
  %tokens203 = load i8*, i8** @g_tokens, align 8
  %struct204 = call i8* @__zen_struct_create(i64 4)
  %tipo205 = load i8*, i8** %tipo, align 8
  call void @__zen_struct_set(i8* %struct204, i64 0, i8* %tipo205)
  %valor206 = load i8*, i8** %valor, align 8
  call void @__zen_struct_set(i8* %struct204, i64 1, i8* %valor206)
  %linea207 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct204, i64 2, i8* %linea207)
  %pos208 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct204, i64 3, i8* %pos208)
  call void @__zen_list_push(i8* %tokens203, i8* %struct204)
  br label %while.cond

then187:                                          ; preds = %while.body169
  %valor190 = load i8*, i8** %valor, align 8
  %ch191 = load i8*, i8** %ch92, align 8
  %s2d192 = call double @strtod(i8* %ch191, i8** null)
  %chr193 = fptosi double %s2d192 to i8
  %chrbuf194 = call i8* @malloc(i64 2)
  store i8 %chr193, i8* %chrbuf194, align 1
  %nullptr195 = getelementptr i8, i8* %chrbuf194, i64 1
  store i8 0, i8* %nullptr195, align 1
  %concat196 = call i8* @zen_concat(i8* %valor190, i8* %chrbuf194)
  store i8* %concat196, i8** %valor, align 8
  %pos197 = load i8*, i8** %pos, align 8
  %s2d198 = call double @strtod(i8* %pos197, i8** null)
  %add199 = fadd double %s2d198, 1.000000e+00
  %numstr200 = call i8* @__zen_num_to_str(double %add199)
  store i8* %numstr200, i8** %pos, align 8
  br label %endif189

else188:                                          ; preds = %while.body169
  br label %while.end170

endif189:                                         ; preds = %then187
  br label %while.cond168

then215:                                          ; preds = %endif167
  %codigo217 = load i8*, i8** %codigo1, align 8
  %pos218 = load i8*, i8** %pos, align 8
  %s2d219 = call double @strtod(i8* %pos218, i8** null)
  %add220 = fadd double %s2d219, 1.000000e+00
  %idx221 = fptosi double %add220 to i64
  %charptr222 = getelementptr i8, i8* %codigo217, i64 %idx221
  %ch223 = load i8, i8* %charptr222, align 1
  %chd224 = sitofp i8 %ch223 to double
  %numstr225 = call i8* @__zen_num_to_str(double %chd224)
  store i8* %numstr225, i8** %c2, align 8
  %c226 = load i8*, i8** %c, align 8
  %s2d227 = call double @strtod(i8* %c226, i8** null)
  %cmp228 = fcmp oeq double %s2d227, 6.100000e+01
  %c2229 = load i8*, i8** %c2, align 8
  %s2d230 = call double @strtod(i8* %c2229, i8** null)
  %cmp231 = fcmp oeq double %s2d230, 6.100000e+01
  %and232 = and i1 %cmp228, %cmp231
  br i1 %and232, label %then233, label %endif234

endif216:                                         ; preds = %endif306, %endif167
  %c316 = load i8*, i8** %c, align 8
  %s2d317 = call double @strtod(i8* %c316, i8** null)
  %cmp318 = fcmp oeq double %s2d317, 4.300000e+01
  br i1 %cmp318, label %then319, label %else320

then233:                                          ; preds = %then215
  %tokens235 = load i8*, i8** @g_tokens, align 8
  %struct236 = call i8* @__zen_struct_create(i64 4)
  %numstr237 = call i8* @__zen_num_to_str(double 4.300000e+01)
  call void @__zen_struct_set(i8* %struct236, i64 0, i8* %numstr237)
  call void @__zen_struct_set(i8* %struct236, i64 1, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @33, i32 0, i32 0))
  %linea238 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct236, i64 2, i8* %linea238)
  %pos239 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct236, i64 3, i8* %pos239)
  call void @__zen_list_push(i8* %tokens235, i8* %struct236)
  %pos240 = load i8*, i8** %pos, align 8
  %s2d241 = call double @strtod(i8* %pos240, i8** null)
  %add242 = fadd double %s2d241, 2.000000e+00
  %numstr243 = call i8* @__zen_num_to_str(double %add242)
  store i8* %numstr243, i8** %pos, align 8
  br label %while.cond

endif234:                                         ; preds = %then215
  %c244 = load i8*, i8** %c, align 8
  %s2d245 = call double @strtod(i8* %c244, i8** null)
  %cmp246 = fcmp oeq double %s2d245, 3.300000e+01
  %c2247 = load i8*, i8** %c2, align 8
  %s2d248 = call double @strtod(i8* %c2247, i8** null)
  %cmp249 = fcmp oeq double %s2d248, 6.100000e+01
  %and250 = and i1 %cmp246, %cmp249
  br i1 %and250, label %then251, label %endif252

then251:                                          ; preds = %endif234
  %tokens253 = load i8*, i8** @g_tokens, align 8
  %struct254 = call i8* @__zen_struct_create(i64 4)
  %numstr255 = call i8* @__zen_num_to_str(double 4.400000e+01)
  call void @__zen_struct_set(i8* %struct254, i64 0, i8* %numstr255)
  call void @__zen_struct_set(i8* %struct254, i64 1, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @34, i32 0, i32 0))
  %linea256 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct254, i64 2, i8* %linea256)
  %pos257 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct254, i64 3, i8* %pos257)
  call void @__zen_list_push(i8* %tokens253, i8* %struct254)
  %pos258 = load i8*, i8** %pos, align 8
  %s2d259 = call double @strtod(i8* %pos258, i8** null)
  %add260 = fadd double %s2d259, 2.000000e+00
  %numstr261 = call i8* @__zen_num_to_str(double %add260)
  store i8* %numstr261, i8** %pos, align 8
  br label %while.cond

endif252:                                         ; preds = %endif234
  %c262 = load i8*, i8** %c, align 8
  %s2d263 = call double @strtod(i8* %c262, i8** null)
  %cmp264 = fcmp oeq double %s2d263, 6.000000e+01
  %c2265 = load i8*, i8** %c2, align 8
  %s2d266 = call double @strtod(i8* %c2265, i8** null)
  %cmp267 = fcmp oeq double %s2d266, 6.100000e+01
  %and268 = and i1 %cmp264, %cmp267
  br i1 %and268, label %then269, label %endif270

then269:                                          ; preds = %endif252
  %tokens271 = load i8*, i8** @g_tokens, align 8
  %struct272 = call i8* @__zen_struct_create(i64 4)
  %numstr273 = call i8* @__zen_num_to_str(double 4.700000e+01)
  call void @__zen_struct_set(i8* %struct272, i64 0, i8* %numstr273)
  call void @__zen_struct_set(i8* %struct272, i64 1, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @35, i32 0, i32 0))
  %linea274 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct272, i64 2, i8* %linea274)
  %pos275 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct272, i64 3, i8* %pos275)
  call void @__zen_list_push(i8* %tokens271, i8* %struct272)
  %pos276 = load i8*, i8** %pos, align 8
  %s2d277 = call double @strtod(i8* %pos276, i8** null)
  %add278 = fadd double %s2d277, 2.000000e+00
  %numstr279 = call i8* @__zen_num_to_str(double %add278)
  store i8* %numstr279, i8** %pos, align 8
  br label %while.cond

endif270:                                         ; preds = %endif252
  %c280 = load i8*, i8** %c, align 8
  %s2d281 = call double @strtod(i8* %c280, i8** null)
  %cmp282 = fcmp oeq double %s2d281, 6.200000e+01
  %c2283 = load i8*, i8** %c2, align 8
  %s2d284 = call double @strtod(i8* %c2283, i8** null)
  %cmp285 = fcmp oeq double %s2d284, 6.100000e+01
  %and286 = and i1 %cmp282, %cmp285
  br i1 %and286, label %then287, label %endif288

then287:                                          ; preds = %endif270
  %tokens289 = load i8*, i8** @g_tokens, align 8
  %struct290 = call i8* @__zen_struct_create(i64 4)
  %numstr291 = call i8* @__zen_num_to_str(double 4.800000e+01)
  call void @__zen_struct_set(i8* %struct290, i64 0, i8* %numstr291)
  call void @__zen_struct_set(i8* %struct290, i64 1, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @36, i32 0, i32 0))
  %linea292 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct290, i64 2, i8* %linea292)
  %pos293 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct290, i64 3, i8* %pos293)
  call void @__zen_list_push(i8* %tokens289, i8* %struct290)
  %pos294 = load i8*, i8** %pos, align 8
  %s2d295 = call double @strtod(i8* %pos294, i8** null)
  %add296 = fadd double %s2d295, 2.000000e+00
  %numstr297 = call i8* @__zen_num_to_str(double %add296)
  store i8* %numstr297, i8** %pos, align 8
  br label %while.cond

endif288:                                         ; preds = %endif270
  %c298 = load i8*, i8** %c, align 8
  %s2d299 = call double @strtod(i8* %c298, i8** null)
  %cmp300 = fcmp oeq double %s2d299, 4.500000e+01
  %c2301 = load i8*, i8** %c2, align 8
  %s2d302 = call double @strtod(i8* %c2301, i8** null)
  %cmp303 = fcmp oeq double %s2d302, 6.200000e+01
  %and304 = and i1 %cmp300, %cmp303
  br i1 %and304, label %then305, label %endif306

then305:                                          ; preds = %endif288
  %tokens307 = load i8*, i8** @g_tokens, align 8
  %struct308 = call i8* @__zen_struct_create(i64 4)
  %numstr309 = call i8* @__zen_num_to_str(double 5.600000e+01)
  call void @__zen_struct_set(i8* %struct308, i64 0, i8* %numstr309)
  call void @__zen_struct_set(i8* %struct308, i64 1, i8* getelementptr inbounds ([3 x i8], [3 x i8]* @37, i32 0, i32 0))
  %linea310 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct308, i64 2, i8* %linea310)
  %pos311 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct308, i64 3, i8* %pos311)
  call void @__zen_list_push(i8* %tokens307, i8* %struct308)
  %pos312 = load i8*, i8** %pos, align 8
  %s2d313 = call double @strtod(i8* %pos312, i8** null)
  %add314 = fadd double %s2d313, 2.000000e+00
  %numstr315 = call i8* @__zen_num_to_str(double %add314)
  store i8* %numstr315, i8** %pos, align 8
  br label %while.cond

endif306:                                         ; preds = %endif288
  br label %endif216

then319:                                          ; preds = %endif216
  %tokens322 = load i8*, i8** @g_tokens, align 8
  %struct323 = call i8* @__zen_struct_create(i64 4)
  %numstr324 = call i8* @__zen_num_to_str(double 3.700000e+01)
  call void @__zen_struct_set(i8* %struct323, i64 0, i8* %numstr324)
  call void @__zen_struct_set(i8* %struct323, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @38, i32 0, i32 0))
  %linea325 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct323, i64 2, i8* %linea325)
  %pos326 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct323, i64 3, i8* %pos326)
  call void @__zen_list_push(i8* %tokens322, i8* %struct323)
  br label %endif321

else320:                                          ; preds = %endif216
  %c327 = load i8*, i8** %c, align 8
  %s2d328 = call double @strtod(i8* %c327, i8** null)
  %cmp329 = fcmp oeq double %s2d328, 4.500000e+01
  br i1 %cmp329, label %then330, label %else331

endif321:                                         ; preds = %endif332, %then319
  %pos491 = load i8*, i8** %pos, align 8
  %s2d492 = call double @strtod(i8* %pos491, i8** null)
  %add493 = fadd double %s2d492, 1.000000e+00
  %numstr494 = call i8* @__zen_num_to_str(double %add493)
  store i8* %numstr494, i8** %pos, align 8
  br label %while.cond

then330:                                          ; preds = %else320
  %tokens333 = load i8*, i8** @g_tokens, align 8
  %struct334 = call i8* @__zen_struct_create(i64 4)
  %numstr335 = call i8* @__zen_num_to_str(double 3.800000e+01)
  call void @__zen_struct_set(i8* %struct334, i64 0, i8* %numstr335)
  call void @__zen_struct_set(i8* %struct334, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @39, i32 0, i32 0))
  %linea336 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct334, i64 2, i8* %linea336)
  %pos337 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct334, i64 3, i8* %pos337)
  call void @__zen_list_push(i8* %tokens333, i8* %struct334)
  br label %endif332

else331:                                          ; preds = %else320
  %c338 = load i8*, i8** %c, align 8
  %s2d339 = call double @strtod(i8* %c338, i8** null)
  %cmp340 = fcmp oeq double %s2d339, 4.200000e+01
  br i1 %cmp340, label %then341, label %else342

endif332:                                         ; preds = %endif343, %then330
  br label %endif321

then341:                                          ; preds = %else331
  %tokens344 = load i8*, i8** @g_tokens, align 8
  %struct345 = call i8* @__zen_struct_create(i64 4)
  %numstr346 = call i8* @__zen_num_to_str(double 3.900000e+01)
  call void @__zen_struct_set(i8* %struct345, i64 0, i8* %numstr346)
  call void @__zen_struct_set(i8* %struct345, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @40, i32 0, i32 0))
  %linea347 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct345, i64 2, i8* %linea347)
  %pos348 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct345, i64 3, i8* %pos348)
  call void @__zen_list_push(i8* %tokens344, i8* %struct345)
  br label %endif343

else342:                                          ; preds = %else331
  %c349 = load i8*, i8** %c, align 8
  %s2d350 = call double @strtod(i8* %c349, i8** null)
  %cmp351 = fcmp oeq double %s2d350, 4.700000e+01
  br i1 %cmp351, label %then352, label %else353

endif343:                                         ; preds = %endif354, %then341
  br label %endif332

then352:                                          ; preds = %else342
  %tokens355 = load i8*, i8** @g_tokens, align 8
  %struct356 = call i8* @__zen_struct_create(i64 4)
  %numstr357 = call i8* @__zen_num_to_str(double 4.000000e+01)
  call void @__zen_struct_set(i8* %struct356, i64 0, i8* %numstr357)
  call void @__zen_struct_set(i8* %struct356, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @41, i32 0, i32 0))
  %linea358 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct356, i64 2, i8* %linea358)
  %pos359 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct356, i64 3, i8* %pos359)
  call void @__zen_list_push(i8* %tokens355, i8* %struct356)
  br label %endif354

else353:                                          ; preds = %else342
  %c360 = load i8*, i8** %c, align 8
  %s2d361 = call double @strtod(i8* %c360, i8** null)
  %cmp362 = fcmp oeq double %s2d361, 6.100000e+01
  br i1 %cmp362, label %then363, label %else364

endif354:                                         ; preds = %endif365, %then352
  br label %endif343

then363:                                          ; preds = %else353
  %tokens366 = load i8*, i8** @g_tokens, align 8
  %struct367 = call i8* @__zen_struct_create(i64 4)
  %numstr368 = call i8* @__zen_num_to_str(double 4.900000e+01)
  call void @__zen_struct_set(i8* %struct367, i64 0, i8* %numstr368)
  call void @__zen_struct_set(i8* %struct367, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @42, i32 0, i32 0))
  %linea369 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct367, i64 2, i8* %linea369)
  %pos370 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct367, i64 3, i8* %pos370)
  call void @__zen_list_push(i8* %tokens366, i8* %struct367)
  br label %endif365

else364:                                          ; preds = %else353
  %c371 = load i8*, i8** %c, align 8
  %s2d372 = call double @strtod(i8* %c371, i8** null)
  %cmp373 = fcmp oeq double %s2d372, 6.000000e+01
  br i1 %cmp373, label %then374, label %else375

endif365:                                         ; preds = %endif376, %then363
  br label %endif354

then374:                                          ; preds = %else364
  %tokens377 = load i8*, i8** @g_tokens, align 8
  %struct378 = call i8* @__zen_struct_create(i64 4)
  %numstr379 = call i8* @__zen_num_to_str(double 4.500000e+01)
  call void @__zen_struct_set(i8* %struct378, i64 0, i8* %numstr379)
  call void @__zen_struct_set(i8* %struct378, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @43, i32 0, i32 0))
  %linea380 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct378, i64 2, i8* %linea380)
  %pos381 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct378, i64 3, i8* %pos381)
  call void @__zen_list_push(i8* %tokens377, i8* %struct378)
  br label %endif376

else375:                                          ; preds = %else364
  %c382 = load i8*, i8** %c, align 8
  %s2d383 = call double @strtod(i8* %c382, i8** null)
  %cmp384 = fcmp oeq double %s2d383, 6.200000e+01
  br i1 %cmp384, label %then385, label %else386

endif376:                                         ; preds = %endif387, %then374
  br label %endif365

then385:                                          ; preds = %else375
  %tokens388 = load i8*, i8** @g_tokens, align 8
  %struct389 = call i8* @__zen_struct_create(i64 4)
  %numstr390 = call i8* @__zen_num_to_str(double 4.600000e+01)
  call void @__zen_struct_set(i8* %struct389, i64 0, i8* %numstr390)
  call void @__zen_struct_set(i8* %struct389, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @44, i32 0, i32 0))
  %linea391 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct389, i64 2, i8* %linea391)
  %pos392 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct389, i64 3, i8* %pos392)
  call void @__zen_list_push(i8* %tokens388, i8* %struct389)
  br label %endif387

else386:                                          ; preds = %else375
  %c393 = load i8*, i8** %c, align 8
  %s2d394 = call double @strtod(i8* %c393, i8** null)
  %cmp395 = fcmp oeq double %s2d394, 4.000000e+01
  br i1 %cmp395, label %then396, label %else397

endif387:                                         ; preds = %endif398, %then385
  br label %endif376

then396:                                          ; preds = %else386
  %tokens399 = load i8*, i8** @g_tokens, align 8
  %struct400 = call i8* @__zen_struct_create(i64 4)
  %numstr401 = call i8* @__zen_num_to_str(double 5.700000e+01)
  call void @__zen_struct_set(i8* %struct400, i64 0, i8* %numstr401)
  call void @__zen_struct_set(i8* %struct400, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @45, i32 0, i32 0))
  %linea402 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct400, i64 2, i8* %linea402)
  %pos403 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct400, i64 3, i8* %pos403)
  call void @__zen_list_push(i8* %tokens399, i8* %struct400)
  br label %endif398

else397:                                          ; preds = %else386
  %c404 = load i8*, i8** %c, align 8
  %s2d405 = call double @strtod(i8* %c404, i8** null)
  %cmp406 = fcmp oeq double %s2d405, 4.100000e+01
  br i1 %cmp406, label %then407, label %else408

endif398:                                         ; preds = %endif409, %then396
  br label %endif387

then407:                                          ; preds = %else397
  %tokens410 = load i8*, i8** @g_tokens, align 8
  %struct411 = call i8* @__zen_struct_create(i64 4)
  %numstr412 = call i8* @__zen_num_to_str(double 5.800000e+01)
  call void @__zen_struct_set(i8* %struct411, i64 0, i8* %numstr412)
  call void @__zen_struct_set(i8* %struct411, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @46, i32 0, i32 0))
  %linea413 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct411, i64 2, i8* %linea413)
  %pos414 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct411, i64 3, i8* %pos414)
  call void @__zen_list_push(i8* %tokens410, i8* %struct411)
  br label %endif409

else408:                                          ; preds = %else397
  %c415 = load i8*, i8** %c, align 8
  %s2d416 = call double @strtod(i8* %c415, i8** null)
  %cmp417 = fcmp oeq double %s2d416, 4.400000e+01
  br i1 %cmp417, label %then418, label %else419

endif409:                                         ; preds = %endif420, %then407
  br label %endif398

then418:                                          ; preds = %else408
  %tokens421 = load i8*, i8** @g_tokens, align 8
  %struct422 = call i8* @__zen_struct_create(i64 4)
  %numstr423 = call i8* @__zen_num_to_str(double 6.300000e+01)
  call void @__zen_struct_set(i8* %struct422, i64 0, i8* %numstr423)
  call void @__zen_struct_set(i8* %struct422, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @47, i32 0, i32 0))
  %linea424 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct422, i64 2, i8* %linea424)
  %pos425 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct422, i64 3, i8* %pos425)
  call void @__zen_list_push(i8* %tokens421, i8* %struct422)
  br label %endif420

else419:                                          ; preds = %else408
  %c426 = load i8*, i8** %c, align 8
  %s2d427 = call double @strtod(i8* %c426, i8** null)
  %cmp428 = fcmp oeq double %s2d427, 4.600000e+01
  br i1 %cmp428, label %then429, label %else430

endif420:                                         ; preds = %endif431, %then418
  br label %endif409

then429:                                          ; preds = %else419
  %tokens432 = load i8*, i8** @g_tokens, align 8
  %struct433 = call i8* @__zen_struct_create(i64 4)
  %numstr434 = call i8* @__zen_num_to_str(double 6.400000e+01)
  call void @__zen_struct_set(i8* %struct433, i64 0, i8* %numstr434)
  call void @__zen_struct_set(i8* %struct433, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @48, i32 0, i32 0))
  %linea435 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct433, i64 2, i8* %linea435)
  %pos436 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct433, i64 3, i8* %pos436)
  call void @__zen_list_push(i8* %tokens432, i8* %struct433)
  br label %endif431

else430:                                          ; preds = %else419
  %c437 = load i8*, i8** %c, align 8
  %s2d438 = call double @strtod(i8* %c437, i8** null)
  %cmp439 = fcmp oeq double %s2d438, 5.800000e+01
  br i1 %cmp439, label %then440, label %else441

endif431:                                         ; preds = %endif442, %then429
  br label %endif420

then440:                                          ; preds = %else430
  %tokens443 = load i8*, i8** @g_tokens, align 8
  %struct444 = call i8* @__zen_struct_create(i64 4)
  %numstr445 = call i8* @__zen_num_to_str(double 6.500000e+01)
  call void @__zen_struct_set(i8* %struct444, i64 0, i8* %numstr445)
  call void @__zen_struct_set(i8* %struct444, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @49, i32 0, i32 0))
  %linea446 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct444, i64 2, i8* %linea446)
  %pos447 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct444, i64 3, i8* %pos447)
  call void @__zen_list_push(i8* %tokens443, i8* %struct444)
  br label %endif442

else441:                                          ; preds = %else430
  %c448 = load i8*, i8** %c, align 8
  %s2d449 = call double @strtod(i8* %c448, i8** null)
  %cmp450 = fcmp oeq double %s2d449, 9.100000e+01
  br i1 %cmp450, label %then451, label %else452

endif442:                                         ; preds = %endif453, %then440
  br label %endif431

then451:                                          ; preds = %else441
  %tokens454 = load i8*, i8** @g_tokens, align 8
  %struct455 = call i8* @__zen_struct_create(i64 4)
  %numstr456 = call i8* @__zen_num_to_str(double 5.900000e+01)
  call void @__zen_struct_set(i8* %struct455, i64 0, i8* %numstr456)
  call void @__zen_struct_set(i8* %struct455, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @50, i32 0, i32 0))
  %linea457 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct455, i64 2, i8* %linea457)
  %pos458 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct455, i64 3, i8* %pos458)
  call void @__zen_list_push(i8* %tokens454, i8* %struct455)
  br label %endif453

else452:                                          ; preds = %else441
  %c459 = load i8*, i8** %c, align 8
  %s2d460 = call double @strtod(i8* %c459, i8** null)
  %cmp461 = fcmp oeq double %s2d460, 9.300000e+01
  br i1 %cmp461, label %then462, label %else463

endif453:                                         ; preds = %endif464, %then451
  br label %endif442

then462:                                          ; preds = %else452
  %tokens465 = load i8*, i8** @g_tokens, align 8
  %struct466 = call i8* @__zen_struct_create(i64 4)
  %numstr467 = call i8* @__zen_num_to_str(double 6.000000e+01)
  call void @__zen_struct_set(i8* %struct466, i64 0, i8* %numstr467)
  call void @__zen_struct_set(i8* %struct466, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @51, i32 0, i32 0))
  %linea468 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct466, i64 2, i8* %linea468)
  %pos469 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct466, i64 3, i8* %pos469)
  call void @__zen_list_push(i8* %tokens465, i8* %struct466)
  br label %endif464

else463:                                          ; preds = %else452
  %c470 = load i8*, i8** %c, align 8
  %s2d471 = call double @strtod(i8* %c470, i8** null)
  %cmp472 = fcmp oeq double %s2d471, 1.230000e+02
  br i1 %cmp472, label %then473, label %else474

endif464:                                         ; preds = %endif475, %then462
  br label %endif453

then473:                                          ; preds = %else463
  %tokens476 = load i8*, i8** @g_tokens, align 8
  %struct477 = call i8* @__zen_struct_create(i64 4)
  %numstr478 = call i8* @__zen_num_to_str(double 6.100000e+01)
  call void @__zen_struct_set(i8* %struct477, i64 0, i8* %numstr478)
  call void @__zen_struct_set(i8* %struct477, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @52, i32 0, i32 0))
  %linea479 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct477, i64 2, i8* %linea479)
  %pos480 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct477, i64 3, i8* %pos480)
  call void @__zen_list_push(i8* %tokens476, i8* %struct477)
  br label %endif475

else474:                                          ; preds = %else463
  %c481 = load i8*, i8** %c, align 8
  %s2d482 = call double @strtod(i8* %c481, i8** null)
  %cmp483 = fcmp oeq double %s2d482, 1.250000e+02
  br i1 %cmp483, label %then484, label %endif485

endif475:                                         ; preds = %endif485, %then473
  br label %endif464

then484:                                          ; preds = %else474
  %tokens486 = load i8*, i8** @g_tokens, align 8
  %struct487 = call i8* @__zen_struct_create(i64 4)
  %numstr488 = call i8* @__zen_num_to_str(double 6.200000e+01)
  call void @__zen_struct_set(i8* %struct487, i64 0, i8* %numstr488)
  call void @__zen_struct_set(i8* %struct487, i64 1, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @53, i32 0, i32 0))
  %linea489 = load i8*, i8** %linea, align 8
  call void @__zen_struct_set(i8* %struct487, i64 2, i8* %linea489)
  %pos490 = load i8*, i8** %pos, align 8
  call void @__zen_struct_set(i8* %struct487, i64 3, i8* %pos490)
  call void @__zen_list_push(i8* %tokens486, i8* %struct487)
  br label %endif485

endif485:                                         ; preds = %then484, %else474
  br label %endif475
}

define i32 @main() {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @56, i32 0, i32 0), i8* getelementptr inbounds ([36 x i8], [36 x i8]* @55, i32 0, i32 0))
  store i8* getelementptr inbounds ([11 x i8], [11 x i8]* @57, i32 0, i32 0), i8** @g_codigo, align 8
  %codigo = load i8*, i8** @g_codigo, align 8
  %concat = call i8* @zen_concat(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @58, i32 0, i32 0), i8* %codigo)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @59, i32 0, i32 0), i8* %concat)
  %codigo1 = load i8*, i8** @g_codigo, align 8
  %call = call i8* @tokenizar(i8* %codigo1)
  store i8* %call, i8** @g_tokens, align 8
  %tokens = load i8*, i8** @g_tokens, align 8
  %len = call i64 @strlen(i8* %tokens)
  %lend = uitofp i64 %len to double
  %numstr = call i8* @__zen_num_to_str(double %lend)
  %concat2 = call i8* @zen_concat(i8* getelementptr inbounds ([19 x i8], [19 x i8]* @60, i32 0, i32 0), i8* %numstr)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @61, i32 0, i32 0), i8* %concat2)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @63, i32 0, i32 0), i8* getelementptr inbounds ([26 x i8], [26 x i8]* @62, i32 0, i32 0))
  ret i32 0
}
