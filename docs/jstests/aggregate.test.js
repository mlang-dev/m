"use strict";
/**
 * record, variant and array are aggregate types in m. Their data are stored on the stack by default. M uses call-by-value convention to pass arguments
 * to the called function. Record type in m is a value type. When the passed argument is record type, the record data is duplicated and
 * its reference/address is passed to the function. This is to prevent the record value is being changed by the callee.
 *
 * Array type is a reference type in m. When you assign an array object to a variable, only its address is assigned to the variable. The operation is
 * super efficlient and this is in contrast to record type assignment, where its contents are copied to the assigned variable.
 */
Object.defineProperty(exports, "__esModule", { value: true });
const mtest_1 = require("./mtest");
(0, mtest_1.mtest)('cf64 re', `
complex cf64 is a builtin record type defined as "record cf64 = re:f64, im:f64", the example below shows
how to initialize a record type variable and how to access field of the record type variable.
`, `
let z = cf64 { 10.0, 20.0 }
z.re
`, 10.0);
(0, mtest_1.mtest)('record with different types', `
use keyword "record" to define a record type`, `
record A = x:int, y:f64
let a = A { 10, 20.0 }
a.y`, 20.0);
(0, mtest_1.mtest)('variant with different types', `
use keyword "variant" to define a variant (tagged union) type`, `
variant A = x:int | y:int
let a = A { 10 }
a.y`, 10);
(0, mtest_1.mtest)('access field no var', `access record initializer's field without using a variable`, `
record A = x:int, y:f64
A { 10, 20.0 }.y
`, 20.0);
(0, mtest_1.mtest)('record in record', `
Struct could be nested. Here we embed record cf64 inside record AB.
`, `
record AB = a:cf64, b:cf64
let ab = AB {cf64{10.0, 20.0}, cf64 { 30.0, 40.0 }}
ab.b.im
`, 40.0);
(0, mtest_1.mtest)('return record in record', `
return a nesting record from a function
`, `
record AB = a:cf64, b:cf64
let get () = AB{cf64{10.0, 20.0}, cf64{30.0, 40.0}}
get().b.im
`, 40.0);
(0, mtest_1.mtest)('return field from record', `
We define record AB as two fields, each of which is also a cf64 record. The following code is 
initializing a record AB object and return one of field a to the caller.
`, `
record AB = a:cf64, b:cf64
let get () = 
    let ab = AB{cf64{10.0, 20.0}, cf64{30.0, 40.0}}
    ab.a
get().im
`, 20.0);
(0, mtest_1.mtest)('return field record', `
One more time, but this time we eliminate a variable in the function.
`, `
record AB = a:cf64, b:cf64
let get () = AB{cf64{10.0, 20.0}, cf64{30.0, 40.0}}.a
get().re
`, 10.0);
(0, mtest_1.mtest)('pass return record', `pass a record and return a new record to the caller then print the field of returned record`, `
let add z:cf64 op:f64 = cf64{z.re + op, z.im + op}
let x = cf64{10.0, 20.0}
(add x 10.0).im
`, 30.0);
(0, mtest_1.mtest)('pass return record no variable', `pass a record and return a new record the caller without any temp variable`, `
let add z:cf64 op:f64 = cf64 { z.re + op, z.im + op }
(add (cf64 { 10.0, 20.0 }) 10.0).re
`, 20.0);
(0, mtest_1.mtest)('record member assign record', 'record member assign record', `
record xy = x:f64, y:f64
record wz = w:f64, z:mut xy
let ab = wz{10.0, xy{20.0, 30.0}}
ab.z = xy{200.0, 300.0}
ab.z.y
`, 300.0);
(0, mtest_1.mtest)('variable scope', 'variable scope', `
record wz = w:f64, z:f64
let z = wz { 10.0, 20.0 }
z.z
`, 20.0);
(0, mtest_1.mtest)('cf64 expr', `
use field of record in expression.
`, `
let z = cf64 { 10.0, 20.0 }
z.re + z.im
`, 30.0);
(0, mtest_1.mtest)('return record', `
Here we define a function returning a record type cf64 and assign it to variable x.
`, `
let z() = cf64 { 10.0, 20.0 }
let x = z()
x.re + x.im
`, 30.0);
(0, mtest_1.mtest)('return record no var', `
We can directly access return of function call without assigning a variable.
`, `
let z() = cf64 { 10.0, 20.0 }
z().im
`, 20.0);
(0, mtest_1.mtest)('pass record', `
We can pass record argument to a function.
`, `
let im z:cf64 = z.im
let x = cf64 { 10.0, 20.0 }
im x
`, 20.0);
(0, mtest_1.mtest)('pass record no var', `
We can pass record argument directly without a variable to a function.
`, `
let im z:cf64 = z.im
im (cf64 { 10.0, 20.0 })
`, 20.0);
(0, mtest_1.mtest)('pass record add one value', `
Pass the record data, and return with expression using one field
`, `
let im z:cf64 = 
    z.im + 200.0
im (cf64 { 10.0, 20.0 })
`, 220.0);
(0, mtest_1.mtest)('pass record add value', `
Pass the record data, and return with a record with new value
`, `
let shift z:cf64 = 
    cf64{z.re + 100.0, z.im + 200.0}
let res = shift (cf64 { 10.0, 20.0 })
res.re + res.im
`, 110.0 + 220.0);
(0, mtest_1.mtest)('complex addition', 'complex addition', `
let add_c a:cf64 b:cf64 = cf64 { a.re + b.re, a.im + b.im }
(add_c (cf64 { 10.0, 20.0 }) (cf64 { 30.0, 40.0 })).im
`, 60.0, false);
(0, mtest_1.mtest)('complex exponent', 'complex exponent', `
let sq z:cf64 = cf64 { z.re ** 2.0 - z.im ** 2.0, 2.0 * z.re * z.im }
(sq (cf64 { 10.0, 20.0 })).im
`, 400.0, false);
(0, mtest_1.mtest)('record member assign', 'record member assign', `
record Point = x:mut f64, y:f64
var z = Point { 10.0, 20.0 }
z.x = 30.0
z.x
`, 30.0);
(0, mtest_1.mtest)('one element array init', `
initialize a 1 length of int array
`, `
var a = [10]
a[0]
`, 10);
(0, mtest_1.mtest)('more array element', `
initialize a two elements of int array
`, `
var a = [10, 20]
a[0]
`, 10);
(0, mtest_1.mtest)('sum of elements', `
sum a two elements of int array
`, `
var a = [10, 20]
a[0] + a[1]
`, 30);
(0, mtest_1.mtest)('update array element', `
update the first element of an array
`, `
var a = [10]
a[0] = 20
a[0]
`, 20);
(0, mtest_1.mtest)('update second element', `
update the second element of an array
`, `
var a = [10,20]
a[1] = 200
a[0] + a[1]
`, 210);
(0, mtest_1.mtest)('u8 array', `
declare a u8 array
`, `
var a:u8[2] = [10, 20]
a[0] + a[1]
`, 30);
(0, mtest_1.mtest)('u8 array update', `
declare a u8 array
`, `
var a:u8[2] = [10, 20]
a[0] = 30
a[1] = 40
a[0] + a[1]
`, 70);
(0, mtest_1.mtest)('u8 two dimension array', `
declare a u8 two dimensions array
`, `
var a:u8[2][2] = [10, 20, 30, 40]
a[1][1]
`, 40);
(0, mtest_1.mtest)('u8 two dimen update', `
update two dimensions array
`, `
var a:u8[2][2] = [10, 20, 30, 40]
a[1][1] = 100
a[1][1]
`, 100);
(0, mtest_1.mtest)('u8 access via var', `
update two dimensions array using variable
`, `
var a:u8[10][10]
for i in 0..10
    for j in 0..10
        a[i][j] = i + j
a[7][8]
`, 15);
(0, mtest_1.mtest)('u32 access via var', `
update two dimensions int array using variable
`, `
var a:u32[10][10]
for i in 0..10
    for j in 0..10
        a[i][j] = i + j
a[7][8]
`, 15);
(0, mtest_1.mtest)('pass array to a function', `
pass array variable to a function.
`, `
var a:u8[10][10]
for i in 0..10
    for j in 0..10
        a[i][j] = i + j
let pick arr:u8[10][10] x:int y:int = arr[x][y]
pick a 9 8
`, 17);
(0, mtest_1.mtest)('use tuple', `
define a tuple, and access like array
`, `
var a = (10, 20)
a[0] + a[1]
`, 30);
(0, mtest_1.mtest)('use tuple unpack', `
construct a tuple, and unpack its fields
`, `
let x, y = (10, 20)
x + y
`, 30);
(0, mtest_1.mtest)('unpack tuple fields', `
construct a tuple variable, and unpack fields of tuple variable
`, `
let a = (10, 20)
let x, y = a
x + y
`, 30);
(0, mtest_1.mtest)('function returns tuple', `
return a tuple from a function
`, `
let a() = (10, 20)
let x, y = a()
x + y
`, 30);
(0, mtest_1.mtest)('pass and return tuple', `
pass a tuple to a function, and return a new tuple from the function
`, `
let t a:(int,int) = (10 + a[0], 20 + a[1])
let x, y = t (100, 200)
x + y
`, 110 + 220);
//# sourceMappingURL=aggregate.test.js.map