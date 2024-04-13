#!/bin/sh

# get submodules
git submodule init
git submodule update

# install npm packages
npm install

(
    cd ./extern
    export WASI_VERSION=20
    export WASI_VERSION_FULL=${WASI_VERSION}.0
    wget https://github.com/WebAssembly/wasi-sdk/releases/download/wasi-sdk-${WASI_VERSION}/wasi-sdk-${WASI_VERSION_FULL}-linux.tar.gz
    tar xvf wasi-sdk-${WASI_VERSION_FULL}-linux.tar.gz
)


# build mlang
cmake -B build -S .
cmake --build build