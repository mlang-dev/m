# m

m is a succinct & expressive programming language, where efficiency and elegance meet. It aims to support imperative and functional programming paradigms and is designed with zero-overhead abstraction whenever possible.

It's written in c but requires c++ linker due to llvm being implemented in c++.

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
cmake -DLLVM_ENABLE_RTTI=ON ../llvm
cmake --build . -j NN    NN - number of CPU (cores) that you have
cmake --build . --target install 
cd ../../../
```

## build mlang
make sure CPATH env variable is set to folder where c standard headers are found. e.g. /usr/include or /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include
```
export CPATH=/usr/include
mkdir build
cd build
cmake ..
make
```

## using m REPL:
./m

## using m compiler: 
./m ./samples/sample_lib.m

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

clang -S -emit-llvm ./examples/test_main.cc

* Dump c/c++ header ast

clang -x c++ -Xclang -ast-dump -fsyntax-only ./include/runtime.h

* WebKit coding style is adopted here: https://webkit.org/code-style-guidelines/

* ./src/c2m -i/usr/include -o../src/lib stdio.h