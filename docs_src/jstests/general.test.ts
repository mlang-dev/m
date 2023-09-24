import { mtest, mtest_string } from './mtest';

mtest_string('hello world', 'The following is the simple statement to print out string "Hello World !" to the console.', 
`
print("hello world, 您好 !")
`, "hello world, 您好 !");


mtest_string('integer format', 'Print string with integer format.',
`
print("hello %d", 10)
`, "hello 10", false);

mtest_string('print var', `Print a variable's value.`,
`
let v = 10
print("hello %d", v)
`, "hello 10", false);

mtest_string('print char', 'Print one character.', 
`
putchar('#')
`, '#', false);

mtest_string('print two int vars', 'Print the string with multiple integer variables.',
`
let v1 = 10
let v2 = 20
print("hello %d %d", v1, v2)
`, "hello 10 20", false);

mtest_string('print float', 'Print a float variable.', 
`
let v = 10.0
print("hello %.2f", v)
`, "hello 10.00", false);

mtest_string('print string, float and int', `Print string and int type value using variable. Variable's type is inferred from assignment statements.`,
`
let s = "world"
let f = 20.0
let i = 10
print("hello %s %.2f %d", s, f, i)
`, "hello world 20.00 10");

mtest('call func', 'call func', 
`
def run(): 10 + 20
run()
`, 30, false);

mtest('call func with params', 'call func with params', 
`
def id(x): x
id(10)`, 10, false);

mtest('call func sub', 'call func sub', 
`
def sub(x, y): x - y
sub(10, 30)`, -20, false);

mtest('use variable', 'use variable', 
`
let x = 1000
x * 2`, 2000, false);

mtest('math.sqrt', `sqrt operator`, "|/4.0", 2.0, false);

mtest('function definition', 
`Use keyword "let" to define a function. 
The grammar is: "let" IDENT param_decls '=' statement. The following code is to define a square function takes
a f64 value and outputs its squared f64 value.`, 
`
def sq(x:f64): x ** 2.0
sq(10.0)
`, 100.0);


mtest('generic function', 
`If type annotation is omitted from the function, then we are defining a generic function. The type of the function is to be infered 
by how it is being used. Character '#' starts comments until the end of the line.`, 
`
def sq(x): x * x  // generic function
sq(10.0)
`, 100.0);

mtest_string('struct type', `You can define struct aggregate type like struct in C. They behavior the same except more succinct in m.`,
`
struct Point2D = x:f64, y:f64
let p = Point2D { 10.0, 20.0 }
print("p.x: %.1f, p.y: %.1f", p.x, p.y)
`, "p.x: 10.0, p.y: 20.0");

//FIXME: uncomment the print will yield deadloop
// mtest('pass by value', `mlang uses pass-by value calling convention to pass arguments to functions. This means the argument value is copied to function's parameter.`,
// `
// struct Point2D = x:mut f64, y:f64
// let change z:Point2D = 
//     z.x = z.x * 10.0
//     z
// let old_z = Point2D { 10.0, 20.0 }
// let new_z = change old_z
// new_z.x
// `, 100.00);


mtest('more assign op', 'more assignment operators', 
`
let mut x = 1000
x += 1
x`, 1001, false);

mtest('inc op postfix', 'inc assignment operator, postfix', 
`
let mut x = 1000
x++
x`, 1001, false);

mtest('inc op', 'inc assignment operator', 
`
let mut x = 1000
++x
x`, 1001, false);

mtest('dec op postfix', 'dec assignment operator, postfix', 
`
let mut x = 1000
x--
x`, 999, false);

mtest('dec op', 'dec assignment operator', 
`
let mut x = 1000
--x
x`, 999, false);

mtest('ternary op', 'conditional ternary operator', 
`
let x = 1000
x ? 10: 100
`, 10, false);
