#! /usr/bin/bash
#curl https://raw.githubusercontent.com/mlang-dev/c2m/main/mlib/math.m > ./src/sys/math.m
#curl https://raw.githubusercontent.com/mlang-dev/c2m/main/mlib/stdio.m > ./src/sys/stdio.m
#curl https://raw.githubusercontent.com/mlang-dev/c2m/main/mlib/stdlib.m > ./src/sys/stdlib.m

curl -L https://github.com/bytecodealliance/wasm-tools/releases/download/v1.201.0/wasm-tools-1.201.0-x86_64-linux.tar.gz -o ./wasm-tools-1.201.0-x86_64-linux.tar.gz
tar -xzvf ./wasm-tools-1.201.0-x86_64-linux.tar.gz -C ./dep wasm-tools-1.201.0-x86_64-linux/wasm-tools --strip-components 1
rm ./wasm-tools-1.201.0-x86_64-linux.tar.gz