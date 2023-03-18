FROM mcr.microsoft.com/devcontainers/base:jammy
RUN sudo apt-get update
RUN sudo apt-get --assume-yes install cmake
RUN sudo apt-get --assume-yes install clang lld llvm
RUN curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash 
RUN sudo apt-get --assume-yes install -y nodejs

RUN npm install -g typescript
