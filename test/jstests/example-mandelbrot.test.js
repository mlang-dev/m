const mtest = require('./mtest.js');

mtest.mtest('mandelbrot set', 'draw mandelbrot set', 
`
var a:u8[200][300 * 4]
let scale = 0.01
for x in 0..300
    for y in 0..200
        let cx = -2.0 + scale*x
        let cy = -1.0 + scale*y
        var zx = 0.0, zy = 0.0
        var zx2 = 0.0, zy2 = 0.0
        var n = 0
        while n<510 && (zx2 + zy2) < 4.0
            zy = 2.0 * zx * zy + cy
            zx = zx2  - zy2 + cx
            zx2 = zx * zx
            zy2 = zy * zy
            n++
        a[y][4*x] = n < 256 ? 0 : 510 - n
        a[y][4*x+1] = n < 256 ? n : 510 - n
        a[y][4*x+2] = n < 256 ? 0 : 510 - n
        a[y][4*x+3] = 255

setImageData a 300 200
`, undefined);

