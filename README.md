[![CMake m](https://github.com/ligangwang/m/actions/workflows/cmake-m.yml/badge.svg)](https://github.com/ligangwang/m/actions/workflows/cmake-m.yml) 

# m (mlang)
A general purpose static-typed programming language with type inference written in C and can be compiled as a WebAssembly module.

mlang is still in early development phase and subject to significant changes.
 
WSL or Ubuntu 22.04 is the recommended development OS, and clang/llvm-14 is the compiler for mlang development.

## m code plotting mandelbrot set 
You can try [mlang](https://mlang.dev) to run the following m code in the browser.
The code will yield the following image:

![mandelbrot set](./docs_src/mandelbrotset.png)

You can zoom in to see more details at [https://mlang.dev](https://mlang.dev).

<img src="./docs_src/mandelbrotset_demo.gif" width="400" height="320"/>

```
//color function returns (r, g, b) tuple based on iteration count and distance
let color iter_count:int iter_max:int sq_dist:f64 =
    var v = 0.0, r = 0.0, g = 0.0, b = 0.0
    if iter_count < iter_max then
        v = (log(iter_count+1.5-(log2((log(sq_dist))/2.0))))/3.4
        if v < 1.0 then 
            r = v ** 4;g = v ** 2.5;b = v
        else
            v = v < 2.0 ? 2.0 - v : 0.0
            r = v;g = v ** 1.5;b = v ** 3.0
    ((u8)(r * 255), (u8)(g * 255), (u8)(b * 255))

/* main plot function
x0, y0: coordinate value of top left
x1, y1: coordinate value of bottom right
*/
let plot_mandelbrot_set x0:f64 y0:f64 x1:f64 y1:f64 =
    print "plot area: x0:%f y0:%f x1:%f y1:%f\\n" x0 y0 x1 y1
    let width = 400, height = 300
    var img:u8[height][width * 4]
    let scalex = (x1-x0)/width, scaley = (y1-y0)/height, max_iter = 510
    for x in 0..width
        for y in 0..height
            let cx = x0 + scalex*x
            let cy = y0 + scaley*y
            var zx = 0.0, zy = 0.0
            var zx2 = 0.0, zy2 = 0.0
            var n = 0
            while n<max_iter && (zx2 + zy2) < 4.0
                zy = 2.0 * zx * zy + cy
                zx = zx2  - zy2 + cx
                zx2 = zx * zx
                zy2 = zy * zy
                n++
            let cr, cg, cb = color n max_iter (zx2 + zy2)
            img[y][4*x] = cr
            img[y][4*x+1] = cg
            img[y][4*x+2] = cb
            img[y][4*x+3] = 255

    setImageData img width height /*call js to set img data on canvas*/
// call main plot function
plot_mandelbrot_set (-2.0) (-1.2) 1.0 1.2
```

# Development: vscode dev container (no manual installation)
https://user-images.githubusercontent.com/6968989/226146999-12b037e6-45e1-48e3-80e9-17282a2361ba.mp4

# or manually install tool-chains on Ubuntu-22.04 
## prerequisites to build m
* Source code version control: git
* Build system generator: cmake 
* Build system: GNU make (Unix-like system)
* Compiler: c/c++ compilers: clang/llvm 14, nodejs v18.15

## install cmake/clang/llvm/lld 
```
sudo apt update
sudo apt install cmake
sudo apt install clang lld llvm
```

## install nodejs v18.15 and typescript
### install nvm first
```
sudo apt install curl -y
curl https://raw.githubusercontent.com/creationix/nvm/master/install.sh | bash 
source ~/.profile
```

### use nvm to install node with version
```
nvm install v18.15.0
npm install -g typescript
```

# build m from source code
## get m source code
```
git clone https://github.com/ligangwang/m
cd m
git submodule init
git submodule update
npm install
```

## build & install wasi-libc
```
(cd ./extern/wasi-libc && make)
```

## build mlang
```
cmake -B build -S .
cmake --build build
```
The build system will build m and mw.wasm under ./apps and run all unit tests.


# useful clang commands to compile to wasm/wasi target
## compile c code to wasm target
```
clang --target=wasm32 --no-standard-libraries test.c -o test.wasm -mmultivalue -Xclang -target-abi -Xclang experimental-mv
```

## compile c into wasi target
```
clang --target=wasm32-wasi --sysroot=../extern/wasi-libc/sysroot hello.c -o hello.wasm -nodefaultlibs -lc -v
```
or manually invoke clang front-end and back-end as two steps:

use front-end clang to produce wasm object file
```
clang -cc1 -triple wasm32-unknown-wasi -emit-obj -internal-isystem /usr/lib/llvm-14/lib/clang/14.0.0/include -internal-isystem ./extern/wasi-libc/sysroot/include -o hello.o -x c hello.c
```

then use back-end wasm-ld to produce wasm module
```
wasm-ld -m wasm32 -L./extern/wasi-libc/sysroot/lib/wasm32-wasi ./extern/wasi-libc/sysroot/lib/wasm32-wasi/crt1-command.o hello.o -lc -o hello.wasm
```

