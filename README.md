# m

m represents Model, or Mathematics, is a succinct & expressive programming language


## build command:
clang++ `/usr/local/Cellar/llvm/9.0.0_1/bin/llvm-config --ldflags --libs --system-libs` src/*.cc -Iinclude -I/usr/local/Cellar/llvm/9.0.0_1/include -std=c++17 -stdlib=libc++ -o m

## using m interactive:
./m

## using m compiler: 
./m ./examples/test_lib.m

## calling function written in m from c:
clang++ ./examples/test_main.cc ./examples/test_lib.o -o ./examples/test_main.o

## m syntax
```
# comment line: this defines an one line function
avg x y = (x + y) / 2


# this is a distance function
distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)
```

to learn llvm IR:
clang -S -emit-llvm ./examples/test_main.cc
