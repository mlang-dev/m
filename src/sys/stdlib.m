func atof(__nptr:string) -> f64
func atoi(__nptr:string) -> int
func strtod(__nptr:string, __endptr:&string) -> f64
func strtoul(__nptr:string, __endptr:&string, __base:int) -> u64
func srandom(__seed:int) -> None
func setstate(__statebuf:string) -> string
func rand() -> int
func srand(__seed:int) -> None
func rand_r(__seed:&int) -> int
func drand48() -> f64
func malloc(__size:u64) -> &u8
func calloc(__nmemb:u64, __size:u64) -> &u8
func realloc(__ptr:&u8, __size:u64) -> &u8
func free(__ptr:&u8) -> None
func alloca(__size:u64) -> &u8
func aligned_alloc(__alignment:u64, __size:u64) -> &u8
func abort() -> None
func exit(__status:int) -> None
func quick_exit(__status:int) -> None
func _Exit(__status:int) -> None
func getenv(__name:string) -> string
func putenv(__string:string) -> int
func setenv(__name:string, __value:string, __replace:int) -> int
func unsetenv(__name:string) -> int
func clearenv() -> int
func mktemp(__template:string) -> string
func mkstemp(__template:string) -> int
func mkstemps(__template:string, __suffixlen:int) -> int
func mkdtemp(__template:string) -> string
func system(__command:string) -> int
func realpath(__name:string, __resolved:string) -> string
func abs(__x:int) -> int
func ecvt(__value:f64, __ndigit:int, __decpt:&int, __sign:&int) -> string
func fcvt(__value:f64, __ndigit:int, __decpt:&int, __sign:&int) -> string
func gcvt(__value:f64, __ndigit:int, __buf:string) -> string
func rpmatch(__response:string) -> int
func getsubopt(__optionp:&string, __tokens:&string, __valuep:&string) -> int
