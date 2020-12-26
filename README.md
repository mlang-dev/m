# m

m is a succinct & expressive programming language, where efficiency and elegance meet. It aims to support imperative and functional programming paradigms and is designed with zero-overhead abstraction whenever possible.

It's written in c but requires c++ linker due to llvm being implemented in c++.

[Googletest](https://github.com/google/googletest) framework is used for unit tests.

The code is able to be compiled on Windows/MacOS/Linux.

It's been tested on following platforms:
MacOS 10.15.7
Ubuntu 20.04
Windows 10

## prerequisites
Source Code Version Control: git
Build system generator: cmake
Build system: GNU make (Unix-like system) or MSBuild (Windows)
Compiler: c++ compilers: gcc/clang/vc++(Visual Studio with C++) 

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

## using m REPL:
./src/m

## using m compiler: 
./src/m ./samples/sample_lib.m

## c calls m functions:
clang++ ./samples/sample_main.cc ./samples/sample_lib.o ./runtime.o -o ./sample

## m syntax
```
# comment line: defines a one-line function
avg x y = (x + y) / 2

# defines a distance function
distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)

# factorial function with recursive call
factorial n = 
  if n < 2 then n
  else n * factorial (n-1)

# using for loop
loopprint n = 
  for i in 0..n
    printf "%d" i   # call c stdio printf
```

## useful tools
* Learn llvm IR

clang -S -emit-llvm ./samples/sample_main.c ./samples/sample_test.c

* Dump c/c++ header ast

clang -x c++ -Xclang -ast-dump -fsyntax-only ./include/runtime.h

* [WebKit coding](https://webkit.org/code-style-guidelines/) style is adopted here.

re-generate stdio.m/math.m lib files

* ./src/c2m -i/usr/include -o../src/lib stdio.h
* ./src/c2m -i/usr/include -o../src/lib math.h