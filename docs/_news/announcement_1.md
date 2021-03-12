---
layout: post
title: about m(mlang)
date: 2020-02-01 16:11:00-0500
inline: false
---

m is a succinct & expressive programming language, where elegant language construction is achieved without sacrificing C level of execution efficiency.

***

It is under active development, currently it supports following features

- c ABI
  - c interoperability: calling c & being called from c

- arithmetic operators
  - \+    addition
  - \-    subtraction
  - \*    multiplication
  - /     division
  - %     remainder

- relational operators:
  - == equality
  - \>  greater than
  - \>= greater than or equals
  - \<  less than
  - \<= less than or equals
  - \!= not equal

- support types:
  - unit
  - bool
  - char
  - int
  - double
  - string (basic stack-based)

- logical operators:
  - \|\| or
  - && and
  - ! not

- control flows
  - if..then..else
  - for loop

- operator overload
  for examples
  - binary: ```(|>) x f = f x```
  - unary: ```(!) x = 0 - x```
  
