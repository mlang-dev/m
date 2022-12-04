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
`, 300, true, true);

mtest.mtest('pattern match with variable', 'Use pattern matching in variable initializer', 
`
let x = 100
let y = 
    match x with
    | 0 -> 100
    | 1 -> 200
    | _ -> 300
y
`, 300, true, true);
