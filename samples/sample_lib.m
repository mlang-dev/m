# This is comment line
# Defines an average function
avg x y = (x + y) / 2

# This is a distance function
distance x1 y1 x2 y2 = 
  xx = (x1-x2) * (x1-x2)
  yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)

# factorial function
factorial n = 
  if n < 2 n
  else n * factorial (n-1)

# using for loop
loopprint n = 
  for i in 0..n
    print i

