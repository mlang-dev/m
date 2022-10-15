fun remove __filename:string -> int
fun rename __old:string __new:string -> int
fun renameat __oldfd:int __old:string __newfd:int __new:string -> int
fun tmpfile () -> string
fun tmpnam __s:string -> string
fun tmpnam_r __s:string -> string
fun tempnam __dir:string __pfx:string -> string
fun fclose __stream:string -> int
fun fflush __stream:string -> int
fun fflush_unlocked __stream:string -> int
fun fopen __filename:string __modes:string -> string
fun freopen __filename:string __modes:string __stream:string -> string
fun fdopen __fd:int __modes:string -> string
fun open_memstream __bufloc:string __sizeloc:string -> string
fun fprintf __stream:string __format:string ... -> int
fun printf __format:string ... -> int
fun sprintf __s:string __format:string ... -> int
fun vfprintf __s:string __format:string __arg:string -> int
fun vprintf __format:string __arg:string -> int
fun vsprintf __s:string __format:string __arg:string -> int
fun dprintf __fd:int __fmt:string ... -> int
fun fscanf __stream:string __format:string ... -> int
fun scanf __format:string ... -> int
fun sscanf __s:string __format:string ... -> int
fun fscanf __stream:string __format:string ... -> int
fun scanf __format:string ... -> int
fun sscanf __s:string __format:string ... -> int
fun vfscanf __s:string __format:string __arg:string -> int
fun vscanf __format:string __arg:string -> int
fun vsscanf __s:string __format:string __arg:string -> int
fun vfscanf __s:string __format:string __arg:string -> int
fun vscanf __format:string __arg:string -> int
fun vsscanf __s:string __format:string __arg:string -> int
fun fgetc __stream:string -> int
fun getc __stream:string -> int
fun getchar () -> int
fun getc_unlocked __stream:string -> int
fun getchar_unlocked () -> int
fun fgetc_unlocked __stream:string -> int
fun fputc __c:int __stream:string -> int
fun putc __c:int __stream:string -> int
fun putchar __c:int -> int
fun fputc_unlocked __c:int __stream:string -> int
fun putc_unlocked __c:int __stream:string -> int
fun putchar_unlocked __c:int -> int
fun getw __stream:string -> int
fun putw __w:int __stream:string -> int
fun fgets __s:string __n:int __stream:string -> string
fun fputs __s:string __stream:string -> int
fun puts __s:string -> int
fun ungetc __c:int __stream:string -> int
fun fgetpos __stream:string __pos:string -> int
fun fsetpos __stream:string __pos:string -> int
fun feof __stream:string -> int
fun ferror __stream:string -> int
fun feof_unlocked __stream:string -> int
fun ferror_unlocked __stream:string -> int
fun fileno __stream:string -> int
fun fileno_unlocked __stream:string -> int
fun popen __command:string __modes:string -> string
fun pclose __stream:string -> int
fun ctermid __s:string -> string
fun ftrylockfile __stream:string -> int
fun __uflow arg0:string -> int
fun __overflow arg0:string arg1:int -> int
