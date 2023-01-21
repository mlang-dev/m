/**
 * Use pattern match to support pattern matching of int, wildcard and variable.
 */

import { mtest } from './mtest';

mtest('pattern match int', 'Apply pattern match to integer', 
`
let pm x =
    match x with
    | 0 -> 100
    | 1 -> 200
pm 0
`, 100);

mtest('pattern match int second', 'Apply pattern match to integer, matching the second case', 
`
let pm x =
    match x with
    | 0 -> 100
    | 1 -> 200
pm 1
`, 200);

mtest('pattern match wildcard', 'Apply pattern match to integer, matching wildcard case', 
`
let pm x =
    match x with
    | 0 -> 100
    | 1 -> 200
    | _ -> 300
pm 20
`, 300);

mtest('pattern match with variable', 'Use pattern matching in variable initializer', 
`
let x = 100
let y = 
    match x with
    | 0 -> 100
    | 1 -> 200
    | _ -> 300
y
`, 300);

mtest('pattern match out of order', 'Apply pattern match to integer, matching int out of order', 
`
let pm x =
    match x with
    | 1 -> 100
    | 0 -> 200
    | _ -> 300
pm 0
`, 200);

mtest('pattern match not zero base', 'Apply pattern match to integer starting non-zero, matching the first', 
`
let pm x =
    match x with
    | 1 -> 100
    | 2 -> 200
    | _ -> 300
pm 1
`, 100);

mtest('pattern match not zero second', 'Apply pattern match to integer starting non-zero, matching second', 
`
let pm x =
    match x with
    | 1 -> 100
    | 2 -> 200
    | _ -> 300
pm 2
`, 200);

mtest('pattern match not zero wildcard', 'Apply pattern match to integer starting non-zero, matching wildcard', 
`
let pm x =
    match x with
    | 1 -> 100
    | 2 -> 200
    | _ -> 300
pm 0
`, 300);

mtest('pattern match gap pattern', 'Apply pattern match to integer case with gap, matching first', 
`
let pm x =
    match x with
    | 1 -> 100
    | 3 -> 200
    | _ -> 300
pm 1
`, 100);

mtest('pattern match gap pattern second', 'Apply pattern match to integer case with gap, matching second', 
`
let pm x =
    match x with
    | 1 -> 100
    | 3 -> 200
    | _ -> 300
pm 3
`, 200);

mtest('pattern match gap pattern wildcard', 'Apply pattern match to integer case with gap, matching wildcard', 
`
let pm x =
    match x with
    | 1 -> 100
    | 3 -> 200
    | _ -> 300
pm 2
`, 300);

mtest('pattern match negative', 'Apply pattern match to negative integer case', 
`
let pm x =
    match x with
    | -1 -> 100
    | 3 -> 200
    | _ -> 300
pm (-1)
`, 100);

mtest('pattern match negative second match', 'Apply pattern match to negative integer case, matching the second', 
`
let pm x =
    match x with
    | -1 -> 100
    | 3 -> 200
    | _ -> 300
pm 3
`, 200);

mtest('pattern match negative wildcard', 'Apply pattern match to negative integer case, matching the wildcard', 
`
let pm x =
    match x with
    | -1 -> 100
    | 3 -> 200
    | _ -> 300
pm 0
`, 300);

mtest('pattern match variable', 'Apply pattern match to variable case, matching the variable expr', 
`
let pm x =
    match x with
    | -1 -> 100
    | 3 -> 200
    | y -> y + 10
pm 2
`, 12);
