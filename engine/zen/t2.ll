; ModuleID = 'zen'
source_filename = "zen"

@x = global double 0.000000e+00

define i32 @main() {
entry:
  store double 1.000000e+01, double* @x, align 8
  ret i32 0
}
