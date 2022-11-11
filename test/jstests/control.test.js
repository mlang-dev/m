const mtest = require('./mtest.js');

mtest.mtest('if 1 statement', 'if 1 statement', "if 1 then 100 else 10", 100);


mtest.mtest('if 3 statement', 'if any non-zero statement is true', "if 3 then 100 else 10", 100);

mtest.mtest('if true statement', 'use true literal', "if true then 100 else 10", 100);

mtest.mtest('use bool variable', 'use bool variable', 
`
x = 10
if x then 100 else 10
`, 100);


mtest.mtest('if 0 statement', 'if 0 statement', 'if 0 then 100 else 10', 10); 

mtest.mtest('if false statement', 'use false literal', 'if false then 100 else 10', 10); 

mtest.mtest('else if statement first', 'else if statement - choose first branch', 
`
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 10
`, 100); 

mtest.mtest('else if statement second', 'else if statement - choose second branch', 
`
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 20
`, 200); 


mtest.mtest('else if statement third', 'else if statement - choose third branch', 
`
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 30
`, 300); 

mtest.mtest('else if statement else', 'else if statement - choose else branch', 
`
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 40
`, 400); 

mtest.mtest_strings('if - else if print statement', 'if - else if print statement',
`
let choice n = 
    if n <= 10 then 
        putchar '#' 
    else if n <= 20 then
        putchar '?'
    else
        putchar '*' 
choice 4
choice 14
choice 24
`, ['#', '?', '*'], false);

mtest.mtest('for loop statement', 'for loop statement', 
`
sum = 0
for i in 0..5
    sum = sum + i
sum
`, 10); 

mtest.mtest('for loop statement float', 'for loop statement using float type', 
`
sum = 0.0
for i in 0.0..5.0
    sum = sum + i
sum
`, 10.0); 

mtest.mtest('for loop with step', 'for loop statement with step variable', 
`
step = 2
sum = 0
for i in 2..step..10
    sum = sum + i
sum
`, 20); 

mtest.mtest('nest for loop', 'nest for loop statement', 
`
sum = 0
for i in 1..3
    for j in 1..3
        sum = sum + i * j
sum`, 9); 

mtest.mtest('recursive factorial', 'implement factorial using recursive', 
`
let factorial n = if n == 1 then n else n * factorial (n-1)
factorial 5
`, 120); 

mtest.mtest('for loop factorial', 'implement factorial using for loop', 
`
let factorial n = 
    p = 1
    for i in 2..n+1
        p = p * i
    p
factorial 5
`, 120); 

mtest.mtest('for loop break', 'use break statement in for loop', 
`
for i in 0..10
    if i == 5 then 
        break
i
`, 5, true, true); 

