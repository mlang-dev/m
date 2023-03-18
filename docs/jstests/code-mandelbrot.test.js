"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const mtest_1 = require("./mtest");
(0, mtest_1.mtest)('mandelbrot set linear', 'draw mandelbrot set using linear color function', `
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
`, null);
(0, mtest_1.mtest)('mandelbrot set nonlinear', 'draw mandelbrot set using log color function', `
var a:u8[200][300 * 4]
let scale = 0.01, max_iter = 510
var v = 0.0, r = 0.0, g = 0.0, b = 0.0
for x in 0..300
    for y in 0..200
        let cx = -2.0 + scale*x
        let cy = -1.0 + scale*y
        var zx = 0.0, zy = 0.0
        var zx2 = 0.0, zy2 = 0.0
        var n = 0
        while n<max_iter && (zx2 + zy2) < 4.0
            zy = 2.0 * zx * zy + cy
            zx = zx2  - zy2 + cx
            zx2 = zx * zx
            zy2 = zy * zy
            n++
        if n < max_iter then
            v = (log(n+1.5-(log2((log(zx2+zy2))/2.0))))/3.4
            if v < 1.0 then 
                r = v ** 4;g = v ** 2.5;b = v
            else
                v = v < 2.0 ? 2.0 - v : 0.0
                r = v;g = v ** 1.5;b = v ** 3.0
        a[y][4*x] = n == max_iter ? 0 : (u8)(r * 255)
        a[y][4*x+1] = n == max_iter ? 0 : (u8)(g * 255)
        a[y][4*x+2] = n == max_iter ? 0 : (u8)(b * 255)
        a[y][4*x+3] = 255

setImageData a 300 200
`, null);
(0, mtest_1.mtest)('julia set 1', 'julia set c = -0.79 + 0.15i', `
let width = 300, height = 200
var a:u8[200][300 * 4]
let scalex = 4.0/300.0, scaley=2.0/200.0, max_iter = 510
var v = 0.0, r = 0.0, g = 0.0, b = 0.0
let cx = -0.79
let cy = 0.15
for x in 0..300
    for y in 0..200
        var zx = -2.0 + scalex * x, zy = -1.0 + scaley * y
        var zx2 = zx * zx, zy2 = zy * zy
        var n = 0
        while n<max_iter && (zx2 + zy2) < 4.0
            zy = 2.0 * zx * zy + cy
            zx = zx2  - zy2 + cx
            zx2 = zx * zx
            zy2 = zy * zy
            n++
        if n < max_iter then
            v = (log(n+1.5-(log2((log(zx2+zy2))/2.0))))/3.4
            if v < 1.0 then 
                r = v ** 4;g = v ** 2.5;b = v
            else
                v = v < 2.0 ? 2.0 - v : 0.0
                r = v;g = v ** 1.5;b = v ** 3.0
        a[y][4*x] = n == max_iter ? 0 : (u8)(r * 255)
        a[y][4*x+1] = n == max_iter ? 0 : (u8)(g * 255)
        a[y][4*x+2] = n == max_iter ? 0 : (u8)(b * 255)
        a[y][4*x+3] = 255

setImageData a 300 200
`, null);
(0, mtest_1.mtest)('julia set 2', 'julia set c = 0.3 - 0.01i', `
let width = 300, height = 200
var a:u8[200][300 * 4]
let scalex = 4.0/300.0, scaley=3.0/200.0, max_iter = 510
var v = 0.0, r = 0.0, g = 0.0, b = 0.0
let cx = 0.3
let cy = -0.01
for x in 0..300
    for y in 0..200
        var zx = -2.0 + scalex * x, zy = -1.5 + scaley * y
        var zx2 = zx * zx, zy2 = zy * zy
        var n = 0
        while n<max_iter && (zx2 + zy2) < 4.0
            zy = 2.0 * zx * zy + cy
            zx = zx2  - zy2 + cx
            zx2 = zx * zx
            zy2 = zy * zy
            n++
        if n < max_iter then
            v = (log(n+1.5-(log2((log(zx2+zy2))/2.0))))/3.4
            if v < 1.0 then 
                r = v ** 4;g = v ** 2.5;b = v
            else
                v = v < 2.0 ? 2.0 - v : 0.0
                r = v;g = v ** 1.5;b = v ** 3.0
        a[y][4*x] = n == max_iter ? 0 : (u8)(r * 255)
        a[y][4*x+1] = n == max_iter ? 0 : (u8)(g * 255)
        a[y][4*x+2] = n == max_iter ? 0 : (u8)(b * 255)
        a[y][4*x+3] = 255

setImageData a 300 200
`, null);
//# sourceMappingURL=code-mandelbrot.test.js.map