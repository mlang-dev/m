import { mtest } from './mtest';

mtest('int to u8 loss', 'down cast int to u8, losing values', 
`
let x:u8 = 256
x
`, 0, false);

mtest('int to u8 lossless', 'down cast int to u8, preserving the value', 
`
let x:u8 = 255
x
`, 255, false);

mtest('int to i8', 'down cast int to i8, turning into negative', 
`
let x:i8 = 128
x
`, -128, false);

mtest('int to i8', 'down cast int to i8, -1', 
`
let x:i8 = 255
x
`, -1, false);

mtest('i8 to i32', 'up cast i8 to i32', 
`
let x:i8 = -1
(i32)x
`, -1, false);

mtest('i8 to u32', 'up cast i8 to u32', 
`
let x:i8 = -1
(u32)x
`, -1, false);

mtest('u8 to i32', 'up cast u8 to i32', 
`
let x:u8 = 255
(i32)x
`, 255, false);

mtest('u8 to f32', 'cast u8 to f32', 
`
let x:u8 = 255
let y = (f32)x
y
`, 255, false);

mtest('int to f64', 'cast int to f64', 
`
let x = 1000
(f64)x * 2.0`, 2000.0, false);

mtest('cast to int', 'cast f64 to int', 
`
let x = 1000
x * (int)2.0`, 2000, false);

mtest('int multi float', 'multiply float by int', 
`
let scale = 0.01
scale * 10`, 0.1, false);


mtest('int float literal', 'multiply int by float literal', 
`
10 * 0.01
`, 0.1, false);

mtest('float int literal', 'multiply float by int literal', 
`
0.01 * 10
`, 0.1, false);

