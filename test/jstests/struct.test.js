/** hello world struct type */

const mtest = require('./mtest.js');

mtest.mtest('cf64 re', `
complex cf64 is a builtin struct type defined as "struct cf64 = re:double, im:double", the example below shows
how to initialize a struct type variable and how to access field of the struct type variable.
`, 
`
z = cf64(10.0, 20.0)
z.re
`, 10.0);

mtest.mtest('struct with different types', `
use keyword "struct" to define a struct type`,
`
struct A = x:int, y:double
a = A(10, 20.0)
a.y`, 20.0);

mtest.mtest('access field no var', 
`access struct initializer's field without using a variable`,
`
struct A = x:int, y:double
A(10, 20.0).y
`, 20.0);

mtest.mtest('struct in struct', 
`
Struct could be nested. Here we embed struct cf64 inside struct AB.
`, 
`
struct AB = a:cf64, b:cf64
ab = AB(cf64(10.0, 20.0), cf64(30.0, 40.0))
ab.b.im
`, 40.0);

mtest.mtest('return struct in struct', 
`
return a nesting struct from a function
`, 
`
struct AB = a:cf64, b:cf64
let get () = AB(cf64(10.0, 20.0), cf64(30.0, 40.0))
get().b.im
`, 40.0);


mtest.mtest('return field from struct',
`
We define struct AB as two fields, each of which is also a cf64 struct. The following code is 
initializing a struct AB object and return one of field a to the caller.
`, 
`
struct AB = a:cf64, b:cf64
let get () = 
    ab = AB(cf64(10.0, 20.0), cf64(30.0, 40.0))
    ab.a
get().im
`, 20.0);

mtest.mtest('return field struct', 
`
One more time, but this time we eliminate a variable in the function.
`, 
`
struct AB = a:cf64, b:cf64
let get () = AB(cf64(10.0, 20.0), cf64(30.0, 40.0)).a
get().re
`, 10.0);

mtest.mtest('pass return struct', 
`pass a struct and return a new struct to the caller then print the field of returned struct`,
`
let add z:cf64 op:double = cf64(z.re + op, z.im + op)
x = cf64(10.0, 20.0)
(add x 10.0).im
`, 30.0);

mtest.mtest('pass return struct no variable',
`pass a struct and return a new struct the caller without any temp variable`,
`
let add z:cf64 op:double = cf64(z.re + op, z.im + op)
(add (cf64(10.0, 20.0)) 10.0).re
`, 20.0);

mtest.mtest('struct member assign struct', 'struct member assign struct',
`
struct xy = x:double, y:double
struct wz = w:double, z:xy
ab = wz(10.0, xy(20.0, 30.0))
ab.z = xy(200.0, 300.0)
ab.z.y
`, 300.0);

mtest.mtest('variable scope', 'variable scope', 
`
struct wz = w:double, z:double
z = wz(10.0, 20.0)
z.z
`, 20.0);


mtest.mtest('cf64 expr', `
use field of struct in expression.
`, 
`
z = cf64(10.0, 20.0)
z.re + z.im
`, 30.0);

mtest.mtest('return struct', `
Here we define a function returning a struct type cf64 and assign it to variable x.
`, 
`
let z() = cf64(10.0, 20.0)
x = z()
x.re + x.im
`, 30.0);


mtest.mtest('return struct no var', `
We can directly access return of function call without assigning a variable.
`, 
`
let z() = cf64(10.0, 20.0)
z().im
`, 20.0);

mtest.mtest('pass struct', `
We can pass struct argument to a function.
`, 
`
let im z:cf64 = z.im
x = cf64(10.0, 20.0)
im x
`, 20.0);

mtest.mtest('pass struct no var', `
We can pass struct argument directly without a variable to a function.
`, 
`
let im z:cf64 = z.im
im (cf64(10.0, 20.0))
`, 20.0);

mtest.mtest('pass struct add one value', `
Pass the struct data, and return with expression using one field
`, 
`
let im z:cf64 = 
    z.im + 200.0
im (cf64(10.0, 20.0))
`, 220.0);

mtest.mtest('pass struct add value', `
Pass the struct data, and return with a struct with new value
`, 
`
let shift z:cf64 = 
    cf64(z.re + 100.0, z.im + 200.0)
result = shift (cf64(10.0, 20.0))
result.re + result.im
`, 110.0 + 220.0);


mtest.mtest('complex addition', 'complex addition', 
`
let add_c a:cf64 b:cf64 = cf64(a.re + b.re, a.im + b.im)
(add_c (cf64(10.0, 20.0)) (cf64(30.0, 40.0))).im
`, 60.0, false);

mtest.mtest('complex exponent', 'complex exponent',
`
let sq z:cf64 = cf64(z.re ** 2.0 - z.im ** 2.0, 2.0 * z.re * z.im)
(sq (cf64(10.0, 20.0))).im
`, 400.0, false);

mtest.mtest('struct member assign', 'struct member assign',
`
z = cf64(10.0, 20.0)
z.re = 30.0
z.re
`, 30.0);
