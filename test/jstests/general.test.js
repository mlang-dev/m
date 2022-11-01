const mtest = require('./mtest.js');

mtest.mtest_string('hello world', 'hello world', 
`
print "hello world"
`, "hello world");


mtest.mtest_string('utc-8 string', 'utc-8 string', 
`
print "你好"
`, "你好");

mtest.mtest_string('string format', 'string format',
`
print "hello %d" 10
`, "hello 10");

mtest.mtest_string('string format with var', 'string format with var',
`
v = 10
print "hello %d" v
`, "hello 10");

mtest.mtest_string('print char', 'print char', 
`
putchar '#'
`, '#');

mtest.mtest_string('string format with both int var', 'string format with both int var',
`
v1 = 10
v2 = 20
print "hello %d %d" v1 v2
`, "hello 10 20");

mtest.mtest_string('string format with float param', 'string format with float param', 
`
v = 10.0
print "hello %.2f" v
`, "hello 10.00");

mtest.mtest_string('string format with string, float, and int param', 'string format with string, float, and int param',
`
s = "world"
f = 20.0
i = 10
print "hello %s %.2f %d" s f i
`, "hello world 20.00 10");

mtest.mtest('call func', 'call func', 
`
let run() = 10 + 20
run()
`, 30);

mtest.mtest('call func with params', 'call func with params', 
`
let id x = x
id 10`, 10);

mtest.mtest('call func sub', 'call func sub', 
`
let sub x y = x - y
sub 10 30`, -20);

mtest.mtest('use variable', 'use variable', 
`
x = 1000
x * 2`, 2000);

mtest.mtest('math.sqrt', 'math.sqrt', "sqrt 4.0", 2.0);
