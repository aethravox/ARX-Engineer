; ModuleID = 'zen'
source_filename = "zen"

@.fmt = constant [1 x i8] zeroinitializer

define i32 @main() {
entry:
  %print = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([1 x i8], [1 x i8]* @.fmt, i32 0, i32 0), double 4.200000e+01)
  ret i32 0
}

declare i32 @printf(i8*, ...)
