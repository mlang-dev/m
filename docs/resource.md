---
layout: default
---

## useful tools
* learn llvm IR

clang -S -emit-llvm ./examples/test_main.cc

* dump c/c++ header ast

clang -x c++ -Xclang -ast-dump -fsyntax-only ./include/runtime.h
