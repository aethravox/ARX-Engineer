; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@__zen_argc = internal global i32 0
@__zen_argv = internal global i8** null
@3 = private unnamed_addr constant [5 x i8] c"pos=\00", align 1
@4 = private unnamed_addr constant [4 x i8] c" c=\00", align 1
@5 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@6 = private unnamed_addr constant [7 x i8] c"LETRA\0A\00", align 1
@7 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@8 = private unnamed_addr constant [11 x i8] c"muestra 42\00", align 1
@9 = private unnamed_addr constant [9 x i8] c"tokens: \00", align 1
@10 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

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

define i8* @es_letra(i8* %c) {
entry:
  %c1 = alloca i8*, align 8
  store i8* %c, i8** %c1, align 8
  %c2 = load i8*, i8** %c1, align 8
  %s2d = call double @strtod(i8* %c2, i8** null)
  %cmp = fcmp oge double %s2d, 9.700000e+01
  br i1 %cmp, label %then, label %endif

then:                                             ; preds = %entry
  %c3 = load i8*, i8** %c1, align 8
  %s2d4 = call double @strtod(i8* %c3, i8** null)
  %cmp5 = fcmp ole double %s2d4, 1.220000e+02
  br i1 %cmp5, label %then6, label %endif7

endif:                                            ; preds = %endif7, %entry
  %c8 = load i8*, i8** %c1, align 8
  %s2d9 = call double @strtod(i8* %c8, i8** null)
  %cmp10 = fcmp oge double %s2d9, 6.500000e+01
  br i1 %cmp10, label %then11, label %endif12

then6:                                            ; preds = %then
  %numstr = call i8* @__zen_num_to_str(double 1.000000e+00)
  ret i8* %numstr

endif7:                                           ; preds = %then
  br label %endif

then11:                                           ; preds = %endif
  %c13 = load i8*, i8** %c1, align 8
  %s2d14 = call double @strtod(i8* %c13, i8** null)
  %cmp15 = fcmp ole double %s2d14, 9.000000e+01
  br i1 %cmp15, label %then16, label %endif17

endif12:                                          ; preds = %endif17, %endif
  %c19 = load i8*, i8** %c1, align 8
  %s2d20 = call double @strtod(i8* %c19, i8** null)
  %cmp21 = fcmp oeq double %s2d20, 9.500000e+01
  br i1 %cmp21, label %then22, label %endif23

then16:                                           ; preds = %then11
  %numstr18 = call i8* @__zen_num_to_str(double 1.000000e+00)
  ret i8* %numstr18

endif17:                                          ; preds = %then11
  br label %endif12

then22:                                           ; preds = %endif12
  %numstr24 = call i8* @__zen_num_to_str(double 1.000000e+00)
  ret i8* %numstr24

endif23:                                          ; preds = %endif12
  %numstr25 = call i8* @__zen_num_to_str(double 0.000000e+00)
  ret i8* %numstr25
}

define i8* @tokenizar(i8* %codigo) {
entry:
  %ch36 = alloca double, align 8
  %valor = alloca i8*, align 8
  %c = alloca double, align 8
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
  br label %while.cond

while.cond:                                       ; preds = %endif, %entry
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
  %c_call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @3, i32 0, i32 0))
  %c2d = sitofp i32 %c_call to double
  %pos8 = load double, double* %pos, align 8
  %numstr = call i8* @__zen_num_to_str(double %pos8)
  %c_call9 = call i32 (i8*, ...) @printf(i8* %numstr)
  %c2d10 = sitofp i32 %c_call9 to double
  %c_call11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @4, i32 0, i32 0))
  %c2d12 = sitofp i32 %c_call11 to double
  %c13 = load double, double* %c, align 8
  %numstr14 = call i8* @__zen_num_to_str(double %c13)
  %c_call15 = call i32 (i8*, ...) @printf(i8* %numstr14)
  %c2d16 = sitofp i32 %c_call15 to double
  %c_call17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @5, i32 0, i32 0))
  %c2d18 = sitofp i32 %c_call17 to double
  %c19 = load double, double* %c, align 8
  %numstr20 = call i8* @__zen_num_to_str(double %c19)
  %call = call i8* @es_letra(i8* %numstr20)
  %s2d = call double @strtod(i8* %call, i8** null)
  %cmp21 = fcmp oeq double %s2d, 1.000000e+00
  br i1 %cmp21, label %then, label %else

while.end:                                        ; preds = %while.cond
  %tokens52 = load i8*, i8** %tokens, align 8
  ret i8* %tokens52

then:                                             ; preds = %while.body
  %c_call22 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @6, i32 0, i32 0))
  %c2d23 = sitofp i32 %c_call22 to double
  store i8* getelementptr inbounds ([1 x i8], [1 x i8]* @7, i32 0, i32 0), i8** %valor, align 8
  br label %while.cond24

else:                                             ; preds = %while.body
  %pos50 = load double, double* %pos, align 8
  %add51 = fadd double %pos50, 1.000000e+00
  store double %add51, double* %pos, align 8
  br label %endif

endif:                                            ; preds = %else, %while.end26
  br label %while.cond

while.cond24:                                     ; preds = %endif44, %then
  %pos27 = load double, double* %pos, align 8
  %len28 = load double, double* %len3, align 8
  %cmp29 = fcmp olt double %pos27, %len28
  br i1 %cmp29, label %while.body25, label %while.end26

while.body25:                                     ; preds = %while.cond24
  %codigo30 = load i8*, i8** %codigo1, align 8
  %pos31 = load double, double* %pos, align 8
  %idx32 = fptosi double %pos31 to i64
  %charptr33 = getelementptr i8, i8* %codigo30, i64 %idx32
  %ch34 = load i8, i8* %charptr33, align 1
  %chd35 = sitofp i8 %ch34 to double
  store double %chd35, double* %ch36, align 8
  %ch37 = load double, double* %ch36, align 8
  %numstr38 = call i8* @__zen_num_to_str(double %ch37)
  %call39 = call i8* @es_letra(i8* %numstr38)
  %s2d40 = call double @strtod(i8* %call39, i8** null)
  %cmp41 = fcmp oeq double %s2d40, 1.000000e+00
  br i1 %cmp41, label %then42, label %else43

while.end26:                                      ; preds = %else43, %while.cond24
  %tokens48 = load i8*, i8** %tokens, align 8
  %valor49 = load i8*, i8** %valor, align 8
  call void @__zen_list_push(i8* %tokens48, i8* %valor49)
  br label %endif

then42:                                           ; preds = %while.body25
  %valor45 = load i8*, i8** %valor, align 8
  %ch46 = load double, double* %ch36, align 8
  %chr = fptosi double %ch46 to i8
  %chrbuf = call i8* @malloc(i64 2)
  store i8 %chr, i8* %chrbuf, align 1
  %nullptr = getelementptr i8, i8* %chrbuf, i64 1
  store i8 0, i8* %nullptr, align 1
  %concat = call i8* @zen_concat(i8* %valor45, i8* %chrbuf)
  store i8* %concat, i8** %valor, align 8
  %pos47 = load double, double* %pos, align 8
  %add = fadd double %pos47, 1.000000e+00
  store double %add, double* %pos, align 8
  br label %endif44

else43:                                           ; preds = %while.body25
  br label %while.end26

endif44:                                          ; preds = %then42
  br label %while.cond24
}

define i8* @main() {
entry:
  %n = alloca double, align 8
  %tokens = alloca i8*, align 8
  %codigo = alloca i8*, align 8
  store i8* getelementptr inbounds ([11 x i8], [11 x i8]* @8, i32 0, i32 0), i8** %codigo, align 8
  %codigo1 = load i8*, i8** %codigo, align 8
  %call = call i8* @tokenizar(i8* %codigo1)
  store i8* %call, i8** %tokens, align 8
  %tokens2 = load i8*, i8** %tokens, align 8
  %len = call i64 @strlen(i8* %tokens2)
  %lend = uitofp i64 %len to double
  store double %lend, double* %n, align 8
  %c_call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @9, i32 0, i32 0))
  %c2d = sitofp i32 %c_call to double
  %n3 = load double, double* %n, align 8
  %numstr = call i8* @__zen_num_to_str(double %n3)
  %c_call4 = call i32 (i8*, ...) @printf(i8* %numstr)
  %c2d5 = sitofp i32 %c_call4 to double
  %c_call6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([2 x i8], [2 x i8]* @10, i32 0, i32 0))
  %c2d7 = sitofp i32 %c_call6 to double
  ret i8* null
}

define i32 @main.1(i32 %argc, i8** %argv) {
entry:
  store i32 %argc, i32* @__zen_argc, align 4
  store i8** %argv, i8*** @__zen_argv, align 8
  %call = call i8* @main()
  ret i32 0
}
