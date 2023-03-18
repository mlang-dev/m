"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * M supports the following control flow statements if then/else, for loop and while loop. The ternary operator
 * ? is also suported for brevity of if-else expression.
 */
const mtest_1 = require("./mtest");
(0, mtest_1.mtest)('if 1 statement', 'if 1 statement', "if 1 then 100 else 10", 100, false);
(0, mtest_1.mtest)('if 3 statement', 'if any non-zero statement is true', "if 3 then 100 else 10", 100, false);
(0, mtest_1.mtest)('if true statement', 'use true literal', "if true then 100 else 10", 100, false);
(0, mtest_1.mtest)('use bool variable', 'use bool variable', `
let x = 10
if x then 100 else 10
`, 100);
(0, mtest_1.mtest)('if but no else', 'only if but no else branch', `
var x = 10
if x==10 then 
    x = 100
x
`, 100);
(0, mtest_1.mtest)('ternary operator', 'use ternary operator', `
let x = 10
x ? 1 : 0
`, 1);
(0, mtest_1.mtest)('if 0 statement', 'if 0 statement', 'if 0 then 100 else 10', 10);
(0, mtest_1.mtest)('if false statement', 'use false literal', 'if false then 100 else 10', 10);
(0, mtest_1.mtest)('else if statement first', 'else if statement - choose first branch', `
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 10
`, 100);
(0, mtest_1.mtest)('else if statement second', 'else if statement - choose second branch', `
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 20
`, 200);
(0, mtest_1.mtest)('else if statement third', 'else if statement - choose third branch', `
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 30
`, 300);
(0, mtest_1.mtest)('else if statement else', 'else if statement - choose else branch', `
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 40
`, 400);
(0, mtest_1.mtest_strings)('if - else if print statement', 'if - else if print statement', `
let choice n = 
    if n <= 10 then 
        putchar '#' 
    else if n <= 20 then
        putchar '?'
    else
        putchar '*' 
choice 4
choice 14
choice 24
`, ['#', '?', '*'], false);
(0, mtest_1.mtest)('for loop statement', 'for loop statement', `
var sum = 0
for i in 0..5
    sum = sum + i
sum
`, 10);
(0, mtest_1.mtest)('for loop statement float', 'for loop statement using float type', `
var sum = 0.0
for i in 0.0..5.0
    sum = sum + i
sum
`, 10.0);
(0, mtest_1.mtest)('for loop with step', 'for loop statement with step variable', `
let step = 2
var sum = 0
for i in 2..step..10
    sum = sum + i
sum
`, 20);
(0, mtest_1.mtest)('nest for loop', 'nest for loop statement', `
var sum = 0
for i in 1..3
    for j in 1..3
        sum = sum + i * j
sum`, 9);
(0, mtest_1.mtest)('recursive factorial', 'implement factorial using recursive', `
let factorial n = if n == 1 then n else n * factorial (n-1)
factorial 5
`, 120);
(0, mtest_1.mtest)('for loop factorial', 'implement factorial using for loop', `
let factorial n = 
    var p = 1
    for i in 2..n+1
        p = p * i
    p
factorial 5
`, 120);
(0, mtest_1.mtest)('for loop break if', 'use break statement in for loop', `
for i in 0..10
    if i == 5 then 
        break
i
`, 5);
(0, mtest_1.mtest)('for loop break', 'use break statement in for-loop without using if-statement', `
for i in 0..10
    break
i
`, 0);
(0, mtest_1.mtest)('for loop break', 'use break statement in for-loop without using if-statement', `
for i in 0..10
    break
i
`, 0);
(0, mtest_1.mtest)('while loop', 'use while loop', `
var i = 0
while i < 10
    i = i + 1
i
`, 10);
(0, mtest_1.mtest)('while loop break', 'break while loop', `
var i = 0
while i < 10
    if i == 5 then
        break
    i = i + 1
i
`, 5);
(0, mtest_1.mtest)('while loop continue', 'use continue in while loop', `
var i = 0
var n = 0
while i < 5
    i = i + 1
    if i == 3 then
        continue
    n = n + i
n
`, 12);
(0, mtest_1.mtest)('for loop continue', 'use continue in for loop', `
var n = 0
for i in 0..5
    if i == 3 then
        continue
    n = n + i
n
`, 7);
(0, mtest_1.mtest)("mandelbrot set function", "various control block to show program structure", `
var a:u8[200][300 * 4]
let scale = 0.01
for x in 0..300
    for y in 0..200
        let cx = -2.0 + scale*(f64)x
        let cy = -1.0 + scale*(f64)y
        var zx = 0.0, zy = 0.0
        var zx2 = 0.0, zy2 = 0.0
        for n in 0..255
            if (zx2 + zy2) > 4.0 then
                break
            zy = 2.0 * zx * zy + cy
            zx = zx2  - zy2 + cx
            zx2 = zx * zx
            zy2 = zy * zy
        n = 255 - n
        a[y][4*x] = n
        a[y][4*x+1] = n
        a[y][4*x+2] = n
        a[y][4*x+3] = 255
setImageData a 300 200
`, null, false);
(0, mtest_1.mtest)("mandelbrot set function using while loop", "inner loop using while block to show program structure", `
var a:u8[200][300 * 4]
let scale = 0.01
for x in 0..300
    for y in 0..200
        let cx = -2.0 + scale*(f64)x
        let cy = -1.0 + scale*(f64)y
        var zx = 0.0, zy = 0.0
        var zx2 = 0.0, zy2 = 0.0
        var n = 0
        while n<255 && (zx2 + zy2) < 4.0
            zy = 2.0 * zx * zy + cy
            zx = zx2  - zy2 + cx
            zx2 = zx * zx
            zy2 = zy * zy
            n = n + 1
        n = 255 - n
        a[y][4*x] = n
        a[y][4*x+1] = n
        a[y][4*x+2] = n
        a[y][4*x+3] = 255
setImageData a 300 200
`, null, false);
//# sourceMappingURL=control.test.js.map