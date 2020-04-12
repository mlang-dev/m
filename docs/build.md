---
layout: default
---


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