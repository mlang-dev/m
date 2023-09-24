fun atof(__nptr:string) -> f64
fun atoi(__nptr:string) -> int
fun strtod(__nptr:string, __endptr:&string) -> f64
fun strtoul(__nptr:string, __endptr:&string, __base:int) -> u64
fun srandom(__seed:int) -> None
fun setstate(__statebuf:string) -> string
fun rand() -> int
fun srand(__seed:int) -> None
fun rand_r(__seed:&int) -> int
fun drand48() -> f64
fun malloc(__size:u64) -> &u8
fun calloc(__nmemb:u64, __size:u64) -> &u8
fun realloc(__ptr:&u8, __size:u64) -> &u8
fun free(__ptr:&u8) -> None
fun alloca(__size:u64) -> &u8
fun aligned_alloc(__alignment:u64, __size:u64) -> &u8
fun abort() -> None
fun exit(__status:int) -> None
fun quick_exit(__status:int) -> None
fun _Exit(__status:int) -> None
fun getenv(__name:string) -> string
fun putenv(__string:string) -> int
fun setenv(__name:string, __value:string, __replace:int) -> int
fun unsetenv(__name:string) -> int
fun clearenv() -> int
fun mktemp(__template:string) -> string
fun mkstemp(__template:string) -> int
fun mkstemps(__template:string, __suffixlen:int) -> int
fun mkdtemp(__template:string) -> string
fun system(__command:string) -> int
fun realpath(__name:string, __resolved:string) -> string
fun abs(__x:int) -> int
fun ecvt(__value:f64, __ndigit:int, __decpt:&int, __sign:&int) -> string
fun fcvt(__value:f64, __ndigit:int, __decpt:&int, __sign:&int) -> string
fun gcvt(__value:f64, __ndigit:int, __buf:string) -> string
fun rpmatch(__response:string) -> int
fun getsubopt(__optionp:&string, __tokens:&string, __valuep:&string) -> int
