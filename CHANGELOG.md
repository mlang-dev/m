# 0.0.39 (2022-12-13)
* support multiple statements in one line, separated by semicolon ';'

# 0.0.38 (2022-12-04)
* use variable pattern in pattern matching. e.g.
```
let pm x =
    match x with
    | -1 -> 100
    | 3 -> 200
    | y -> y + 10
```

# 0.0.37 (2022-12-03)

* start to add pattern match: e.g.
```
match x with 
| 0 -> 100
| 1 -> 200
| _ -> 10000
```
* enforce keyword "let" to define immutable variable ("var" for mutable variable)
* without "let" or "var" keyword, "x = 10" always will be interpreted as assignment statement. 


# 0.0.36 (2022-11-20)

* various memory leak fix.
* enforce mutability of variables, variables are all immutable by default except array types


# 0.0.35 (2022-11-17)

* start to use var/let to declare variable as mutable and immutable.


# 0.0.34 (2022-11-13)

* implicit casting types between int and float types
