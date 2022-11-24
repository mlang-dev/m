const mtest = require('./mtest.js');


mtest.mtest('generic function', 'generic function', 
`
let sq x = x * x  // generic function
sq 10.0
`, 100.0);


mtest.mtest('return struct with var', 'return struct with var', 
`
struct Point2D = var x:f64, y:f64
let change z:Point2D = 
    z.x = z.x * 2.0
    z
old_z = cf64{10.0, 20.0}
new_z = change old_z
new_z.x
`, 20.0);


mtest.mtest('mandelbrot set function', 'mandelbrot set function',
`
let print_density:() d:int =
  if d > 8 then
    putchar ' '
  else if d > 4 then
    putchar '.'
  else if d > 2 then
    putchar '+'
  else		
    putchar '*'
      
let sum_sq z:cf64 = z.re ** 2.0 + z.im ** 2.0
let ms z:cf64 c:cf64 = cf64{z.re ** 2.0 - z.im ** 2.0 + c.re, 2.0 * z.re * z.im + c.im}
let converger z:cf64 iters:int c:cf64 = 
  if iters > 255 || (sum_sq z) > 4.0 then
    iters
  else
    converger (ms z c) (iters + 1) c
let converge z:cf64 = converger z 0 z
let print_ms start:cf64 step:cf64 =
  for y in start.im..step.im..start.im + step.im * 80.0
    for x in start.re..step.re..start.re + step.re * 78.0
       print_density (converge (cf64{x, y}))
    putchar '\n'
print_ms (cf64{-2.3, -1.3}) (cf64{0.05, 0.07})
(ms (cf64{10.0, 20.0}) (cf64{10.0, 20.0})).im
`, 400.0 + 20.0, false);