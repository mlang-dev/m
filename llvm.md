# build m targeting LLVM 

## build clang/llvm
Note: You can skip building llvm from source but just install binary (version 13.0) from [llvm site](https://releases.llvm.org/). 

Replace "Debug" with "Release" in following commands if Release build is desired.
```
cd ./extern
git clone https://github.com/llvm/llvm-project
cd ./llvm-project
git checkout release/13.x
mkdir build
cd build
cmake -DLLVM_ENABLE_RTTI=ON -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS="clang;lld" ../llvm
sudo cmake --build . --config Debug -j NN --target install   (Run under Administrator, NN - number of CPU (cores) that you have)
cd ../../../
```
On Windows, llvm binary folder(C:\Program Files (x86)\LLVM\bin) needs to be added to the Path environment variable so that "llvm-config" command is able to be executed in cmake. 

## build m
```
mkdir llvm_build
cd llvm_build
cmake ..
cmake --build .
```
The build system will build m executable under ./tools on macOS/Linux, or .\tools\Debug on Windows Note: To use clang on windows, use command: cmake -G"Visual Studio 16 2019" -T ClangCL ..

## using m compiler
```
./tools/m ./samples/sample_lib.m
```
The above command using m compiler generates an object file "sample_lib.o" under the same folder, which will be used to be compiled with C code in the following example.

## c calls m functions
```
clang++ ./samples/sample_main.cc ./samples/sample_lib.o ./runtime.o -o ./sample
```

## re-generate c sys file to m
./build/apps/c2m -i /usr/include/ -o ./mlib stdio.h
./build/apps/c2m -i /usr/include/ -o ./mlib math.h
