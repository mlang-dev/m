const mtest = require('./mtest.js');


mtest.mtest('int ref use stack memory', 'int ref use stack memory', 
`
i = 10
j = &i
i`, 10, false);

mtest.mtest('evalute ref content', 'evaluate the content of reference type variable j',
`
i = 10
j = &i
*j`, 10);

mtest.mtest('change via ref', 'change original value by its reference.',
`
i = 10
j = &i
*j = 20
i
`, 20);

mtest.mtest('change via ref 2', 'change original value by its reference. print reference', 
`
i = 10
j = &i
*j = 20
*j
`, 20);

mtest.mtest('struct type reference', 'struct type reference',
`
z = cf64(10.0, 20.0)
j = &z
j.im
`, 20);

mtest.mtest('pass struct by ref', `pass struct data to function by reference`, 
`
let update x:&cf64 =
    x.re = 10.0
z = &cf64(100.0, 200.0)
update z
z.re
`, 10.0, true);

