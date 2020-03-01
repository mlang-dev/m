---
layout: default
---

## using m REPL:
```
./m
```

## using m compiler: 
```
./m ./samples/sample_lib.m
```

## c calls m functions:
```
clang++ ./samples/sample_main.cc ./samples/sample_lib.o ./runtime.o -o ./sample
```

