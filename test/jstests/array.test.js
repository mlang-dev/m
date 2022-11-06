/* array type in m is a value type. By default it stores the data on the stack. 
 */

const mtest = require('./mtest.js');

mtest.mtest('array init', 
`
initialize a 1 length of int array
`, 
`
a = [10]
a[0]
`, 10);

mtest.mtest('update array element', 
`
initialize a 1 length of int array
`, 
`
a = [10]
a[0] = 20
a[0]
`, 20);
