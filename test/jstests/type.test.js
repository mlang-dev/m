const mtest = require('./mtest.js');

// mtest.mtest('u8 to f32', 'cast u8 to f32', 
// `
// x:u8 = 255
// y = (f32)x
// `, 255, false);

mtest.mtest('int to f64', 'cast int to f64', 
`
x = 1000
(f64)x * 2.0`, 2000.0, false);

mtest.mtest('cast to int', 'cast f64 to int', 
`
x = 1000
x * (int)2.0`, 2000, false);
