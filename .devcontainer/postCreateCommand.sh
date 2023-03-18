#!/bin/sh

# get submodules
git submodule init
git submodule update

# install npm packages
npm install

# build wasi-libc
(cd ./extern/wasi-libc && make)

# build mlang
cmake -B build -S .
cmake --build build