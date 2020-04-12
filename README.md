# m

m represents Model, or Mathematics, a succinct & expressive programming language. It supports imperative and functional programming paradigms targeting high performance computing.



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
  if n < 2 then n
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

* WebKit coding style is adopted here: https://webkit.org/code-style-guidelines/