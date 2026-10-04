; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@__zen_argc = internal global i32 0
@__zen_argv = internal global i8** null
@g_raiz = internal global i8* null
@3 = private unnamed_addr constant [26 x i8] c"=== Funciones de libm ===\00", align 1
@4 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@5 = private unnamed_addr constant [12 x i8] c"sqrt(16) = \00", align 1
@6 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@7 = private unnamed_addr constant [11 x i8] c"sqrt(2) = \00", align 1
@8 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@9 = private unnamed_addr constant [10 x i8] c"sin(0) = \00", align 1
@10 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@11 = private unnamed_addr constant [10 x i8] c"cos(0) = \00", align 1
@12 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@13 = private unnamed_addr constant [14 x i8] c"pow(2, 10) = \00", align 1
@14 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@15 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@16 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@17 = private unnamed_addr constant [26 x i8] c"=== Funciones de libc ===\00", align 1
@18 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@19 = private unnamed_addr constant [12 x i8] c"abs(-42) = \00", align 1
@20 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@21 = private unnamed_addr constant [11 x i8] c"abs(42) = \00", align 1
@22 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@23 = private unnamed_addr constant [24 x i8] c"strlen(\22hola mundo\22) = \00", align 1
@24 = private unnamed_addr constant [11 x i8] c"hola mundo\00", align 1
@25 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@26 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@27 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@28 = private unnamed_addr constant [18 x i8] c"=== Combinado ===\00", align 1
@29 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@30 = private unnamed_addr constant [13 x i8] c"sqrt(144) = \00", align 1
@31 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@32 = private unnamed_addr constant [6 x i8] c"sqrt(\00", align 1
@33 = private unnamed_addr constant [5 x i8] c") = \00", align 1
@34 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

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

declare double @sqrt(double)

declare double @sin(double)

declare double @cos(double)

declare i32 @abs(i32)

define i32 @main(i32 %argc, i8** %argv) {
entry:
  store i32 %argc, i32* @__zen_argc, align 4
  store i8** %argv, i8*** @__zen_argv, align 8
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @4, i32 0, i32 0), i8* getelementptr inbounds ([26 x i8], [26 x i8]* @3, i32 0, i32 0))
  %c_call = call double @sqrt(double 1.600000e+01)
  %numstr = call i8* @__zen_num_to_str(double %c_call)
  %concat = call i8* @zen_concat(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @5, i32 0, i32 0), i8* %numstr)
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @6, i32 0, i32 0), i8* %concat)
  %c_call1 = call double @sqrt(double 2.000000e+00)
  %numstr2 = call i8* @__zen_num_to_str(double %c_call1)
  %concat3 = call i8* @zen_concat(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @7, i32 0, i32 0), i8* %numstr2)
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @8, i32 0, i32 0), i8* %concat3)
  %c_call4 = call double @sin(double 0.000000e+00)
  %numstr5 = call i8* @__zen_num_to_str(double %c_call4)
  %concat6 = call i8* @zen_concat(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @9, i32 0, i32 0), i8* %numstr5)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @10, i32 0, i32 0), i8* %concat6)
  %c_call7 = call double @cos(double 0.000000e+00)
  %numstr8 = call i8* @__zen_num_to_str(double %c_call7)
  %concat9 = call i8* @zen_concat(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @11, i32 0, i32 0), i8* %numstr8)
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @12, i32 0, i32 0), i8* %concat9)
  %c_call10 = call double @pow(double 2.000000e+00, double 1.000000e+01)
  %numstr11 = call i8* @__zen_num_to_str(double %c_call10)
  %concat12 = call i8* @zen_concat(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @13, i32 0, i32 0), i8* %numstr11)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @14, i32 0, i32 0), i8* %concat12)
  %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @16, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @15, i32 0, i32 0))
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @18, i32 0, i32 0), i8* getelementptr inbounds ([26 x i8], [26 x i8]* @17, i32 0, i32 0))
  %c_call13 = call i32 @abs(i32 -42)
  %c2d = sitofp i32 %c_call13 to double
  %numstr14 = call i8* @__zen_num_to_str(double %c2d)
  %concat15 = call i8* @zen_concat(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @19, i32 0, i32 0), i8* %numstr14)
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @20, i32 0, i32 0), i8* %concat15)
  %c_call16 = call i32 @abs(i32 42)
  %c2d17 = sitofp i32 %c_call16 to double
  %numstr18 = call i8* @__zen_num_to_str(double %c2d17)
  %concat19 = call i8* @zen_concat(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @21, i32 0, i32 0), i8* %numstr18)
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @22, i32 0, i32 0), i8* %concat19)
  %c_call20 = call i64 @strlen(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @24, i32 0, i32 0))
  %c2d21 = sitofp i64 %c_call20 to double
  %numstr22 = call i8* @__zen_num_to_str(double %c2d21)
  %concat23 = call i8* @zen_concat(i8* getelementptr inbounds ([24 x i8], [24 x i8]* @23, i32 0, i32 0), i8* %numstr22)
  %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @25, i32 0, i32 0), i8* %concat23)
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @27, i32 0, i32 0), i8* getelementptr inbounds ([1 x i8], [1 x i8]* @26, i32 0, i32 0))
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @29, i32 0, i32 0), i8* getelementptr inbounds ([18 x i8], [18 x i8]* @28, i32 0, i32 0))
  %c_call24 = call double @sqrt(double 1.440000e+02)
  %numstr25 = call i8* @__zen_num_to_str(double %c_call24)
  store i8* %numstr25, i8** @g_raiz, align 8
  %raiz = load i8*, i8** @g_raiz, align 8
  %concat26 = call i8* @zen_concat(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @30, i32 0, i32 0), i8* %raiz)
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @31, i32 0, i32 0), i8* %concat26)
  %i = alloca double, align 8
  store double 1.000000e+00, double* %i, align 8
  br label %for.cond

for.cond:                                         ; preds = %for.inc, %entry
  %i27 = load double, double* %i, align 8
  %cmp = fcmp olt double %i27, 5.000000e+00
  br i1 %cmp, label %for.body, label %for.end

for.body:                                         ; preds = %for.cond
  %i28 = load double, double* %i, align 8
  %numstr29 = call i8* @__zen_num_to_str(double %i28)
  %concat30 = call i8* @zen_concat(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @32, i32 0, i32 0), i8* %numstr29)
  %concat31 = call i8* @zen_concat(i8* %concat30, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @33, i32 0, i32 0))
  %i32 = load double, double* %i, align 8
  %c_call33 = call double @sqrt(double %i32)
  %numstr34 = call i8* @__zen_num_to_str(double %c_call33)
  %concat35 = call i8* @zen_concat(i8* %concat31, i8* %numstr34)
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @34, i32 0, i32 0), i8* %concat35)
  br label %for.inc

for.inc:                                          ; preds = %for.body
  %i36 = load double, double* %i, align 8
  %next = fadd double %i36, 1.000000e+00
  store double %next, double* %i, align 8
  br label %for.cond

for.end:                                          ; preds = %for.cond
  ret i32 0
}
