; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@__zen_argc = internal global i32 0
@__zen_argv = internal global i8** null
@3 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@5 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@7 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@9 = private unnamed_addr constant [3 x i8] c"si\00", align 1
@10 = private unnamed_addr constant [3 x i8] c"if\00", align 1
@11 = private unnamed_addr constant [8 x i8] c"muestra\00", align 1
@12 = private unnamed_addr constant [5 x i8] c"show\00", align 1
@13 = private unnamed_addr constant [6 x i8] c"print\00", align 1
@14 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@15 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@16 = private unnamed_addr constant [4 x i8] c"EOF\00", align 1
@17 = private unnamed_addr constant [14 x i8] c"/tmp/hola.txt\00", align 1
@18 = private unnamed_addr constant [3 x i8] c"rb\00", align 1
@19 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@20 = private unnamed_addr constant [9 x i8] c"tokens: \00", align 1
@21 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

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
  %strcmp7 = call i32 @strcmp(i8* %palabra6, i8* getelementptr inbounds ([8 x i8], [8 x i8]* @11, i32 0, i32 0))
  %scmp8 = icmp eq i32 %strcmp7, 0
  %palabra9 = load i8*, i8** %palabra1, align 8
  %strcmp10 = call i32 @strcmp(i8* %palabra9, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @12, i32 0, i32 0))
  %scmp11 = icmp eq i32 %strcmp10, 0
  %or12 = or i1 %scmp8, %scmp11
  %palabra13 = load i8*, i8** %palabra1, align 8
  %strcmp14 = call i32 @strcmp(i8* %palabra13, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @13, i32 0, i32 0))
  %scmp15 = icmp eq i32 %strcmp14, 0
  %or16 = or i1 %or12, %scmp15
  br i1 %or16, label %then17, label %endif18

then17:                                           ; preds = %endif
  %numstr19 = call i8* @__zen_num_to_str(double 1.900000e+01)
  ret i8* %numstr19

endif18:                                          ; preds = %endif
  %numstr20 = call i8* @__zen_num_to_str(double 3.000000e+00)
  ret i8* %numstr20
}

define i8* @tokenizar(i8* %codigo) {
entry:
  %tipo = alloca i8*, align 8
  %t = alloca i8*, align 8
  %ch55 = alloca double, align 8
  %valor = alloca i8*, align 8
  %c = alloca double, align 8
  %linea = alloca double, align 8
  %pos = alloca double, align 8
  %len3 = alloca double, align 8
  %tokens = alloca i8*, align 8
  %codigo1 = alloca i8*, align 8
  store i8* %codigo, i8** %codigo1, align 8
  %list = call i8* @__zen_list_create(i64 4)
  store i8* %list, i8** %tokens, align 8
  %codigo2 = load i8*, i8** %codigo1, align 8
  %len = call i64 @strlen(i8* %codigo2)
  %lend = uitofp i64 %len to double
  store double %lend, double* %len3, align 8
  store double 0.000000e+00, double* %pos, align 8
  store double 1.000000e+00, double* %linea, align 8
  br label %while.cond

while.cond:                                       ; preds = %endif82, %while.end85, %while.end45, %while.end27, %then15, %then, %entry
  %pos4 = load double, double* %pos, align 8
  %len5 = load double, double* %len3, align 8
  %cmp = fcmp olt double %pos4, %len5
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %codigo6 = load i8*, i8** %codigo1, align 8
  %pos7 = load double, double* %pos, align 8
  %idx = fptosi double %pos7 to i64
  %charptr = getelementptr i8, i8* %codigo6, i64 %idx
  %ch = load i8, i8* %charptr, align 1
  %chd = sitofp i8 %ch to double
  store double %chd, double* %c, align 8
  %c8 = load double, double* %c, align 8
  %cmp9 = fcmp oeq double %c8, 3.200000e+01
  %c10 = load double, double* %c, align 8
  %cmp11 = fcmp oeq double %c10, 9.000000e+00
  %or = or i1 %cmp9, %cmp11
  br i1 %or, label %then, label %endif

while.end:                                        ; preds = %while.cond
  %struct123 = call i8* @__zen_struct_create(i64 4)
  %numstr124 = call i8* @__zen_num_to_str(double 0.000000e+00)
  call void @__zen_struct_set(i8* %struct123, i64 0, i8* %numstr124)
  call void @__zen_struct_set(i8* %struct123, i64 1, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @16, i32 0, i32 0))
  %linea125 = load double, double* %linea, align 8
  %numstr126 = call i8* @__zen_num_to_str(double %linea125)
  call void @__zen_struct_set(i8* %struct123, i64 2, i8* %numstr126)
  %pos127 = load double, double* %pos, align 8
  %numstr128 = call i8* @__zen_num_to_str(double %pos127)
  call void @__zen_struct_set(i8* %struct123, i64 3, i8* %numstr128)
  store i8* %struct123, i8** %t, align 8
  %tokens129 = load i8*, i8** %tokens, align 8
  %t130 = load i8*, i8** %t, align 8
  call void @__zen_list_push(i8* %tokens129, i8* %t130)
  %tokens131 = load i8*, i8** %tokens, align 8
  ret i8* %tokens131

then:                                             ; preds = %while.body
  %pos12 = load double, double* %pos, align 8
  %add = fadd double %pos12, 1.000000e+00
  store double %add, double* %pos, align 8
  br label %while.cond

endif:                                            ; preds = %while.body
  %c13 = load double, double* %c, align 8
  %cmp14 = fcmp oeq double %c13, 1.000000e+01
  br i1 %cmp14, label %then15, label %endif16

then15:                                           ; preds = %endif
  %pos17 = load double, double* %pos, align 8
  %add18 = fadd double %pos17, 1.000000e+00
  store double %add18, double* %pos, align 8
  %linea19 = load double, double* %linea, align 8
  %add20 = fadd double %linea19, 1.000000e+00
  store double %add20, double* %linea, align 8
  br label %while.cond

endif16:                                          ; preds = %endif
  %c21 = load double, double* %c, align 8
  %cmp22 = fcmp oeq double %c21, 3.500000e+01
  br i1 %cmp22, label %then23, label %endif24

then23:                                           ; preds = %endif16
  br label %while.cond25

endif24:                                          ; preds = %endif16
  %c40 = load double, double* %c, align 8
  %numstr = call i8* @__zen_num_to_str(double %c40)
  %call = call i8* @es_digito(i8* %numstr)
  %tobool = icmp ne i8* %call, null
  br i1 %tobool, label %then41, label %endif42

while.cond25:                                     ; preds = %while.body26, %then23
  %pos28 = load double, double* %pos, align 8
  %len29 = load double, double* %len3, align 8
  %cmp30 = fcmp olt double %pos28, %len29
  %codigo31 = load i8*, i8** %codigo1, align 8
  %pos32 = load double, double* %pos, align 8
  %idx33 = fptosi double %pos32 to i64
  %charptr34 = getelementptr i8, i8* %codigo31, i64 %idx33
  %ch35 = load i8, i8* %charptr34, align 1
  %chd36 = sitofp i8 %ch35 to double
  %cmp37 = fcmp one double %chd36, 1.000000e+01
  %and = and i1 %cmp30, %cmp37
  br i1 %and, label %while.body26, label %while.end27

while.body26:                                     ; preds = %while.cond25
  %pos38 = load double, double* %pos, align 8
  %add39 = fadd double %pos38, 1.000000e+00
  store double %add39, double* %pos, align 8
  br label %while.cond25

while.end27:                                      ; preds = %while.cond25
  br label %while.cond

then41:                                           ; preds = %endif24
  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @14, i32 0, i32 0), i8** %valor, align 8
  br label %while.cond43

endif42:                                          ; preds = %endif24
  %c77 = load double, double* %c, align 8
  %numstr78 = call i8* @__zen_num_to_str(double %c77)
  %call79 = call i8* @es_letra(i8* %numstr78)
  %tobool80 = icmp ne i8* %call79, null
  br i1 %tobool80, label %then81, label %endif82

while.cond43:                                     ; preds = %endif64, %then41
  %pos46 = load double, double* %pos, align 8
  %len47 = load double, double* %len3, align 8
  %cmp48 = fcmp olt double %pos46, %len47
  br i1 %cmp48, label %while.body44, label %while.end45

while.body44:                                     ; preds = %while.cond43
  %codigo49 = load i8*, i8** %codigo1, align 8
  %pos50 = load double, double* %pos, align 8
  %idx51 = fptosi double %pos50 to i64
  %charptr52 = getelementptr i8, i8* %codigo49, i64 %idx51
  %ch53 = load i8, i8* %charptr52, align 1
  %chd54 = sitofp i8 %ch53 to double
  store double %chd54, double* %ch55, align 8
  %ch56 = load double, double* %ch55, align 8
  %numstr57 = call i8* @__zen_num_to_str(double %ch56)
  %call58 = call i8* @es_digito(i8* %numstr57)
  %ch59 = load double, double* %ch55, align 8
  %cmp60 = fcmp oeq double %ch59, 4.600000e+01
  %tobool61 = icmp ne i8* %call58, null
  %or62 = or i1 %tobool61, %cmp60
  br i1 %or62, label %then63, label %else

while.end45:                                      ; preds = %else, %while.cond43
  %struct = call i8* @__zen_struct_create(i64 4)
  %numstr69 = call i8* @__zen_num_to_str(double 1.000000e+00)
  call void @__zen_struct_set(i8* %struct, i64 0, i8* %numstr69)
  %valor70 = load i8*, i8** %valor, align 8
  call void @__zen_struct_set(i8* %struct, i64 1, i8* %valor70)
  %linea71 = load double, double* %linea, align 8
  %numstr72 = call i8* @__zen_num_to_str(double %linea71)
  call void @__zen_struct_set(i8* %struct, i64 2, i8* %numstr72)
  %pos73 = load double, double* %pos, align 8
  %numstr74 = call i8* @__zen_num_to_str(double %pos73)
  call void @__zen_struct_set(i8* %struct, i64 3, i8* %numstr74)
  store i8* %struct, i8** %t, align 8
  %tokens75 = load i8*, i8** %tokens, align 8
  %t76 = load i8*, i8** %t, align 8
  call void @__zen_list_push(i8* %tokens75, i8* %t76)
  br label %while.cond

then63:                                           ; preds = %while.body44
  %valor65 = load i8*, i8** %valor, align 8
  %ch66 = load double, double* %ch55, align 8
  %chr = fptosi double %ch66 to i8
  %chrbuf = call i8* @malloc(i64 2)
  store i8 %chr, i8* %chrbuf, align 1
  %nullptr = getelementptr i8, i8* %chrbuf, i64 1
  store i8 0, i8* %nullptr, align 1
  %concat = call i8* @zen_concat(i8* %valor65, i8* %chrbuf)
  store i8* %concat, i8** %valor, align 8
  %pos67 = load double, double* %pos, align 8
  %add68 = fadd double %pos67, 1.000000e+00
  store double %add68, double* %pos, align 8
  br label %endif64

else:                                             ; preds = %while.body44
  br label %while.end45

endif64:                                          ; preds = %then63
  br label %while.cond43

then81:                                           ; preds = %endif42
  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @15, i32 0, i32 0), i8** %valor, align 8
  br label %while.cond83

endif82:                                          ; preds = %endif42
  %pos121 = load double, double* %pos, align 8
  %add122 = fadd double %pos121, 1.000000e+00
  store double %add122, double* %pos, align 8
  br label %while.cond

while.cond83:                                     ; preds = %endif101, %then81
  %pos86 = load double, double* %pos, align 8
  %len87 = load double, double* %len3, align 8
  %cmp88 = fcmp olt double %pos86, %len87
  br i1 %cmp88, label %while.body84, label %while.end85

while.body84:                                     ; preds = %while.cond83
  %codigo89 = load i8*, i8** %codigo1, align 8
  %pos90 = load double, double* %pos, align 8
  %idx91 = fptosi double %pos90 to i64
  %charptr92 = getelementptr i8, i8* %codigo89, i64 %idx91
  %ch93 = load i8, i8* %charptr92, align 1
  %chd94 = sitofp i8 %ch93 to double
  store double %chd94, double* %ch55, align 8
  %ch95 = load double, double* %ch55, align 8
  %numstr96 = call i8* @__zen_num_to_str(double %ch95)
  %call97 = call i8* @es_alfa_num(i8* %numstr96)
  %tobool98 = icmp ne i8* %call97, null
  br i1 %tobool98, label %then99, label %else100

while.end85:                                      ; preds = %else100, %while.cond83
  %valor110 = load i8*, i8** %valor, align 8
  %call111 = call i8* @tipo_keyword(i8* %valor110)
  store i8* %call111, i8** %tipo, align 8
  %struct112 = call i8* @__zen_struct_create(i64 4)
  %tipo113 = load i8*, i8** %tipo, align 8
  call void @__zen_struct_set(i8* %struct112, i64 0, i8* %tipo113)
  %valor114 = load i8*, i8** %valor, align 8
  call void @__zen_struct_set(i8* %struct112, i64 1, i8* %valor114)
  %linea115 = load double, double* %linea, align 8
  %numstr116 = call i8* @__zen_num_to_str(double %linea115)
  call void @__zen_struct_set(i8* %struct112, i64 2, i8* %numstr116)
  %pos117 = load double, double* %pos, align 8
  %numstr118 = call i8* @__zen_num_to_str(double %pos117)
  call void @__zen_struct_set(i8* %struct112, i64 3, i8* %numstr118)
  store i8* %struct112, i8** %t, align 8
  %tokens119 = load i8*, i8** %tokens, align 8
  %t120 = load i8*, i8** %t, align 8
  call void @__zen_list_push(i8* %tokens119, i8* %t120)
  br label %while.cond

then99:                                           ; preds = %while.body84
  %valor102 = load i8*, i8** %valor, align 8
  %ch103 = load double, double* %ch55, align 8
  %chr104 = fptosi double %ch103 to i8
  %chrbuf105 = call i8* @malloc(i64 2)
  store i8 %chr104, i8* %chrbuf105, align 1
  %nullptr106 = getelementptr i8, i8* %chrbuf105, i64 1
  store i8 0, i8* %nullptr106, align 1
  %concat107 = call i8* @zen_concat(i8* %valor102, i8* %chrbuf105)
  store i8* %concat107, i8** %valor, align 8
  %pos108 = load double, double* %pos, align 8
  %add109 = fadd double %pos108, 1.000000e+00
  store double %add109, double* %pos, align 8
  br label %endif101

else100:                                          ; preds = %while.body84
  br label %while.end85

endif101:                                         ; preds = %then99
  br label %while.cond83
}

define i8* @principal() {
entry:
  %n = alloca double, align 8
  %tokens = alloca i8*, align 8
  %codigo = alloca i8*, align 8
  %file = call i8* @fopen(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @17, i32 0, i32 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @18, i32 0, i32 0))
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
  %rf_result = phi i8* [ getelementptr inbounds ([1 x i8], [1 x i8]* @19, i32 0, i32 0), %entry ], [ %buf, %rf.open ]
  store i8* %rf_result, i8** %codigo, align 8
  %codigo1 = load i8*, i8** %codigo, align 8
  %call = call i8* @tokenizar(i8* %codigo1)
  store i8* %call, i8** %tokens, align 8
  %tokens2 = load i8*, i8** %tokens, align 8
  %c_call = call i64 @__zen_list_len(i8* %tokens2)
  %c2d = sitofp i64 %c_call to double
  store double %c2d, double* %n, align 8
  %c_call3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @20, i32 0, i32 0))
  %c2d4 = sitofp i32 %c_call3 to double
  %n5 = load double, double* %n, align 8
  %numstr = call i8* @__zen_num_to_str(double %n5)
  %c_call6 = call i32 (i8*, ...) @printf(i8* %numstr)
  %c2d7 = sitofp i32 %c_call6 to double
  %c_call8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @21, i32 0, i32 0))
  %c2d9 = sitofp i32 %c_call8 to double
  ret i8* null
}

define i32 @main(i32 %argc, i8** %argv) {
entry:
  store i32 %argc, i32* @__zen_argc, align 4
  store i8** %argv, i8*** @__zen_argv, align 8
  %call = call i8* @principal()
  ret i32 0
}
