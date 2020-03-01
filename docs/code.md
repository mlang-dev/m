---
layout: default
---

## m syntax
```
# comment line: defines a one-line function
avg x y = (x + y) / 2

# defines a distance function
distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)

# factorial function with recursive call
factorial n = 
  if n < 2 n
  else n * factorial (n-1)

# using for loop
loopprint n = 
  for i in 0..n
    print i
```