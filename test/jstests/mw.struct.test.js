const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('zf64 complex re', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
z = zf64(10.0, 20.0)
z.re
        `;
        expect(m.run_mcode(code)).toEqual(10.0);
    });
});

test('zf64 complex im', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
z = zf64(10.0, 20.0)
z.im
        `;
        expect(m.run_mcode(code)).toEqual(20.0);
    });
});

test('zf64 complex fun return value', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let z() = zf64(10.0, 20.0)
x = z()
x.im
        `;
        expect(m.run_mcode(code)).toEqual(20.0);
    });
});

test('zf64 complex fun return value without var', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let z() = zf64(10.0, 20.0)
z().im
        `;
        expect(m.run_mcode(code)).toEqual(20.0);
    });
});

test('zf64 complex fun pass value', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let im z:zf64 = z.im
x = zf64(10.0, 20.0)
im x
        `;
        expect(m.run_mcode(code)).toEqual(20.0);
    });
});

test('zf64 complex fun pass value without temp var', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let im z:zf64 = z.im
im (zf64(10.0, 20.0))
        `;
        m.compile(code, "test2.wasm");
        expect(m.run_mcode(code)).toEqual(20.0);
    });
});


test('zf64 complex fun pass and add value', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let im z:zf64 = 
    z.im + 200.0
im (zf64(10.0, 20.0))
        `;
        m.compile(code, "test2.wasm");
        expect(m.run_mcode(code)).toEqual(220.0);
    });
});

test('zf64 complex fun pass and return value', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let shift z:zf64 = 
    zf64(z.re + 100.0, z.im + 200.0)
result = shift (zf64(10.0, 20.0))
result.re + result.im
        `;
        m.compile(code, "test2.wasm");
        expect(m.run_mcode(code)).toEqual(110.0 + 220.0);
    });
});

