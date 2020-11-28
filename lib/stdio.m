extern remove(__filename:string):int
extern rename(__old:string __new:string):int
extern renameat(__oldfd:int __old:string __newfd:int __new:string):int
extern tmpfile():string
extern tmpnam(__s:string):string
extern tmpnam_r(__s:string):string
extern tempnam(__dir:string __pfx:string):string
extern fclose(__stream:string):int
extern fflush(__stream:string):int
extern fflush_unlocked(__stream:string):int
extern fopen(__filename:string __modes:string):string
extern freopen(__filename:string __modes:string __stream:string):string
extern fdopen(__fd:int __modes:string):string
extern fmemopen(__s:string __len:int __modes:string):string
extern open_memstream(__bufloc:string __sizeloc:string):string
extern setvbuf(__stream:string __buf:string __modes:int __n:int):int
extern fprintf(__stream:string __format:string ...):int
extern printf(__format:string ...):int
extern sprintf(__s:string __format:string ...):int
extern vfprintf(__s:string __format:string __arg:int):int
extern vprintf(__format:string __arg:int):int
extern vsprintf(__s:string __format:string __arg:int):int
extern snprintf(__s:string __maxlen:int __format:string ...):int
extern vsnprintf(__s:string __maxlen:int __format:string __arg:int):int
extern vdprintf(__fd:int __fmt:string __arg:int):int
extern dprintf(__fd:int __fmt:string ...):int
extern fscanf(__stream:string __format:string ...):int
extern scanf(__format:string ...):int
extern sscanf(__s:string __format:string ...):int
extern fscanf(__stream:string __format:string ...):int
extern scanf(__format:string ...):int
extern sscanf(__s:string __format:string ...):int
extern vfscanf(__s:string __format:string __arg:int):int
extern vscanf(__format:string __arg:int):int
extern vsscanf(__s:string __format:string __arg:int):int
extern vfscanf(__s:string __format:string __arg:int):int
extern vscanf(__format:string __arg:int):int
extern vsscanf(__s:string __format:string __arg:int):int
extern fgetc(__stream:string):int
extern getc(__stream:string):int
extern getchar():int
extern getc_unlocked(__stream:string):int
extern getchar_unlocked():int
extern fgetc_unlocked(__stream:string):int
extern fputc(__c:int __stream:string):int
extern putc(__c:int __stream:string):int
extern putchar(__c:int):int
extern fputc_unlocked(__c:int __stream:string):int
extern putc_unlocked(__c:int __stream:string):int
extern putchar_unlocked(__c:int):int
extern getw(__stream:string):int
extern putw(__w:int __stream:string):int
extern fgets(__s:string __n:int __stream:string):string
extern fputs(__s:string __stream:string):int
extern puts(__s:string):int
extern ungetc(__c:int __stream:string):int
extern fread(__ptr:string __size:int __n:int __stream:string):int
extern fwrite(__ptr:string __size:int __n:int __s:string):int
extern fread_unlocked(__ptr:string __size:int __n:int __stream:string):int
extern fwrite_unlocked(__ptr:string __size:int __n:int __stream:string):int
extern fgetpos(__stream:string __pos:string):int
extern fsetpos(__stream:string __pos:string):int
extern feof(__stream:string):int
extern ferror(__stream:string):int
extern feof_unlocked(__stream:string):int
extern ferror_unlocked(__stream:string):int
extern fileno(__stream:string):int
extern fileno_unlocked(__stream:string):int
extern popen(__command:string __modes:string):string
extern pclose(__stream:string):int
extern ctermid(__s:string):string
extern ftrylockfile(__stream:string):int
extern __uflow(arg0:string):int
extern __overflow(arg0:string arg1:int):int
