/**
 * M supports pattern matching for tagged union and other structured data types.
 */

const mtest = require('./mtest.js');

mtest.mtest('pattern match int', 'Apply pattern match to integer', 
`
let pm x =
    match x with
    | 0 -> 100
    | 1 -> 200
pm 0
`, 100);

mtest.mtest('pattern match int second', 'Apply pattern match to integer, matching the second case', 
`
let pm x =
    match x with
    | 0 -> 100
    | 1 -> 200
pm 1
`, 200);

mtest.mtest('pattern match wildcard', 'Apply pattern match to integer, matching wildcard case', 
`
let pm x =
    match x with
    | 0 -> 100
    | 1 -> 200
    | _ -> 300
pm 20
`, 300);

mtest.mtest('pattern match with variable', 'Use pattern matching in variable initializer', 
`
let x = 100
let y = 
    match x with
    | 0 -> 100
    | 1 -> 200
    | _ -> 300
y
`, 300);

mtest.mtest('pattern match out of order', 'Apply pattern match to integer, matching int out of order', 
`
let pm x =
    match x with
    | 1 -> 100
    | 0 -> 200
    | _ -> 300
pm 0
`, 200);

mtest.mtest('pattern match not zero base', 'Apply pattern match to integer starting non-zero, matching the first', 
`
let pm x =
    match x with
    | 1 -> 100
    | 2 -> 200
    | _ -> 300
pm 1
`, 100);

mtest.mtest('pattern match not zero second', 'Apply pattern match to integer starting non-zero, matching second', 
`
let pm x =
    match x with
    | 1 -> 100
    | 2 -> 200
    | _ -> 300
pm 2
`, 200);

mtest.mtest('pattern match not zero wildcard', 'Apply pattern match to integer starting non-zero, matching wildcard', 
`
let pm x =
    match x with
    | 1 -> 100
    | 2 -> 200
    | _ -> 300
pm 0
`, 300);

mtest.mtest('pattern match gap pattern', 'Apply pattern match to integer case with gap, matching first', 
`
let pm x =
    match x with
    | 1 -> 100
    | 3 -> 200
    | _ -> 300
pm 1
`, 100);

mtest.mtest('pattern match gap pattern second', 'Apply pattern match to integer case with gap, matching second', 
`
let pm x =
    match x with
    | 1 -> 100
    | 3 -> 200
    | _ -> 300
pm 3
`, 200);

mtest.mtest('pattern match gap pattern wildcard', 'Apply pattern match to integer case with gap, matching wildcard', 
`
let pm x =
    match x with
    | 1 -> 100
    | 3 -> 200
    | _ -> 300
pm 2
`, 300);
