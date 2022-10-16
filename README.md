[![CMake m](https://github.com/ligangwang/m/actions/workflows/cmake-app.yml/badge.svg)](https://github.com/ligangwang/m/actions/workflows/cmake-app.yml) [![CMake m WASM](https://github.com/ligangwang/m/actions/workflows/cmake-wasm.yml/badge.svg)](https://github.com/ligangwang/m/actions/workflows/cmake-wasm.yml)

# m (mlang)
mlang is a succinct & expressive general purpose programming language for WebAssembly development. It aims to support imperative and functional programming paradigms and is designed with zero-overhead abstraction. It's a static-typed language with type inference, which means most of time types of variables are not required to be annotated. 

The mlang is implemented in C without third party library dependency other than C standard libraries. The mlang can be compiled as a WebAssembly module so that the compiler itself can be run in the browser or other wasm runtime like nodejs and wastime etc. MacOS or Linux or WSL is the recommended development OS environement.

You can try [mlang](https://mlang.dev) in the browser.

## m code snippets
```
# hello world !
let main () = print "hello world !\n"

# comment line: defines a one-line function
let avg x y = (x + y) / 2

# defines a distance function
let distance x1 y1 x2 y2 = 
  xx = (x1-x2) ** 2.0
  yy = (y1-y2) ** 2.0
  sqrt (xx + yy) # call c std sqrt math function

# factorial function with recursive call
let factorial n = 
  if n < 2 then n
  else n * factorial (n-1)

# using for loop
let loopprint n = 
  for i in 0..n
    printf "%d" i   # call c std io printf function
```

## prerequisites to build m
* Source code version control: git
* Build system generator: cmake
* Build system: GNU make (Unix-like system)
* Compiler: c/c++ compilers: clang 13 or later

## get source codes
```
git clone https://github.com/ligangwang/m
cd m
git submodule init
git submodule update
```

## build & install wasi-libc
```
cd ./extern/wasi-libc
make (required: clang, llvm-ar, llvm-nm)
```

## build mlang as WebAssembly module
```
mkdir build
cd build
cmake -DWASM=On -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
cmake --build .
```
The build system will build mw.wasm under ./apps

Note to build m targeting LLVM backend, please refer to the [llvm build](./llvm.md) document.

# useful tools
## install clang-13 on ubuntu:
```
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo apt-add-repository "deb http://apt.llvm.org/focal/ llvm-toolchain-focal-13 main"
sudo apt install clang-13 lldb-13 lld-13 libclang-13-dev liblld-13-dev

sudo ln /usr/bin/clang-13 /usr/bin/clang
sudo ln /usr/bin/clang++-13 /usr/bin/clang++
sudo ln /usr/bin/llvm-ar-13 /usr/bin/llvm-ar
sudo ln /usr/bin/llvm-nm-13 /usr/bin/llvm-nm
sudo ln /usr/bin/llvm-ranlib-13 /usr/bin/llvm-ranlib
sudo ln /usr/bin/wasm-ld-13 /usr/bin/wasm-ld
sudo ln /usr/bin/llvm-config-13 /usr/bin/llvm-config
```

## compile c into wasm
```
clang --target=wasm32 --no-standard-libraries test.c -o test.wasm -mmultivalue -Xclang -target-abi -Xclang experimental-mv
```