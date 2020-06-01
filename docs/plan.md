---
layout: default
---

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
