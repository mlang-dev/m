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

## build llvm/clang
```
cd ./extern/llvm-project
mkdir build
cd build
cmake -DLLVM_ENABLE_RTTI=ON -DLLVM_ENABLE_PROJECTS=clang ../llvm
cmake --build . -j NN    NN - number of CPU (cores) that you have
cmake --build . --target install 
cd ../../../
```

## build mlang
make sure MPATH env variable is set to the src folder of mlang e.g. /Users/lwang/dev/m/src
```
export CPATH=/Users/lwang/dev/m/src
mkdir build
cd build
cmake ..
make
```