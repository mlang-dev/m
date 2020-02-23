# m

m represents Model, or Mathematics, is to be a most succinct & expressive programming language. It supports imperative and functional programming targeting high performance computing.


## build command:
clang++ `/usr/local/Cellar/llvm/9.0.0_1/bin/llvm-config --ldflags --libs --system-libs` src/*.cc -Iinclude -I/usr/local/Cellar/llvm/9.0.0_1/include -std=c++17 -stdlib=libc++ -o m

clang -c ./src/runtime.cc -I./include -o ./runtime.o

## or using cmake to build
```
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
  if n < 2 n
  else n * factorial (n-1)

# using for loop
loopprint n = 
  for i in 0..n
    print i
```

## useful tools
* learn llvm IR

clang -S -emit-llvm ./examples/test_main.cc

* dump c/c++ header ast

clang -x c++ -Xclang -ast-dump -fsyntax-only ./include/runtime.h

