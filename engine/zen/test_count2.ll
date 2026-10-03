; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@g_codigo = internal global i8* null
@g_len = internal global i8* null
@g_pos = internal global i8* null
@g_total = internal global i8* null
@g_coms = internal global i8* null
@g_cod = internal global i8* null
@g_blanks = internal global i8* null
@g_linicio = internal global i8* null
@g_es_com = internal global i8* null
@g_tiene_cod = internal global i8* null
@3 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@4 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@5 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@6 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@7 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@8 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@9 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@10 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@11 = private unnamed_addr constant [32 x i8] c"# comment\0Acode line\0A\0A# another\0A\00", align 1
@12 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@13 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@14 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@15 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@16 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@17 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@18 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@19 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@20 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@21 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@22 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@23 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@24 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@25 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@26 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@27 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@28 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@29 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@30 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@31 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@32 = private unnamed_addr constant [4 x i8] c"yes\00", align 1
@33 = private unnamed_addr constant [3 x i8] c"no\00", align 1
@34 = private unnamed_addr constant [8 x i8] c"Total: \00", align 1
@35 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@36 = private unnamed_addr constant [9 x i8] c"Codigo: \00", align 1
@37 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@38 = private unnamed_addr constant [14 x i8] c"Comentarios: \00", align 1
@39 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@40 = private unnamed_addr constant [9 x i8] c"Blanks: \00", align 1
@41 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

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

define i8* @es_contenido(i8* %c) {
entry:
  %c1 = alloca i8*, align 8
  store i8* %c, i8** %c1, align 8
  %c2 = load i8*, i8** %c1, align 8
  %s2d = call double @strtod(i8* %c2, i8** null)
  %cmp = fcmp oeq double %s2d, 1.000000e+01
  br i1 %cmp, label %then, label %endif

then:                                             ; preds = %entry
  ret i8* getelementptr inbounds ([3 x i8], [3 x i8]* @4, i32 0, i32 0)

endif:                                            ; preds = %entry
  %c3 = load i8*, i8** %c1, align 8
  %s2d4 = call double @strtod(i8* %c3, i8** null)
  %cmp5 = fcmp oeq double %s2d4, 3.200000e+01
  br i1 %cmp5, label %then6, label %endif7

then6:                                            ; preds = %endif
  ret i8* getelementptr inbounds ([3 x i8], [3 x i8]* @6, i32 0, i32 0)

endif7:                                           ; preds = %endif
  %c8 = load i8*, i8** %c1, align 8
  %s2d9 = call double @strtod(i8* %c8, i8** null)
  %cmp10 = fcmp oeq double %s2d9, 9.000000e+00
  br i1 %cmp10, label %then11, label %endif12

then11:                                           ; preds = %endif7
  ret i8* getelementptr inbounds ([3 x i8], [3 x i8]* @8, i32 0, i32 0)

endif12:                                          ; preds = %endif7
  ret i8* getelementptr inbounds ([4 x i8], [4 x i8]* @9, i32 0, i32 0)
}

define i32 @main() {
entry:
  %c = alloca i8*, align 8
  store i8* getelementptr inbounds ([32 x i8], [32 x i8]* @11, i32 0, i32 0), i8** @g_codigo, align 8
  %codigo = load i8*, i8** @g_codigo, align 8
  %len = call i64 @strlen(i8* %codigo)
  %lend = uitofp i64 %len to double
  %numstr = call i8* @__zen_num_to_str(double %lend)
  store i8* %numstr, i8** @g_len, align 8
  %numstr1 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr1, i8** @g_pos, align 8
  %numstr2 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr2, i8** @g_total, align 8
  %numstr3 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr3, i8** @g_coms, align 8
  %numstr4 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr4, i8** @g_cod, align 8
  %numstr5 = call i8* @__zen_num_to_str(double 0.000000e+00)
  store i8* %numstr5, i8** @g_blanks, align 8
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @12, i32 0, i32 0), i8** @g_linicio, align 8
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @15, i32 0, i32 0), i8** @g_es_com, align 8
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @17, i32 0, i32 0), i8** @g_tiene_cod, align 8
  br label %while.cond

while.cond:                                       ; preds = %endif23, %entry
  %pos = load i8*, i8** @g_pos, align 8
  %len6 = load i8*, i8** @g_len, align 8
  %s2d = call double @strtod(i8* %pos, i8** null)
  %s2d7 = call double @strtod(i8* %len6, i8** null)
  %cmp = fcmp olt double %s2d, %s2d7
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %codigo8 = load i8*, i8** @g_codigo, align 8
  %pos9 = load i8*, i8** @g_pos, align 8
  %s2d10 = call double @strtod(i8* %pos9, i8** null)
  %idx = fptosi double %s2d10 to i64
  %charptr = getelementptr i8, i8* %codigo8, i64 %idx
  %ch = load i8, i8* %charptr, align 1
  %chd = sitofp i8 %ch to double
  %numstr11 = call i8* @__zen_num_to_str(double %chd)
  store i8* %numstr11, i8** %c, align 8
  %linicio = load i8*, i8** @g_linicio, align 8
  %s2d12 = call double @strtod(i8* %linicio, i8** null)
  %cmp13 = fcmp oeq double %s2d12, 1.000000e+00
  br i1 %cmp13, label %then, label %endif

while.end:                                        ; preds = %while.cond
  %total59 = load i8*, i8** @g_total, align 8
  %concat = call i8* @zen_concat(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @34, i32 0, i32 0), i8* %total59)
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @35, i32 0, i32 0), i8* %concat)
  %cod60 = load i8*, i8** @g_cod, align 8
  %concat61 = call i8* @zen_concat(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @36, i32 0, i32 0), i8* %cod60)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @37, i32 0, i32 0), i8* %concat61)
  %coms62 = load i8*, i8** @g_coms, align 8
  %concat63 = call i8* @zen_concat(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @38, i32 0, i32 0), i8* %coms62)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @39, i32 0, i32 0), i8* %concat63)
  %blanks64 = load i8*, i8** @g_blanks, align 8
  %concat65 = call i8* @zen_concat(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @40, i32 0, i32 0), i8* %blanks64)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @41, i32 0, i32 0), i8* %concat65)
  ret i32 0

then:                                             ; preds = %while.body
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @19, i32 0, i32 0), i8** @g_es_com, align 8
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @21, i32 0, i32 0), i8** @g_tiene_cod, align 8
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @23, i32 0, i32 0), i8** @g_linicio, align 8
  br label %endif

endif:                                            ; preds = %then, %while.body
  %c14 = load i8*, i8** %c, align 8
  %s2d15 = call double @strtod(i8* %c14, i8** null)
  %cmp16 = fcmp oeq double %s2d15, 3.500000e+01
  br i1 %cmp16, label %then17, label %endif18

then17:                                           ; preds = %endif
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @24, i32 0, i32 0), i8** @g_es_com, align 8
  br label %endif18

endif18:                                          ; preds = %then17, %endif
  %c19 = load i8*, i8** %c, align 8
  %s2d20 = call double @strtod(i8* %c19, i8** null)
  %cmp21 = fcmp oeq double %s2d20, 1.000000e+01
  br i1 %cmp21, label %then22, label %else

then22:                                           ; preds = %endif18
  %total = load i8*, i8** @g_total, align 8
  %s2d24 = call double @strtod(i8* %total, i8** null)
  %add = fadd double %s2d24, 1.000000e+00
  %numstr25 = call i8* @__zen_num_to_str(double %add)
  store i8* %numstr25, i8** @g_total, align 8
  %es_com = load i8*, i8** @g_es_com, align 8
  %s2d26 = call double @strtod(i8* %es_com, i8** null)
  %cmp27 = fcmp oeq double %s2d26, 1.000000e+00
  br i1 %cmp27, label %then28, label %else29

else:                                             ; preds = %endif18
  %es_com45 = load i8*, i8** @g_es_com, align 8
  %s2d46 = call double @strtod(i8* %es_com45, i8** null)
  %cmp47 = fcmp oeq double %s2d46, 0.000000e+00
  br i1 %cmp47, label %then48, label %endif49

endif23:                                          ; preds = %endif49, %endif30
  %pos55 = load i8*, i8** @g_pos, align 8
  %s2d56 = call double @strtod(i8* %pos55, i8** null)
  %add57 = fadd double %s2d56, 1.000000e+00
  %numstr58 = call i8* @__zen_num_to_str(double %add57)
  store i8* %numstr58, i8** @g_pos, align 8
  br label %while.cond

then28:                                           ; preds = %then22
  %coms = load i8*, i8** @g_coms, align 8
  %s2d31 = call double @strtod(i8* %coms, i8** null)
  %add32 = fadd double %s2d31, 1.000000e+00
  %numstr33 = call i8* @__zen_num_to_str(double %add32)
  store i8* %numstr33, i8** @g_coms, align 8
  br label %endif30

else29:                                           ; preds = %then22
  %tiene_cod = load i8*, i8** @g_tiene_cod, align 8
  %s2d34 = call double @strtod(i8* %tiene_cod, i8** null)
  %cmp35 = fcmp oeq double %s2d34, 0.000000e+00
  br i1 %cmp35, label %then36, label %else37

endif30:                                          ; preds = %endif38, %then28
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @26, i32 0, i32 0), i8** @g_linicio, align 8
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @29, i32 0, i32 0), i8** @g_es_com, align 8
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @31, i32 0, i32 0), i8** @g_tiene_cod, align 8
  br label %endif23

then36:                                           ; preds = %else29
  %blanks = load i8*, i8** @g_blanks, align 8
  %s2d39 = call double @strtod(i8* %blanks, i8** null)
  %add40 = fadd double %s2d39, 1.000000e+00
  %numstr41 = call i8* @__zen_num_to_str(double %add40)
  store i8* %numstr41, i8** @g_blanks, align 8
  br label %endif38

else37:                                           ; preds = %else29
  %cod = load i8*, i8** @g_cod, align 8
  %s2d42 = call double @strtod(i8* %cod, i8** null)
  %add43 = fadd double %s2d42, 1.000000e+00
  %numstr44 = call i8* @__zen_num_to_str(double %add43)
  store i8* %numstr44, i8** @g_cod, align 8
  br label %endif38

endif38:                                          ; preds = %else37, %then36
  br label %endif30

then48:                                           ; preds = %else
  %c50 = load i8*, i8** %c, align 8
  %call = call i8* @es_contenido(i8* %c50)
  %s2d51 = call double @strtod(i8* %call, i8** null)
  %cmp52 = fcmp oeq double %s2d51, 1.000000e+00
  br i1 %cmp52, label %then53, label %endif54

endif49:                                          ; preds = %endif54, %else
  br label %endif23

then53:                                           ; preds = %then48
  store i8* getelementptr inbounds ([4 x i8], [4 x i8]* @32, i32 0, i32 0), i8** @g_tiene_cod, align 8
  br label %endif54

endif54:                                          ; preds = %then53, %then48
  br label %endif49
}
