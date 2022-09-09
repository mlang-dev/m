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
        expect(m.run_mcode(code)).toEqual(100);
    });
});

test('if 1000 statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
if 1000 then 100 else 10
        `;
        expect(m.run_mcode(code)).toEqual(100);
    });
});

test('if true statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
if true then 100 else 10
        `;
        expect(m.run_mcode(code)).toEqual(100);
    });
});

test('if true variable statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
x = 10
if x then 100 else 10
        `;
        expect(m.run_mcode(code)).toEqual(100);
    });
});

test('if 0 statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
if 0 then 100 else 10
        `;
        expect(m.run_mcode(code)).toEqual(10);
    });
});

test('if false statement', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
if false then 100 else 10
        `;
        expect(m.run_mcode(code)).toEqual(10);
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
        expect(m.run_mcode(code)).toEqual(10);
    });
});

