/**
 * struct and array are aggregate types in m. Their data are stored on the stack by default. M uses call-by-value convention to pass arguments
 * to the called function. Struct type in m is a value type. When the passed argument is struct type, the struct data is duplicated and 
 * its reference/address is passed to the function. This is to prevent the struct value is being changed by the callee.
 * 
 * Array type is a reference type in m. When you assign an array object to a variable, only its address is assigned to the variable. The operation is 
 * super efficlient and this is in contrast to struct type assignment, where its contents are copied to the assigned variable.
 */

const mtest = require('./mtest.js');

mtest.mtest('cf64 re', `
complex cf64 is a builtin struct type defined as "struct cf64 = re:f64, im:f64", the example below shows
how to initialize a struct type variable and how to access field of the struct type variable.
`, 
`
z = cf64(10.0, 20.0)
z.re
`, 10.0);

mtest.mtest('struct with different types', `
use keyword "struct" to define a struct type`,
`
struct A = x:int, y:f64
a = A(10, 20.0)
a.y`, 20.0);

mtest.mtest('access field no var', 
`access struct initializer's field without using a variable`,
`
struct A = x:int, y:f64
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
let add z:cf64 op:f64 = cf64(z.re + op, z.im + op)
x = cf64(10.0, 20.0)
(add x 10.0).im
`, 30.0);

mtest.mtest('pass return struct no variable',
`pass a struct and return a new struct the caller without any temp variable`,
`
let add z:cf64 op:f64 = cf64(z.re + op, z.im + op)
(add (cf64(10.0, 20.0)) 10.0).re
`, 20.0);

mtest.mtest('struct member assign struct', 'struct member assign struct',
`
struct xy = x:f64, y:f64
struct wz = w:f64, z:xy
ab = wz(10.0, xy(20.0, 30.0))
ab.z = xy(200.0, 300.0)
ab.z.y
`, 300.0);

mtest.mtest('variable scope', 'variable scope', 
`
struct wz = w:f64, z:f64
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


mtest.mtest('one element array init', 
`
initialize a 1 length of int array
`, 
`
a = [10]
a[0]
`, 10);

mtest.mtest('more array element', 
`
initialize a two elements of int array
`, 
`
a = [10, 20]
a[0]
`, 10);

mtest.mtest('sum of elements', 
`
sum a two elements of int array
`, 
`
a = [10, 20]
a[0] + a[1]
`, 30);

mtest.mtest('update array element', 
`
update the first element of an array
`, 
`
a = [10]
a[0] = 20
a[0]
`, 20);

mtest.mtest('update second element', 
`
update the second element of an array
`, 
`
a = [10,20]
a[1] = 200
a[0] + a[1]
`, 210);

mtest.mtest('u8 array', 
`
declare a u8 array
`, 
`
a:u8[2] = [10, 20]
a[0] + a[1]
`, 30);

mtest.mtest('u8 array update', 
`
declare a u8 array
`, 
`
a:u8[2] = [10, 20]
a[0] = 30
a[1] = 40
a[0] + a[1]
`, 70);

mtest.mtest('u8 two dimension array', 
`
declare a u8 two dimensions array
`, 
`
a:u8[2][2] = [10, 20, 30, 40]
a[1][1]
`, 40);

mtest.mtest('u8 two dimen update', 
`
update two dimensions array
`, 
`
a:u8[2][2] = [10, 20, 30, 40]
a[1][1] = 100
a[1][1]
`, 100);

mtest.mtest('u8 access via var', 
`
update two dimensions array using variable
`, 
`
a:u8[10][10]
for i in 0..10
    for j in 0..10
        a[i][j] = i + j
a[7][8]
`, 15);

mtest.mtest('u32 access via var', 
`
update two dimensions int array using variable
`, 
`
a:u32[10][10]
for i in 0..10
    for j in 0..10
        a[i][j] = i + j
a[7][8]
`, 15);

mtest.mtest('pass array to a function', 
`
pass array variable to a function.
`, 
`
a:u8[10][10]
for i in 0..10
    for j in 0..10
        a[i][j] = i + j
let pick arr:u8[10][10] x:int y:int = arr[x][y]
pick a 9 8
`, 17);
