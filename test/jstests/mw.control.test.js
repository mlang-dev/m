const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('if 1 statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
if 1 then 100 else 10
        `;
        expect(m.run_code(code)).toEqual(100);
    });
});

test('if 1000 statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
if 1000 then 100 else 10
        `;
        expect(m.run_code(code)).toEqual(100);
    });
});

test('if true statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
if true then 100 else 10
        `;
        expect(m.run_code(code)).toEqual(100);
    });
});

test('if true variable statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
x = 10
if x then 100 else 10
        `;
        expect(m.run_code(code)).toEqual(100);
    });
});

test('if 0 statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
if 0 then 100 else 10
        `;
        expect(m.run_code(code)).toEqual(10);
    });
});

test('if false statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
if false then 100 else 10
        `;
        expect(m.run_code(code)).toEqual(10);
    });
});

test('if - else if 1st branch statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 10
        `;
        expect(m.run_code(code)).toEqual(100);
    });
});

test('if - else if 2nd branch statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 20
        `;
        expect(m.run_code(code)).toEqual(200);
    });
});

test('if - else if 3rd branch statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 30
        `;
        expect(m.run_code(code)).toEqual(300);
    });
});

test('if - else if 4th branch statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let choice n = 
    if n <= 10 then 100 
    else if n <= 20 then 200
    else if n <= 30 then 300
    else 400
choice 40
        `;
        expect(m.run_code(code)).toEqual(400);
    });
});

test('if - else if print statement', () => {
    let outputs = [];
    function log_fun(text) {
        outputs.push(text);
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
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
        `;
        expect(m.run_code(code)).toEqual(undefined);
        expect(outputs).toEqual(['#', '?', '*']);
    });
});

test('for loop statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
sum = 0
for i in 0..5
    sum = sum + i
sum
        `;
        expect(m.run_code(code)).toEqual(10);
    });
});

test('for loop statement double type', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
sum = 0.0
for i in 0.0..5.0
    sum = sum + i
sum
        `;
        expect(m.run_code(code)).toEqual(10);
    });
});

test('for nest loop statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
sum = 0
for i in 1..3
    for j in 1..3
        sum = sum + i * j
sum`;
        expect(m.run_code(code)).toEqual(9);
    });
});

test('for loop with step statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
step = 2
sum = 0
for i in 2..step..10
    sum = sum + i
sum`;
        expect(m.run_code(code)).toEqual(20);
    });
});

test('recursive factorial', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let factorial n = if n == 1 then n else n * factorial (n-1)
factorial 5
        `;
        expect(m.run_code(code)).toEqual(120);
    });
});

test('for loop factorial', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let factorial n = 
    p = 1
    for i in 2..n+1
        p = p * i
    p
factorial 5
        `;
        expect(m.run_code(code)).toEqual(120);
    });
});
