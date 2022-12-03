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
`, 100, true, true);

mtest.mtest('pattern match int second', 'Apply pattern match to integer, matching the second case', 
`
let pm x =
    match x with
    | 0 -> 100
    | 1 -> 200
pm 1
`, 200);
