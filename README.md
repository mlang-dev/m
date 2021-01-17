# m (mlang)
m is a succinct & expressive programming language, where elegant language construction is achieved without sacrificing C level of execution efficiency. It aims to support imperative and functional programming paradigms and is designed with zero-overhead abstraction to its maximum. It's a static-typed language with type inference, which means most of time types of variables are not required to be annotated. m supports native C data type without introducing reference count and GC overheads like Python and other languages do. Like C, m has fixed-length data allocated in stack and variable-length data allocated in heap, the stack memory is automatically released when it is out of scope. Unlike C the heap memory is released when its owner is out of scope statiscally by m compiler, which is similar like what Rust does and m programmer does not need to explicitly release heap memory. This avoids the cost of garbage collections.

Another goal of m language design is to eanble native calls between functions written in m and C language to maximize reuse of existing high performance libraries written in C.

In summary the followings are design characteristics of m:
* Zero-overhead abstraction
* Static typed with type inference
* Generic
* Imperative & functional paradigms
* Object with Interface support (but no inheritance)
* Memory management with ownership
* Succinct and mathmatics-friendly language constructions

The mlang is written in C but requires C++ linker due to llvm being implemented in C++. 

The code is able to be compiled on Windows/macOS/Linux, which has been tested on following platforms:
* macOS 10.15.7
* Ubuntu 20.04
* Windows 10

## dependencies

[Googletest](https://github.com/google/googletest) framework is used for unit tests.

[llvm](https://github.com/llvm/llvm-project) is used as m's backend implementation.

[WebKit coding](https://webkit.org/code-style-guidelines/) coding style is adopted here.

## m syntax
```
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
* Compiler: c++ compilers: gcc/clang/vc++(Visual Studio with C++) 

## get source codes
```
git clone https://github.com/ligangwang/m
cd m
git submodule init
git submodule update
```

## build llvm
```
cd ./extern/llvm-project
mkdir build
cd build
cmake -Thost=x64 -DLLVM_ENABLE_RTTI=ON -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_PROJECTS=clang ../llvm
cmake --build . --config Release -j NN    NN - number of CPU (cores) that you have
cmake --build . --target install 
cd ../../../
```

## build mlang
```
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
The build system will build m executable under ./src on macOS/Linux, or .\src\Release on Windows

## using m REPL
```
./src/m
```

## using m compiler
```
./src/m ./samples/sample_lib.m
```
The above command using m compiler generates an object file "sample_lib.o" under the same folder, which will be used to be compiled with C code in the following example.

## c calls m functions
```
clang++ ./samples/sample_main.cc ./samples/sample_lib.o ./runtime.o -o ./sample
```

## useful tools
* Learn llvm IR
```
clang -S -emit-llvm ./samples/sample_main.c ./samples/sample_test.c
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
./src/c2m -i/usr/include -o../src/lib stdio.h
./src/c2m -i/usr/include -o../src/lib math.h
```
