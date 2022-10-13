const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('cf64 complex re', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
z = cf64(10.0, 20.0)
z.re
        `;
        expect(m.run_code(code)).toEqual(10.0);
    });
});

test('cf64 complex im', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
z = cf64(10.0, 20.0)
z.im
        `;
        expect(m.run_code(code)).toEqual(20.0);
    });
});

test('cf64 complex fun return value', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let z() = cf64(10.0, 20.0)
x = z()
x.im
        `;
        expect(m.run_code(code)).toEqual(20.0);
    });
});

test('cf64 complex fun return value without var', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let z() = cf64(10.0, 20.0)
z().im
        `;
        expect(m.run_code(code)).toEqual(20.0);
    });
});

test('cf64 complex fun pass value', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let im z:cf64 = z.im
x = cf64(10.0, 20.0)
im x
        `;
        expect(m.run_code(code)).toEqual(20.0);
    });
});

test('cf64 complex fun pass value without temp var', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let im z:cf64 = z.im
im (cf64(10.0, 20.0))
        `;
        expect(m.run_code(code)).toEqual(20.0);
    });
});


test('cf64 complex fun pass and add value', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let im z:cf64 = 
    z.im + 200.0
im (cf64(10.0, 20.0))
        `;
        expect(m.run_code(code)).toEqual(220.0);
    });
});

test('cf64 complex fun pass and return value', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let shift z:cf64 = 
    cf64(z.re + 100.0, z.im + 200.0)
result = shift (cf64(10.0, 20.0))
result.re + result.im
        `;
        expect(m.run_code(code)).toEqual(110.0 + 220.0);
    });
});

test('struct with different types', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
struct A = x:int, y:double
a = A(10, 20.0)
a.y`;
        expect(m.run_code(code)).toEqual(20.0);
    });
});

test('struct initialization without var', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
struct A = x:int, y:double
A(10, 20.0).y
`;
        expect(m.run_code(code)).toEqual(20.0);
    });
});

test('struct in struct', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
struct AB = a:cf64, b:cf64
ab = AB(cf64(10.0, 20.0), cf64(30.0, 40.0))
ab.b.im
`;
        expect(m.run_code(code)).toEqual(40.0);
    });
});

test('return struct in struct', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
struct AB = a:cf64, b:cf64
let get () = AB(cf64(10.0, 20.0), cf64(30.0, 40.0))
get().b.im
`;
        expect(m.run_code(code)).toEqual(40.0);
    });
});

test('return struct in struct 1', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
struct AB = a:cf64, b:cf64
let get () = 
    ab = AB(cf64(10.0, 20.0), cf64(30.0, 40.0))
    ab
get().b.re
`;
        expect(m.run_code(code)).toEqual(30.0);
    });
});

test('return struct from struct', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
struct AB = a:cf64, b:cf64
let get () = 
    ab = AB(cf64(10.0, 20.0), cf64(30.0, 40.0))
    ab.a
get().im
`;
        expect(m.run_code(code)).toEqual(20.0);
    });
});

test('return struct from struct 1', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
struct AB = a:cf64, b:cf64
let get () = AB(cf64(10.0, 20.0), cf64(30.0, 40.0)).a
get().re
`;
        expect(m.run_code(code)).toEqual(10.0);
    });
});

test('pass struct and return struct', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let add z:cf64 op:double = cf64(z.re + op, z.im + op)
x = cf64(10.0, 20.0)
(add x 10.0).im
`;
        expect(m.run_code(code)).toEqual(30.0);
    });
});

test('pass struct and return struct more forms', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let add z:cf64 op:double = cf64(z.re + op, z.im + op)
(add (cf64(10.0, 20.0)) 10.0).re
`;
        expect(m.run_code(code)).toEqual(20.0);
    });
});

test('complex addition', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let add_c a:cf64 b:cf64 = cf64(a.re + b.re, a.im + b.im)
(add_c (cf64(10.0, 20.0)) (cf64(30.0, 40.0))).im
`;
        expect(m.run_code(code)).toEqual(60.0);
    });
});

test('complex exponent', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let sq z:cf64 = cf64(z.re ** 2.0 - z.im ** 2.0, 2.0 * z.re * z.im)
(sq (cf64(10.0, 20.0))).im
`;
        expect(m.run_code(code)).toEqual(400.0);
    });
});

test('struct member assign', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
z = cf64(10.0, 20.0)
z.re = 30.0
z.re
`;
        expect(m.run_code(code)).toEqual(30.0);
    });
});

test('mandelbrot set function', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let print_density:() d:int =
  if d > 8 then
    putchar ' '
  else if d > 4 then
    putchar '.'
  else if d > 2 then
    putchar '+'
  else		
    putchar '*'
      
let sum_sq z:cf64 = z.re ** 2.0 + z.im ** 2.0
let ms z:cf64 c:cf64 = cf64(z.re ** 2.0 - z.im ** 2.0 + c.re, 2.0 * z.re * z.im + c.im)
let converger z:cf64 iters:int c:cf64 = 
  if iters > 255 || (sum_sq z) > 4.0 then
    iters
  else
    converger (ms z c) (iters + 1) c
let converge z:cf64 = converger z 0 z
let print_ms start:cf64 step:cf64 =
  for y in start.im..step.im..start.im + step.im * 80.0
    for x in start.re..step.re..start.re + step.re * 78.0
       print_density (converge (cf64(x, y)))
    putchar '\n'
print_ms (cf64(-2.3, -1.3)) (cf64(0.05, 0.07))
(ms (cf64(10.0, 20.0)) (cf64(10.0, 20.0))).im
`;
        expect(m.run_code(code)).toEqual(400.0 + 20.0);
    });
});
