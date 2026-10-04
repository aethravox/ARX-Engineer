; ModuleID = 'zen'
source_filename = "zen"

@0 = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@1 = private unnamed_addr constant [3 x i8] c"%g\00", align 1
@empty = private unnamed_addr constant [1 x i8] zeroinitializer, align 1
@2 = private unnamed_addr constant [6 x i8] c"linux\00", align 1
@__zen_argc = internal global i32 0
@__zen_argv = internal global i8** null
@g_saludo = internal global i8* null
@g_s = internal global i8* null
@g_r = internal global i8* null
@g_espacios = internal global i8* null
@g_n = internal global i8* null
@g_t = internal global i8* null
@g_frase = internal global i8* null
@g_palabras_idx = internal global i8* null
@g_nueva_frase = internal global i8* null
@3 = private unnamed_addr constant [5 x i8] c"hola\00", align 1
@4 = private unnamed_addr constant [5 x i8] c"hola\00", align 1
@5 = private unnamed_addr constant [15 x i8] c"Saludaste bien\00", align 1
@6 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@7 = private unnamed_addr constant [13 x i8] c"No saludaste\00", align 1
@8 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@9 = private unnamed_addr constant [5 x i8] c"chau\00", align 1
@10 = private unnamed_addr constant [11 x i8] c"No es chau\00", align 1
@11 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@12 = private unnamed_addr constant [11 x i8] c"hola mundo\00", align 1
@13 = private unnamed_addr constant [14 x i8] c"Longitud de '\00", align 1
@14 = private unnamed_addr constant [4 x i8] c"': \00", align 1
@15 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@16 = private unnamed_addr constant [12 x i8] c"Sub [0:4]: \00", align 1
@17 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@18 = private unnamed_addr constant [13 x i8] c"Sub [5:10]: \00", align 1
@19 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@20 = private unnamed_addr constant [17 x i8] c"Buscar 'mundo': \00", align 1
@21 = private unnamed_addr constant [6 x i8] c"mundo\00", align 1
@22 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@23 = private unnamed_addr constant [15 x i8] c"Buscar 'xyz': \00", align 1
@24 = private unnamed_addr constant [4 x i8] c"xyz\00", align 1
@25 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@26 = private unnamed_addr constant [6 x i8] c"mundo\00", align 1
@27 = private unnamed_addr constant [4 x i8] c"zen\00", align 1
@28 = private unnamed_addr constant [12 x i8] c"Reemplazo: \00", align 1
@29 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@30 = private unnamed_addr constant [8 x i8] c"Mayus: \00", align 1
@31 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@32 = private unnamed_addr constant [8 x i8] c"Minus: \00", align 1
@33 = private unnamed_addr constant [11 x i8] c"HOLA Mundo\00", align 1
@34 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@35 = private unnamed_addr constant [11 x i8] c"   hola   \00", align 1
@36 = private unnamed_addr constant [13 x i8] c"Recortado: '\00", align 1
@37 = private unnamed_addr constant [2 x i8] c"'\00", align 1
@38 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@39 = private unnamed_addr constant [5 x i8] c"42.5\00", align 1
@40 = private unnamed_addr constant [13 x i8] c"Numero + 1: \00", align 1
@41 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@42 = private unnamed_addr constant [8 x i8] c"Texto: \00", align 1
@43 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@44 = private unnamed_addr constant [16 x i8] c"El gato es gris\00", align 1
@45 = private unnamed_addr constant [5 x i8] c"gato\00", align 1
@46 = private unnamed_addr constant [24 x i8] c"Palabra encontrada en: \00", align 1
@47 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@48 = private unnamed_addr constant [5 x i8] c"gato\00", align 1
@49 = private unnamed_addr constant [6 x i8] c"perro\00", align 1
@50 = private unnamed_addr constant [14 x i8] c"Nueva frase: \00", align 1
@51 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@52 = private unnamed_addr constant [8 x i8] c"Mayus: \00", align 1
@53 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

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

define i32 @main(i32 %argc, i8** %argv) {
entry:
  store i32 %argc, i32* @__zen_argc, align 4
  store i8** %argv, i8*** @__zen_argv, align 8
  store i8* getelementptr inbounds ([5 x i8], [5 x i8]* @3, i32 0, i32 0), i8** @g_saludo, align 8
  %saludo = load i8*, i8** @g_saludo, align 8
  %strcmp = call i32 @strcmp(i8* %saludo, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @4, i32 0, i32 0))
  %scmp = icmp eq i32 %strcmp, 0
  br i1 %scmp, label %then, label %else

then:                                             ; preds = %entry
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @6, i32 0, i32 0), i8* getelementptr inbounds ([15 x i8], [15 x i8]* @5, i32 0, i32 0))
  br label %endif

else:                                             ; preds = %entry
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @8, i32 0, i32 0), i8* getelementptr inbounds ([13 x i8], [13 x i8]* @7, i32 0, i32 0))
  br label %endif

endif:                                            ; preds = %else, %then
  %saludo1 = load i8*, i8** @g_saludo, align 8
  %strcmp2 = call i32 @strcmp(i8* %saludo1, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @9, i32 0, i32 0))
  %scmp3 = icmp ne i32 %strcmp2, 0
  br i1 %scmp3, label %then4, label %endif5

then4:                                            ; preds = %endif
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @11, i32 0, i32 0), i8* getelementptr inbounds ([11 x i8], [11 x i8]* @10, i32 0, i32 0))
  br label %endif5

endif5:                                           ; preds = %then4, %endif
  store i8* getelementptr inbounds ([11 x i8], [11 x i8]* @12, i32 0, i32 0), i8** @g_s, align 8
  %s = load i8*, i8** @g_s, align 8
  %concat = call i8* @zen_concat(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @13, i32 0, i32 0), i8* %s)
  %concat6 = call i8* @zen_concat(i8* %concat, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @14, i32 0, i32 0))
  %s7 = load i8*, i8** @g_s, align 8
  %len = call i64 @strlen(i8* %s7)
  %lend = uitofp i64 %len to double
  %numstr = call i8* @__zen_num_to_str(double %lend)
  %concat8 = call i8* @zen_concat(i8* %concat6, i8* %numstr)
  %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @15, i32 0, i32 0), i8* %concat8)
  %s9 = load i8*, i8** @g_s, align 8
  %buf = call i8* @malloc(i64 5)
  %src = getelementptr inbounds i8, i8* %s9, i64 0
  %4 = call i8* @strncpy(i8* %buf, i8* %src, i64 4)
  %npos = getelementptr inbounds i8, i8* %buf, i64 4
  store i8 0, i8* %npos, align 1
  %concat10 = call i8* @zen_concat(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @16, i32 0, i32 0), i8* %buf)
  %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @17, i32 0, i32 0), i8* %concat10)
  %s11 = load i8*, i8** @g_s, align 8
  %buf12 = call i8* @malloc(i64 6)
  %src13 = getelementptr inbounds i8, i8* %s11, i64 5
  %6 = call i8* @strncpy(i8* %buf12, i8* %src13, i64 5)
  %npos14 = getelementptr inbounds i8, i8* %buf12, i64 5
  store i8 0, i8* %npos14, align 1
  %concat15 = call i8* @zen_concat(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @18, i32 0, i32 0), i8* %buf12)
  %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @19, i32 0, i32 0), i8* %concat15)
  %s16 = load i8*, i8** @g_s, align 8
  %found = call i8* @strstr(i8* %s16, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @21, i32 0, i32 0))
  %isnull = icmp eq i8* %found, null
  %fi = ptrtoint i8* %found to i64
  %si = ptrtoint i8* %s16 to i64
  %idx = sub i64 %fi, %si
  %idxd = uitofp i64 %idx to double
  %buscar_r = select i1 %isnull, double -1.000000e+00, double %idxd
  %numstr17 = call i8* @__zen_num_to_str(double %buscar_r)
  %concat18 = call i8* @zen_concat(i8* getelementptr inbounds ([17 x i8], [17 x i8]* @20, i32 0, i32 0), i8* %numstr17)
  %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @22, i32 0, i32 0), i8* %concat18)
  %s19 = load i8*, i8** @g_s, align 8
  %found20 = call i8* @strstr(i8* %s19, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @24, i32 0, i32 0))
  %isnull21 = icmp eq i8* %found20, null
  %fi22 = ptrtoint i8* %found20 to i64
  %si23 = ptrtoint i8* %s19 to i64
  %idx24 = sub i64 %fi22, %si23
  %idxd25 = uitofp i64 %idx24 to double
  %buscar_r26 = select i1 %isnull21, double -1.000000e+00, double %idxd25
  %numstr27 = call i8* @__zen_num_to_str(double %buscar_r26)
  %concat28 = call i8* @zen_concat(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @23, i32 0, i32 0), i8* %numstr27)
  %9 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @25, i32 0, i32 0), i8* %concat28)
  %s29 = load i8*, i8** @g_s, align 8
  %found30 = call i8* @strstr(i8* %s29, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @26, i32 0, i32 0))
  %isnull31 = icmp eq i8* %found30, null
  %si32 = ptrtoint i8* %s29 to i64
  %fi33 = ptrtoint i8* %found30 to i64
  %idx34 = sub i64 %fi33, %si32
  %vlen = call i64 @strlen(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @26, i32 0, i32 0))
  %rest = add i64 %idx34, %vlen
  %nlen = call i64 @strlen(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @27, i32 0, i32 0))
  %slen = call i64 @strlen(i8* %s29)
  %restlen = sub i64 %slen, %rest
  %tl1 = add i64 %idx34, %nlen
  %tl2 = add i64 %tl1, %restlen
  %ts = add i64 %tl2, 1
  %buf35 = call i8* @malloc(i64 %ts)
  %10 = call i8* @strncpy(i8* %buf35, i8* %s29, i64 %idx34)
  %bidx = getelementptr inbounds i8, i8* %buf35, i64 %idx34
  call void @memcpy(i8* %bidx, i8* getelementptr inbounds ([4 x i8], [4 x i8]* @27, i32 0, i32 0), i64 %nlen)
  %ipn = add i64 %idx34, %nlen
  %b2 = getelementptr inbounds i8, i8* %buf35, i64 %ipn
  %sr = getelementptr inbounds i8, i8* %s29, i64 %rest
  call void @memcpy(i8* %b2, i8* %sr, i64 %restlen)
  %end = getelementptr inbounds i8, i8* %buf35, i64 %tl2
  store i8 0, i8* %end, align 1
  %repl_r = select i1 %isnull31, i8* %s29, i8* %buf35
  store i8* %repl_r, i8** @g_r, align 8
  %r = load i8*, i8** @g_r, align 8
  %concat36 = call i8* @zen_concat(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @28, i32 0, i32 0), i8* %r)
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @29, i32 0, i32 0), i8* %concat36)
  %s37 = load i8*, i8** @g_s, align 8
  %slen38 = call i64 @strlen(i8* %s37)
  %size = add i64 %slen38, 1
  %buf39 = call i8* @malloc(i64 %size)
  %i = alloca i64, align 8
  store i64 0, i64* %i, align 4
  br label %case.cond

case.cond:                                        ; preds = %case.body, %endif5
  %i40 = load i64, i64* %i, align 4
  %cmp = icmp slt i64 %i40, %slen38
  br i1 %cmp, label %case.body, label %case.end

case.body:                                        ; preds = %case.cond
  %sp = getelementptr inbounds i8, i8* %s37, i64 %i40
  %dp = getelementptr inbounds i8, i8* %buf39, i64 %i40
  %ch = load i8, i8* %sp, align 1
  %ch32 = sext i8 %ch to i32
  %conv = call i32 @toupper(i32 %ch32)
  %conv8 = trunc i32 %conv to i8
  store i8 %conv8, i8* %dp, align 1
  %next = add i64 %i40, 1
  store i64 %next, i64* %i, align 4
  br label %case.cond

case.end:                                         ; preds = %case.cond
  %np = getelementptr inbounds i8, i8* %buf39, i64 %slen38
  store i8 0, i8* %np, align 1
  %concat41 = call i8* @zen_concat(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @30, i32 0, i32 0), i8* %buf39)
  %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @31, i32 0, i32 0), i8* %concat41)
  %slen42 = call i64 @strlen(i8* getelementptr inbounds ([11 x i8], [11 x i8]* @33, i32 0, i32 0))
  %size43 = add i64 %slen42, 1
  %buf44 = call i8* @malloc(i64 %size43)
  %i48 = alloca i64, align 8
  store i64 0, i64* %i48, align 4
  br label %case.cond45

case.cond45:                                      ; preds = %case.body46, %case.end
  %i49 = load i64, i64* %i48, align 4
  %cmp50 = icmp slt i64 %i49, %slen42
  br i1 %cmp50, label %case.body46, label %case.end47

case.body46:                                      ; preds = %case.cond45
  %sp51 = getelementptr inbounds i8, i8* getelementptr inbounds ([11 x i8], [11 x i8]* @33, i32 0, i32 0), i64 %i49
  %dp52 = getelementptr inbounds i8, i8* %buf44, i64 %i49
  %ch53 = load i8, i8* %sp51, align 1
  %ch3254 = sext i8 %ch53 to i32
  %conv55 = call i32 @tolower(i32 %ch3254)
  %conv856 = trunc i32 %conv55 to i8
  store i8 %conv856, i8* %dp52, align 1
  %next57 = add i64 %i49, 1
  store i64 %next57, i64* %i48, align 4
  br label %case.cond45

case.end47:                                       ; preds = %case.cond45
  %np58 = getelementptr inbounds i8, i8* %buf44, i64 %slen42
  store i8 0, i8* %np58, align 1
  %concat59 = call i8* @zen_concat(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @32, i32 0, i32 0), i8* %buf44)
  %13 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @34, i32 0, i32 0), i8* %concat59)
  store i8* getelementptr inbounds ([11 x i8], [11 x i8]* @35, i32 0, i32 0), i8** @g_espacios, align 8
  %espacios = load i8*, i8** @g_espacios, align 8
  %s.ptr = alloca i8*, align 8
  store i8* %espacios, i8** %s.ptr, align 8
  br label %trim.cond

trim.cond:                                        ; preds = %trim.body, %case.end47
  %s60 = load i8*, i8** %s.ptr, align 8
  %ch61 = load i8, i8* %s60, align 1
  %sp62 = icmp eq i8 %ch61, 32
  br i1 %sp62, label %trim.body, label %trim.end

trim.body:                                        ; preds = %trim.cond
  %next63 = getelementptr inbounds i8, i8* %s60, i64 1
  store i8* %next63, i8** %s.ptr, align 8
  br label %trim.cond

trim.end:                                         ; preds = %trim.cond
  %trimmed = load i8*, i8** %s.ptr, align 8
  %concat64 = call i8* @zen_concat(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @36, i32 0, i32 0), i8* %trimmed)
  %concat65 = call i8* @zen_concat(i8* %concat64, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @37, i32 0, i32 0))
  %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @38, i32 0, i32 0), i8* %concat65)
  %num = call double @strtod(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @39, i32 0, i32 0), i8** null)
  %numstr66 = call i8* @__zen_num_to_str(double %num)
  store i8* %numstr66, i8** @g_n, align 8
  %n = load i8*, i8** @g_n, align 8
  %s2d = call double @strtod(i8* %n, i8** null)
  %add = fadd double %s2d, 1.000000e+00
  %numstr67 = call i8* @__zen_num_to_str(double %add)
  %concat68 = call i8* @zen_concat(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @40, i32 0, i32 0), i8* %numstr67)
  %15 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @41, i32 0, i32 0), i8* %concat68)
  %numstr69 = call i8* @__zen_num_to_str(double 3.141590e+00)
  store i8* %numstr69, i8** @g_t, align 8
  %t = load i8*, i8** @g_t, align 8
  %concat70 = call i8* @zen_concat(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @42, i32 0, i32 0), i8* %t)
  %16 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @43, i32 0, i32 0), i8* %concat70)
  store i8* getelementptr inbounds ([16 x i8], [16 x i8]* @44, i32 0, i32 0), i8** @g_frase, align 8
  %frase = load i8*, i8** @g_frase, align 8
  %found71 = call i8* @strstr(i8* %frase, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @45, i32 0, i32 0))
  %isnull72 = icmp eq i8* %found71, null
  %fi73 = ptrtoint i8* %found71 to i64
  %si74 = ptrtoint i8* %frase to i64
  %idx75 = sub i64 %fi73, %si74
  %idxd76 = uitofp i64 %idx75 to double
  %buscar_r77 = select i1 %isnull72, double -1.000000e+00, double %idxd76
  %numstr78 = call i8* @__zen_num_to_str(double %buscar_r77)
  store i8* %numstr78, i8** @g_palabras_idx, align 8
  %palabras_idx = load i8*, i8** @g_palabras_idx, align 8
  %concat79 = call i8* @zen_concat(i8* getelementptr inbounds ([24 x i8], [24 x i8]* @46, i32 0, i32 0), i8* %palabras_idx)
  %17 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @47, i32 0, i32 0), i8* %concat79)
  %frase80 = load i8*, i8** @g_frase, align 8
  %found81 = call i8* @strstr(i8* %frase80, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @48, i32 0, i32 0))
  %isnull82 = icmp eq i8* %found81, null
  %si83 = ptrtoint i8* %frase80 to i64
  %fi84 = ptrtoint i8* %found81 to i64
  %idx85 = sub i64 %fi84, %si83
  %vlen86 = call i64 @strlen(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @48, i32 0, i32 0))
  %rest87 = add i64 %idx85, %vlen86
  %nlen88 = call i64 @strlen(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @49, i32 0, i32 0))
  %slen89 = call i64 @strlen(i8* %frase80)
  %restlen90 = sub i64 %slen89, %rest87
  %tl191 = add i64 %idx85, %nlen88
  %tl292 = add i64 %tl191, %restlen90
  %ts93 = add i64 %tl292, 1
  %buf94 = call i8* @malloc(i64 %ts93)
  %18 = call i8* @strncpy(i8* %buf94, i8* %frase80, i64 %idx85)
  %bidx95 = getelementptr inbounds i8, i8* %buf94, i64 %idx85
  call void @memcpy(i8* %bidx95, i8* getelementptr inbounds ([6 x i8], [6 x i8]* @49, i32 0, i32 0), i64 %nlen88)
  %ipn96 = add i64 %idx85, %nlen88
  %b297 = getelementptr inbounds i8, i8* %buf94, i64 %ipn96
  %sr98 = getelementptr inbounds i8, i8* %frase80, i64 %rest87
  call void @memcpy(i8* %b297, i8* %sr98, i64 %restlen90)
  %end99 = getelementptr inbounds i8, i8* %buf94, i64 %tl292
  store i8 0, i8* %end99, align 1
  %repl_r100 = select i1 %isnull82, i8* %frase80, i8* %buf94
  store i8* %repl_r100, i8** @g_nueva_frase, align 8
  %nueva_frase = load i8*, i8** @g_nueva_frase, align 8
  %concat101 = call i8* @zen_concat(i8* getelementptr inbounds ([14 x i8], [14 x i8]* @50, i32 0, i32 0), i8* %nueva_frase)
  %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @51, i32 0, i32 0), i8* %concat101)
  %nueva_frase102 = load i8*, i8** @g_nueva_frase, align 8
  %slen103 = call i64 @strlen(i8* %nueva_frase102)
  %size104 = add i64 %slen103, 1
  %buf105 = call i8* @malloc(i64 %size104)
  %i109 = alloca i64, align 8
  store i64 0, i64* %i109, align 4
  br label %case.cond106

case.cond106:                                     ; preds = %case.body107, %trim.end
  %i110 = load i64, i64* %i109, align 4
  %cmp111 = icmp slt i64 %i110, %slen103
  br i1 %cmp111, label %case.body107, label %case.end108

case.body107:                                     ; preds = %case.cond106
  %sp112 = getelementptr inbounds i8, i8* %nueva_frase102, i64 %i110
  %dp113 = getelementptr inbounds i8, i8* %buf105, i64 %i110
  %ch114 = load i8, i8* %sp112, align 1
  %ch32115 = sext i8 %ch114 to i32
  %conv116 = call i32 @toupper(i32 %ch32115)
  %conv8117 = trunc i32 %conv116 to i8
  store i8 %conv8117, i8* %dp113, align 1
  %next118 = add i64 %i110, 1
  store i64 %next118, i64* %i109, align 4
  br label %case.cond106

case.end108:                                      ; preds = %case.cond106
  %np119 = getelementptr inbounds i8, i8* %buf105, i64 %slen103
  store i8 0, i8* %np119, align 1
  %concat120 = call i8* @zen_concat(i8* getelementptr inbounds ([8 x i8], [8 x i8]* @52, i32 0, i32 0), i8* %buf105)
  %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @53, i32 0, i32 0), i8* %concat120)
  ret i32 0
}
