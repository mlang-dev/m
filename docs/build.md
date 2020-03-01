---
layout: default
---



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
