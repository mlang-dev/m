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
make sure CPATH env variable is set to folder where c standard headers are found. e.g. /usr/include or /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include
```
export CPATH=/usr/include
mkdir build
cd build
cmake ..
make
```