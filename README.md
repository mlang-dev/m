# m (mlang)
m is a succinct & expressive programming language, where elegant language construction is achieved without sacrificing C level of execution efficiency. It aims to support imperative and functional programming paradigms and is designed with zero-overhead abstraction to its maximum. It's a static-typed language with type inference, which means most of time types of variables are not required to be annotated. m supports native C data type without introducing reference count and GC overheads like Python and other high-level languages do. Like C, m supports memory allocations on stack and heap, the stack memory is automatically released when it is out of scope. Unlike C the heap memory release instruction is automatically inserted where its owner on the stack is out of scope statically at compile time by m, and m programmer does not need to explicitly release heap memory. This design avoids the non-deterministic garbage collections and is tailored for real-time system programming.

Another goal of m language design is to enable native calls between functions written in m and C language to maximize reuse of existing high performance libraries written in C/C++.

In summary the followings are design characteristics of m:
* Static typed with type inference
* Generic
* Polymorphic Composition and Interface
* Memory management with ownership
* Succinct and mathmatics-friendly language constructions
* Zero-overhead abstraction
* Imperative & functional paradigms


The mlang is written in C but requires C++ linker to build due to llvm being implemented in C++. 

The code is still under developement but is able to be compiled on Windows/macOS/Linux, which has been tested on following platforms:
* macOS 10.15.7
* Ubuntu 20.04
* Windows 10


## dependencies

[Googletest](https://github.com/google/googletest) framework is used for unit tests.

[llvm](https://github.com/llvm/llvm-project) is used as m's backend implementation.

## m code snippets
```
# hello world !
main () = printf "hello world !\n"

# comment line: defines a one-line function
avg x y = (x + y) / 2

# defines a distance function
distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy) # call c std sqrt math function

# factorial function with recursive call
factorial n = 
  if n < 2 then n
  else n * factorial (n-1)

# using for loop
loopprint n = 
  for i in 0..n
    printf "%d" i   # call c std io printf function
```

## prerequisites to build m
* Source code version control: git
* Build system generator: cmake
* Build system: GNU make (Unix-like system) or MSBuild (Windows)
* Compiler: c++ compilers: gcc/clang/vc++(Visual Studio Community Edition with Desktop Developement with C++) 

## get source codes
```
git clone https://github.com/ligangwang/m
cd m
git submodule init
git submodule update
```

## build llvm
Note: You can skip building llvm from source but just install binary (version 11.0) from [llvm site](https://releases.llvm.org/). 

Replace "Debug" with "Release" in following commands if Release build is desired.
```
cd ./extern/llvm-project
mkdir build
cd build
cmake -DLLVM_ENABLE_RTTI=ON -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS="clang;lld" ../llvm
sudo cmake --build . --config Debug -j NN --target install   (Run under Administrator, NN - number of CPU (cores) that you have)
cd ../../../
```
On Windows, llvm binary folder(C:\Program Files (x86)\LLVM\bin) needs to be added to the Path environment variable so that "llvm-config" command is able to be executed in cmake. 

## build mlang
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --config Debug
```
The build system will build m executable under ./tools on macOS/Linux, or .\tools\Debug on Windows

## using m REPL
```
./tools/m
```

## using m compiler
```
./tools/m ./samples/sample_lib.m
```
The above command using m compiler generates an object file "sample_lib.o" under the same folder, which will be used to be compiled with C code in the following example.

## c calls m functions
```
clang++ ./samples/sample_main.cc ./samples/sample_lib.o ./runtime.o -o ./sample
```

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
./tools/c2m -i/usr/include -o../mlib stdio.h
./tools/c2m -i/usr/include -o../mlib math.h
```

* ld link to executable command. Unix-like system crt1.0: _start entry point for ELF, crti.o: initialization, crtn.o: finalization
```
ld -o helloworld -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib/x86_64-linux-gnu/crt1.o /usr/lib/x86_64-linux-gnu/crti.o -lc helloworld.o /usr/lib/x86_64-linux-gnu/crtn.o
```
* MSVC link
```
link /ENTRY:main  legacy_stdio_definitions.lib ucrt.lib helloworld.o 
```