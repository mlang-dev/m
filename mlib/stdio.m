extern remove:int __filename:string
extern rename:int __old:string __new:string
extern renameat:int __oldfd:int __old:string __newfd:int __new:string
extern tmpfile:string ()
extern tmpnam:string __s:string
extern tmpnam_r:string __s:string
extern tempnam:string __dir:string __pfx:string
extern fclose:int __stream:string
extern fflush:int __stream:string
extern fflush_unlocked:int __stream:string
extern fopen:string __filename:string __modes:string
extern freopen:string __filename:string __modes:string __stream:string
extern fdopen:string __fd:int __modes:string
extern open_memstream:string __bufloc:string __sizeloc:string
extern fprintf:int __stream:string __format:string ...
extern printf:int __format:string ...
extern sprintf:int __s:string __format:string ...
extern vfprintf:int __s:string __format:string __arg:string
extern vprintf:int __format:string __arg:string
extern vsprintf:int __s:string __format:string __arg:string
extern dprintf:int __fd:int __fmt:string ...
extern fscanf:int __stream:string __format:string ...
extern scanf:int __format:string ...
extern sscanf:int __s:string __format:string ...
extern fscanf:int __stream:string __format:string ...
extern scanf:int __format:string ...
extern sscanf:int __s:string __format:string ...
extern vfscanf:int __s:string __format:string __arg:string
extern vscanf:int __format:string __arg:string
extern vsscanf:int __s:string __format:string __arg:string
extern vfscanf:int __s:string __format:string __arg:string
extern vscanf:int __format:string __arg:string
extern vsscanf:int __s:string __format:string __arg:string
extern fgetc:int __stream:string
extern getc:int __stream:string
extern getchar:int ()
extern getc_unlocked:int __stream:string
extern getchar_unlocked:int ()
extern fgetc_unlocked:int __stream:string
extern fputc:int __c:int __stream:string
extern putc:int __c:int __stream:string
extern putchar:int __c:int
extern fputc_unlocked:int __c:int __stream:string
extern putc_unlocked:int __c:int __stream:string
extern putchar_unlocked:int __c:int
extern getw:int __stream:string
extern putw:int __w:int __stream:string
extern fgets:string __s:string __n:int __stream:string
extern fputs:int __s:string __stream:string
extern puts:int __s:string
extern ungetc:int __c:int __stream:string
extern fgetpos:int __stream:string __pos:string
extern fsetpos:int __stream:string __pos:string
extern feof:int __stream:string
extern ferror:int __stream:string
extern feof_unlocked:int __stream:string
extern ferror_unlocked:int __stream:string
extern fileno:int __stream:string
extern fileno_unlocked:int __stream:string
extern popen:string __command:string __modes:string
extern pclose:int __stream:string
extern ctermid:string __s:string
extern ftrylockfile:int __stream:string
extern __uflow:int arg0:string
extern __overflow:int arg0:string arg1:int
