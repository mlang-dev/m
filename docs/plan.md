---
layout: default
---

# m
m is a succinct & expressive programming language, where efficiency and elegance meet. It aims to support imperative and functional programming paradigms and is designed with zero-overhead abstraction whenever possible.

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
  
  ### next development plans:
- debug support
- operators
  - binary operator: **
- heap types
  - string
  - tuple
  - option
  - array/list
  - queue/dequeue
  - map/set
  - RAII memory management
- wasm target support
- code in browser
- multiple modules
- pattern matching
  - _: anything
  - x: anything (binding x to the value)
  - 10: integer 10 
  - false: the boolean value false
  - (pat1, pat2): a pair (v1, v2) if v1 matches pat1 and v2 matches pat2
  - (x, _): matches (true, 100), binding x to true
  - None: matches None of any option type
  - Some pat: matches Some v if pat matchs the v
  - pat1::pat2
- IDE support
