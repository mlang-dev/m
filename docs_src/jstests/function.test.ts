import { mtest } from './mtest';


mtest('generic function', 'generic function', 
`
def sq(x): x * x  // generic function
sq 10.0
`, 100.0);


mtest('return struct with var', 'return struct type with var', 
`
struct Point2D = x:mut f64, y:f64
def change(z:Point2D):
    z.x = z.x * 2.0
    z
let mut old_z = cf64{10.0, 20.0}
let new_z = change old_z
new_z.x
`, 20.0);

mtest('mandelbrot set function', 'mandelbrot set function',
`
def print_density(d:int) -> None:
  if d > 8 then
    putchar ' '
  else if d > 4 then
    putchar '.'
  else if d > 2 then
    putchar '+'
  else		
    putchar '*'
      
def sum_sq(z:cf64): z.re ** 2.0 + z.im ** 2.0
def ms(z:cf64 c:cf64): cf64{z.re ** 2.0 - z.im ** 2.0 + c.re, 2.0 * z.re * z.im + c.im}
def converger(z:cf64 iters:int c:cf64):
  if iters > 255 || (sum_sq z) > 4.0 then
    iters
  else
    converger (ms z c) (iters + 1) c
def converge(z:cf64): converger z 0 z
def print_ms(start:cf64 step:cf64):
  for y in start.im..step.im..start.im + step.im * 80.0
    for x in start.re..step.re..start.re + step.re * 78.0
       print_density (converge (cf64{x, y}))
    putchar '\n'
print_ms (cf64{-2.3, -1.3}) (cf64{0.05, 0.07})
(ms (cf64{10.0, 20.0}) (cf64{10.0, 20.0})).im
`, 400.0 + 20.0, false);

mtest('mandelbrot set function2', 'mandelbrot set plot function 2',
`
print "plot a mandelbrot set"

def color(iter_count iter_max:int sq_dist):
    let mut v = 0.0, r = 0.0, g = 0.0, b = 0.0
    if iter_count < iter_max then
        v = (log(iter_count+1.5-(log2((log(sq_dist))/2.0))))/3.4
        if v < 1.0 then 
            r = v ** 4;g = v ** 2.5;b = v
        else
            v = v < 2.0 ? 2.0 - v : 0.0
            r = v;g = v ** 1.5;b = v ** 3.0
    ((u8)(r * 255), (u8)(g * 255), (u8)(b * 255))

def plot_mandelbrot_set(x0:f64 y0:f64 x1:f64 y1:f64):
    let width = 400, height = 300
    let mut a:u8[height][width * 4]
    let scalex = (x1-x0)/width, scaley = (y1-y0)/height, max_iter = 510
    for x in 0..width
        for y in 0..height
            let cx = x0 + scalex*x
            let cy = y0 + scaley*y
            let mut zx = 0.0, zy = 0.0
            let mut zx2 = 0.0, zy2 = 0.0
            let mut n = 0
            while n<max_iter && (zx2 + zy2) < 4.0
                zy = 2.0 * zx * zy + cy
                zx = zx2  - zy2 + cx
                zx2 = zx * zx
                zy2 = zy * zy
                n++
            let cr, cg, cb = color n max_iter (zx2 + zy2)
            a[y][4*x] = cr
            a[y][4*x+1] = cg
            a[y][4*x+2] = cb
            a[y][4*x+3] = 255

    setImageData a width height

plot_mandelbrot_set (-2.0) (-1.2) 1.0 1.2
`, null, false);