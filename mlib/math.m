func __fpclassify __value:f64 -> int
func __signbit __value:f64 -> int
func __isinf __value:f64 -> int
func __finite __value:f64 -> int
func __isnan __value:f64 -> int
func __iseqsig __x:f64 __y:f64 -> int
func __issignaling __value:f64 -> int
func acos __x:f64 -> f64
func __acos __x:f64 -> f64
func asin __x:f64 -> f64
func __asin __x:f64 -> f64
func atan __x:f64 -> f64
func __atan __x:f64 -> f64
func atan2 __y:f64 __x:f64 -> f64
func __atan2 __y:f64 __x:f64 -> f64
func cos __x:f64 -> f64
func __cos __x:f64 -> f64
func sin __x:f64 -> f64
func __sin __x:f64 -> f64
func tan __x:f64 -> f64
func __tan __x:f64 -> f64
func cosh __x:f64 -> f64
func __cosh __x:f64 -> f64
func sinh __x:f64 -> f64
func __sinh __x:f64 -> f64
func tanh __x:f64 -> f64
func __tanh __x:f64 -> f64
func acosh __x:f64 -> f64
func __acosh __x:f64 -> f64
func asinh __x:f64 -> f64
func __asinh __x:f64 -> f64
func atanh __x:f64 -> f64
func __atanh __x:f64 -> f64
func exp __x:f64 -> f64
func __exp __x:f64 -> f64
func frexp __x:f64 __exponent:string -> f64
func __frexp __x:f64 __exponent:string -> f64
func ldexp __x:f64 __exponent:int -> f64
func __ldexp __x:f64 __exponent:int -> f64
func log __x:f64 -> f64
func __log __x:f64 -> f64
func log10 __x:f64 -> f64
func __log10 __x:f64 -> f64
func modf __x:f64 __iptr:string -> f64
func __modf __x:f64 __iptr:string -> f64
func expm1 __x:f64 -> f64
func __expm1 __x:f64 -> f64
func log1p __x:f64 -> f64
func __log1p __x:f64 -> f64
func logb __x:f64 -> f64
func __logb __x:f64 -> f64
func exp2 __x:f64 -> f64
func __exp2 __x:f64 -> f64
func log2 __x:f64 -> f64
func __log2 __x:f64 -> f64
func pow __x:f64 __y:f64 -> f64
func __pow __x:f64 __y:f64 -> f64
func sqrt __x:f64 -> f64
func __sqrt __x:f64 -> f64
func hypot __x:f64 __y:f64 -> f64
func __hypot __x:f64 __y:f64 -> f64
func cbrt __x:f64 -> f64
func __cbrt __x:f64 -> f64
func ceil __x:f64 -> f64
func __ceil __x:f64 -> f64
func fabs __x:f64 -> f64
func __fabs __x:f64 -> f64
func floor __x:f64 -> f64
func __floor __x:f64 -> f64
func fmod __x:f64 __y:f64 -> f64
func __fmod __x:f64 __y:f64 -> f64
func isinf __value:f64 -> int
func finite __value:f64 -> int
func drem __x:f64 __y:f64 -> f64
func __drem __x:f64 __y:f64 -> f64
func significand __x:f64 -> f64
func __significand __x:f64 -> f64
func copysign __x:f64 __y:f64 -> f64
func __copysign __x:f64 __y:f64 -> f64
func nan __tagb:string -> f64
func __nan __tagb:string -> f64
func isnan __value:f64 -> int
func j0 arg0:f64 -> f64
func __j0 arg0:f64 -> f64
func j1 arg0:f64 -> f64
func __j1 arg0:f64 -> f64
func jn arg0:int arg1:f64 -> f64
func __jn arg0:int arg1:f64 -> f64
func y0 arg0:f64 -> f64
func __y0 arg0:f64 -> f64
func y1 arg0:f64 -> f64
func __y1 arg0:f64 -> f64
func yn arg0:int arg1:f64 -> f64
func __yn arg0:int arg1:f64 -> f64
func erf arg0:f64 -> f64
func __erf arg0:f64 -> f64
func erfc arg0:f64 -> f64
func __erfc arg0:f64 -> f64
func lgamma arg0:f64 -> f64
func __lgamma arg0:f64 -> f64
func tgamma arg0:f64 -> f64
func __tgamma arg0:f64 -> f64
func gamma arg0:f64 -> f64
func __gamma arg0:f64 -> f64
func lgamma_r arg0:f64 __signgamp:string -> f64
func __lgamma_r arg0:f64 __signgamp:string -> f64
func rint __x:f64 -> f64
func __rint __x:f64 -> f64
func nextafter __x:f64 __y:f64 -> f64
func __nextafter __x:f64 __y:f64 -> f64
func remainder __x:f64 __y:f64 -> f64
func __remainder __x:f64 __y:f64 -> f64
func scalbn __x:f64 __n:int -> f64
func __scalbn __x:f64 __n:int -> f64
func ilogb __x:f64 -> int
func __ilogb __x:f64 -> int
func nearbyint __x:f64 -> f64
func __nearbyint __x:f64 -> f64
func round __x:f64 -> f64
func __round __x:f64 -> f64
func trunc __x:f64 -> f64
func __trunc __x:f64 -> f64
func remquo __x:f64 __y:f64 __quo:string -> f64
func __remquo __x:f64 __y:f64 __quo:string -> f64
func fdim __x:f64 __y:f64 -> f64
func __fdim __x:f64 __y:f64 -> f64
func fmax __x:f64 __y:f64 -> f64
func __fmax __x:f64 __y:f64 -> f64
func fmin __x:f64 __y:f64 -> f64
func __fmin __x:f64 __y:f64 -> f64
func fma __x:f64 __y:f64 __z:f64 -> f64
func __fma __x:f64 __y:f64 __z:f64 -> f64
func scalb __x:f64 __n:f64 -> f64
func __scalb __x:f64 __n:f64 -> f64
