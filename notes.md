
## useful tools
* m commands
```
./tools/Debug/m -fir ../samples/test.m
```
* Learn llvm IR
```
clang -S -emit-llvm ./samples/test_c.c -o ./samples/test_c.ll
```

* clang RVO (return value optimization)
```
clang -S -emit-llvm ./test/test_rvo.c
```

* Dump c/c++ header ast
```
clang -x c++ -Xclang -ast-dump -fsyntax-only ./include/test-ccompiler.h
```


* Generate stdio.m/math.m lib files
```
./apps/c2m -i/usr/include -o../mlib stdio.h
./apps/c2m -i/usr/include -o../mlib math.h
```

* ld link to executable command. Unix-like system crt1.0: _start entry point for ELF, crti.o: initialization, crtn.o: finalization
```
ld -o helloworld -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o -lc helloworld.o /usr/lib/x86_64-linux-gnu/crtn.o
```
* MSVC link
```
link /ENTRY:main  legacy_stdio_definitions.lib ucrt.lib helloworld.o 
```

* history
May 31, 2020	m 0.0.16 - support generic/type inference/stack-based types. :smile:
Feb 29, 2020	mlang.dev site online & m 0.0.9 with if and for control support. :sparkles:
Feb 1, 2020	about m(mlang)