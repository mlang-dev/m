/* array type in m is a value type. By default it stores the data on the stack. 
 */

const mtest = require('./mtest.js');

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
