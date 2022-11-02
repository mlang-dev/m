const mtest = require('./mtest.js');

mtest.mtest_string('hello world', 'The following is the simple statement to print out string "Hello World !" to the console.', 
`
print "hello world"
`, "hello world");


mtest.mtest_string('print utc-8 string', 'you can print out the unicode string.', 
`
print "你好"
`, "你好");

mtest.mtest_string('integer format', 'print out string with integer format',
`
print "hello %d" 10
`, "hello 10");

mtest.mtest_string('print var', 'print out variable value',
`
v = 10
print "hello %d" v
`, "hello 10");

mtest.mtest_string('print char', 'print one character', 
`
putchar '#'
`, '#');

mtest.mtest_string('print two int vars', 'print out the string with multiple integers',
`
v1 = 10
v2 = 20
print "hello %d %d" v1 v2
`, "hello 10 20");

mtest.mtest_string('print float', 'print float variable', 
`
v = 10.0
print "hello %.2f" v
`, "hello 10.00");

mtest.mtest_string('print string, float and int', `Print string and int type value using variable. Variable's type is inferred from assignment statements.`,
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
`, 30, false);

mtest.mtest('call func with params', 'call func with params', 
`
let id x = x
id 10`, 10, false);

mtest.mtest('call func sub', 'call func sub', 
`
let sub x y = x - y
sub 10 30`, -20, false);

mtest.mtest('use variable', 'use variable', 
`
x = 1000
x * 2`, 2000, false);

mtest.mtest('math.sqrt', `This expression is to call C's sqrt function`, "sqrt 4.0", 2.0);

mtest.mtest('function definition', 
`Use keyword "let" to define a function. 
The grammar is: "let" IDENT param_decls '=' statement. The following code is to define a square function takes
a double value and outputs its squared double value.`, 
`
let sq x:double = x ** 2.0
sq 10.0
`, 100.0);


mtest.mtest('generic function', 
`If type annotation is omitted from the function, then we are defining a generic function. The type of the function is to be infered 
by how it is being used. Character '#' starts comments until the end of the line.`, 
`
let sq x = x * x  # generic function
sq 10.0
`, 100.0);

