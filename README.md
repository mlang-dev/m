# m (mlang)
m is a succinct & expressive programming language, where efficiency and elegance meet. It aims to support imperative and functional programming paradigms and is designed with zero-overhead abstraction to its maximum. It supports native C data type without introducing reference count and GC overheads like Python and other languages do. Like c, m has fixed-length data allocated in stack and variable-length data allocated in heap, the stack memory is automatically released when it is out of scope. Unlike C the heap memory is released when its owner is out of scope statiscally by m compiler, which is similar like what Rust does and m programmer does not need to explicitly release heap memory. This avoids the cost of garbage collections.

Another goal of m language design is to eanble native calls between functions written in m and c language to maximize reuse of existing high performance libraries written in C.

The mlang is written in c but requires c++ linker due to llvm being implemented in c++. 

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
make sure MPATH env variable is set to the src folder of mlang e.g. ~/dev/m/src
```
export MPATH=~/dev/m/src (on Windows run: set MPATH=c:\dev\m\src)
mkdir build
cd build
cmake ..
cmake --build . --config Release
```
The build system will build m executable under ./src on macOS/Linux, or .\src\Release on Windows

## using m REPL
./src/m

## using m compiler
./src/m ./samples/sample_lib.m


## c calls m functions:
clang++ ./samples/sample_main.cc ./samples/sample_lib.o ./runtime.o -o ./sample

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
