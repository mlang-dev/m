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
