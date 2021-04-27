---
layout: page
title: about
permalink: /about/
description: 
nav: true
---

m is a succinct & expressive programming language, where elegant language construction is achieved without sacrificing C level of execution efficiency. It aims to support imperative and functional programming paradigms and is designed with zero-overhead abstraction to its maximum. It's a static-typed language with type inference, which means most of time types of variables are not required to be annotated. m supports native C data type without introducing reference count and GC overheads like Python and other high-level languages do. Like C, m supports memory allocations on stack and heap, the stack memory is automatically released when it is out of scope. Unlike C the heap memory release instruction is automatically inserted where its owner on the stack is out of scope statically at compile time by m, and m programmer does not need to explicitly release heap memory. This design avoids the non-deterministic garbage collections and is tailored for real-time system programming.

Another goal of m language design is to eanble native calls between functions written in m and C language to maximize reuse of existing high performance libraries written in C/C++.

In summary the followings are design characteristics of m:
* Static typed with type inference
* Generic
* Polymorphic Composition and Interface
* Memory management with ownership
* Succinct and mathmatics-friendly language constructions
* Zero-overhead abstraction
* Imperative & functional paradigms

The mlang is written in C but requires C++ linker to build due to llvm being implemented in C++. 

The code is still under developement but is able to be compiled on Windows/macOS/Linux, which has been tested on following platforms:
* macOS 10.15.7
* Ubuntu 20.04
* Windows 10

