func remove(__filename:string) -> int
func rename(__old:string, __new:string) -> int
func renameat(__oldfd:int, __old:string, __newfd:int, __new:string) -> int
func tempnam(__dir:string, __pfx:string) -> string
func printf(__format:string, ...) -> int
func sprintf(__s:string, __format:string, ...) -> int
func snprintf(__s:string, __maxlen:u64, __format:string, ...) -> int
func dprintf(__fd:int, __fmt:string, ...) -> int
func scanf(__format:string, ...) -> int
func sscanf(__s:string, __format:string, ...) -> int
func scanf(__format:string, ...) -> int
func sscanf(__s:string, __format:string, ...) -> int
func getchar() -> int
func getchar_unlocked() -> int
func putchar(__c:int) -> int
func putchar_unlocked(__c:int) -> int
func puts(__s:string) -> int
func perror(__s:string) -> None
func ctermid(__s:string) -> string
