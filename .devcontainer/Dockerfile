FROM mcr.microsoft.com/devcontainers/base:jammy
RUN sudo echo "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-18 main" | sudo tee /etc/apt/sources.list.d/docker.list
RUN wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc
RUN sudo apt-get update
RUN sudo apt-get --assume-yes install clang-18 lld-18 llvm-18 libmlir-18-dev mlir-18-tools
RUN sudo apt-get --assume-yes install gdb

RUN sudo apt-get --assume-yes install cmake
RUN curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash 
RUN sudo apt-get --assume-yes install -y nodejs
RUN npm install -g typescript
