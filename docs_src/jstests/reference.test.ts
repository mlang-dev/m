/**
 * Reference type is like pointer in c. The difference is the reference type is not allow to do arithmetic operation. 
 * You can only use reference type to refer to the other object or pass the reference to functions.
 */
import { mtest } from './mtest';

mtest('int ref use stack memory', 'int ref use stack memory', 
`
let i = 10
let j = &i
i
`, 10);

mtest('evalute ref content', 'evaluate the content of reference type variable j',
`
let i = 10
let j = &i
*j`, 10);

mtest('change via ref', 'change original value by its reference.',
`
let mut i = 10
let mut j = &i
*j = 20
i
`, 20);

mtest('change via ref 2', 'change original value by its reference. print reference', 
`
let mut i = 10
let mut j = &i
*j = 20
*j
`, 20);

mtest('struct type reference', 'struct type reference',
`
let z = cf64{10.0, 20.0}
let j = &z
j.im
`, 20);

mtest('pass struct by ref', `pass struct data to function by reference`, 
`
struct Point = x:mut f64, y:f64
def update(xy:&Point):
    xy.x = 10.0
let z = Point{100.0, 200.0}
update(&z)
z.x
`, 10.0, true);

