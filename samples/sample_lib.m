// This is comment line
// Defines an average function
def avg(x y) = (x + y) / 2.0

// This is a distance function
def distance(x1 y1 x2 y2) = 
  let xx = (x1-x2) * (x1-x2)
  let yy = (y1-y2) * (y1-y2)
  sqrt (xx + yy)

// factorial function
def factorial(n) = 
  if n < 2 then n
  else n * factorial (n-1)

// using for loop
def loopprint(n) = 
  for i in 0..n
    printf "%d\n" i
