fun remove(__filename:string) -> int
fun rename(__old:string, __new:string) -> int
fun renameat(__oldfd:int, __old:string, __newfd:int, __new:string) -> int
fun tempnam(__dir:string, __pfx:string) -> string
fun printf(__format:string, ...) -> int
fun sprintf(__s:string, __format:string, ...) -> int
fun snprintf(__s:string, __maxlen:u64, __format:string, ...) -> int
fun dprintf(__fd:int, __fmt:string, ...) -> int
fun scanf(__format:string, ...) -> int
fun sscanf(__s:string, __format:string, ...) -> int
fun scanf(__format:string, ...) -> int
fun sscanf(__s:string, __format:string, ...) -> int
fun getchar() -> int
fun getchar_unlocked() -> int
fun putchar(__c:int) -> int
fun putchar_unlocked(__c:int) -> int
fun puts(__s:string) -> int
fun perror(__s:string) -> None
fun ctermid(__s:string) -> string
