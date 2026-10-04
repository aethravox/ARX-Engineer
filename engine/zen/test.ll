; ModuleID = 'zen'
source_filename = "zen"

@.fmt = constant i8 zeroinitializer

define i32 @main() {
entry:
  %print = call i32 (...) @printf(i8* @.fmt, double 5.000000e+00)
  ret i32 0
}

declare i32 @printf(...)
