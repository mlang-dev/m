func remove __filename:string -> int
func rename __old:string __new:string -> int
func renameat __oldfd:int __old:string __newfd:int __new:string -> int
func tmpfile () -> string
func tmpnam __s:string -> string
func tmpnam_r __s:string -> string
func tempnam __dir:string __pfx:string -> string
func fclose __stream:string -> int
func fflush __stream:string -> int
func fflush_unlocked __stream:string -> int
func fopen __filename:string __modes:string -> string
func freopen __filename:string __modes:string __stream:string -> string
func fdopen __fd:int __modes:string -> string
func open_memstream __bufloc:string __sizeloc:string -> string
func fprintf __stream:string __format:string ... -> int
func printf __format:string ... -> int
func sprintf __s:string __format:string ... -> int
func vfprintf __s:string __format:string __arg:string -> int
func vprintf __format:string __arg:string -> int
func vsprintf __s:string __format:string __arg:string -> int
func dprintf __fd:int __fmt:string ... -> int
func fscanf __stream:string __format:string ... -> int
func scanf __format:string ... -> int
func sscanf __s:string __format:string ... -> int
func fscanf __stream:string __format:string ... -> int
func scanf __format:string ... -> int
func sscanf __s:string __format:string ... -> int
func vfscanf __s:string __format:string __arg:string -> int
func vscanf __format:string __arg:string -> int
func vsscanf __s:string __format:string __arg:string -> int
func vfscanf __s:string __format:string __arg:string -> int
func vscanf __format:string __arg:string -> int
func vsscanf __s:string __format:string __arg:string -> int
func fgetc __stream:string -> int
func getc __stream:string -> int
func getchar () -> int
func getc_unlocked __stream:string -> int
func getchar_unlocked () -> int
func fgetc_unlocked __stream:string -> int
func fputc __c:int __stream:string -> int
func putc __c:int __stream:string -> int
func putchar __c:int -> int
func fputc_unlocked __c:int __stream:string -> int
func putc_unlocked __c:int __stream:string -> int
func putchar_unlocked __c:int -> int
func getw __stream:string -> int
func putw __w:int __stream:string -> int
func fgets __s:string __n:int __stream:string -> string
func fputs __s:string __stream:string -> int
func puts __s:string -> int
func ungetc __c:int __stream:string -> int
func fgetpos __stream:string __pos:string -> int
func fsetpos __stream:string __pos:string -> int
func feof __stream:string -> int
func ferror __stream:string -> int
func feof_unlocked __stream:string -> int
func ferror_unlocked __stream:string -> int
func fileno __stream:string -> int
func fileno_unlocked __stream:string -> int
func popen __command:string __modes:string -> string
func pclose __stream:string -> int
func ctermid __s:string -> string
func ftrylockfile __stream:string -> int
func __uflow arg0:string -> int
func __overflow arg0:string arg1:int -> int
