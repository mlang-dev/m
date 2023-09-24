fun __fpclassify(__value:f64) -> int
fun __signbit(__value:f64) -> int
fun __isinf(__value:f64) -> int
fun __finite(__value:f64) -> int
fun __isnan(__value:f64) -> int
fun __iseqsig(__x:f64, __y:f64) -> int
fun __issignaling(__value:f64) -> int
fun acos(__x:f64) -> f64
fun __acos(__x:f64) -> f64
fun asin(__x:f64) -> f64
fun __asin(__x:f64) -> f64
fun atan(__x:f64) -> f64
fun __atan(__x:f64) -> f64
fun atan2(__y:f64, __x:f64) -> f64
fun __atan2(__y:f64, __x:f64) -> f64
fun cos(__x:f64) -> f64
fun __cos(__x:f64) -> f64
fun sin(__x:f64) -> f64
fun __sin(__x:f64) -> f64
fun tan(__x:f64) -> f64
fun __tan(__x:f64) -> f64
fun cosh(__x:f64) -> f64
fun __cosh(__x:f64) -> f64
fun sinh(__x:f64) -> f64
fun __sinh(__x:f64) -> f64
fun tanh(__x:f64) -> f64
fun __tanh(__x:f64) -> f64
fun acosh(__x:f64) -> f64
fun __acosh(__x:f64) -> f64
fun asinh(__x:f64) -> f64
fun __asinh(__x:f64) -> f64
fun atanh(__x:f64) -> f64
fun __atanh(__x:f64) -> f64
fun exp(__x:f64) -> f64
fun __exp(__x:f64) -> f64
fun frexp(__x:f64, __exponent:&int) -> f64
fun __frexp(__x:f64, __exponent:&int) -> f64
fun ldexp(__x:f64, __exponent:int) -> f64
fun __ldexp(__x:f64, __exponent:int) -> f64
fun log(__x:f64) -> f64
fun __log(__x:f64) -> f64
fun log10(__x:f64) -> f64
fun __log10(__x:f64) -> f64
fun modf(__x:f64, __iptr:&f64) -> f64
fun __modf(__x:f64, __iptr:&f64) -> f64
fun expm1(__x:f64) -> f64
fun __expm1(__x:f64) -> f64
fun log1p(__x:f64) -> f64
fun __log1p(__x:f64) -> f64
fun logb(__x:f64) -> f64
fun __logb(__x:f64) -> f64
fun exp2(__x:f64) -> f64
fun __exp2(__x:f64) -> f64
fun log2(__x:f64) -> f64
fun __log2(__x:f64) -> f64
fun pow(__x:f64, __y:f64) -> f64
fun __pow(__x:f64, __y:f64) -> f64
fun sqrt(__x:f64) -> f64
fun __sqrt(__x:f64) -> f64
fun hypot(__x:f64, __y:f64) -> f64
fun __hypot(__x:f64, __y:f64) -> f64
fun cbrt(__x:f64) -> f64
fun __cbrt(__x:f64) -> f64
fun ceil(__x:f64) -> f64
fun __ceil(__x:f64) -> f64
fun fabs(__x:f64) -> f64
fun __fabs(__x:f64) -> f64
fun floor(__x:f64) -> f64
fun __floor(__x:f64) -> f64
fun fmod(__x:f64, __y:f64) -> f64
fun __fmod(__x:f64, __y:f64) -> f64
fun isinf(__value:f64) -> int
fun finite(__value:f64) -> int
fun drem(__x:f64, __y:f64) -> f64
fun __drem(__x:f64, __y:f64) -> f64
fun significand(__x:f64) -> f64
fun __significand(__x:f64) -> f64
fun copysign(__x:f64, __y:f64) -> f64
fun __copysign(__x:f64, __y:f64) -> f64
fun nan(__tagb:string) -> f64
fun __nan(__tagb:string) -> f64
fun isnan(__value:f64) -> int
fun j0(arg0:f64) -> f64
fun __j0(arg0:f64) -> f64
fun j1(arg0:f64) -> f64
fun __j1(arg0:f64) -> f64
fun jn(arg0:int, arg1:f64) -> f64
fun __jn(arg0:int, arg1:f64) -> f64
fun y0(arg0:f64) -> f64
fun __y0(arg0:f64) -> f64
fun y1(arg0:f64) -> f64
fun __y1(arg0:f64) -> f64
fun yn(arg0:int, arg1:f64) -> f64
fun __yn(arg0:int, arg1:f64) -> f64
fun erf(arg0:f64) -> f64
fun __erf(arg0:f64) -> f64
fun erfc(arg0:f64) -> f64
fun __erfc(arg0:f64) -> f64
fun lgamma(arg0:f64) -> f64
fun __lgamma(arg0:f64) -> f64
fun tgamma(arg0:f64) -> f64
fun __tgamma(arg0:f64) -> f64
fun gamma(arg0:f64) -> f64
fun __gamma(arg0:f64) -> f64
fun lgamma_r(arg0:f64, __signgamp:&int) -> f64
fun __lgamma_r(arg0:f64, __signgamp:&int) -> f64
fun rint(__x:f64) -> f64
fun __rint(__x:f64) -> f64
fun nextafter(__x:f64, __y:f64) -> f64
fun __nextafter(__x:f64, __y:f64) -> f64
fun remainder(__x:f64, __y:f64) -> f64
fun __remainder(__x:f64, __y:f64) -> f64
fun scalbn(__x:f64, __n:int) -> f64
fun __scalbn(__x:f64, __n:int) -> f64
fun ilogb(__x:f64) -> int
fun __ilogb(__x:f64) -> int
fun nearbyint(__x:f64) -> f64
fun __nearbyint(__x:f64) -> f64
fun round(__x:f64) -> f64
fun __round(__x:f64) -> f64
fun trunc(__x:f64) -> f64
fun __trunc(__x:f64) -> f64
fun remquo(__x:f64, __y:f64, __quo:&int) -> f64
fun __remquo(__x:f64, __y:f64, __quo:&int) -> f64
fun fdim(__x:f64, __y:f64) -> f64
fun __fdim(__x:f64, __y:f64) -> f64
fun fmax(__x:f64, __y:f64) -> f64
fun __fmax(__x:f64, __y:f64) -> f64
fun fmin(__x:f64, __y:f64) -> f64
fun __fmin(__x:f64, __y:f64) -> f64
fun fma(__x:f64, __y:f64, __z:f64) -> f64
fun __fma(__x:f64, __y:f64, __z:f64) -> f64
fun scalb(__x:f64, __n:f64) -> f64
fun __scalb(__x:f64, __n:f64) -> f64
