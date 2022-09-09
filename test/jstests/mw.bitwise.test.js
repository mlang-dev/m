const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('bitwise not 0', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
~0
        `;
        expect(m.run_mcode(code)).toEqual(-1);
    });
});

test('bitwise not -1', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
~-1
        `;
        expect(m.run_mcode(code)).toEqual(0);
    });
});

test('bitwise not 1', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
~1
        `;
        expect(m.run_mcode(code)).toEqual(-2);
    });
});

test('bitwise left', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
1 << 1
        `;
        expect(m.run_mcode(code)).toEqual(2);
    });
});

test('bitwise right', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
2 >> 1
        `;
        expect(m.run_mcode(code)).toEqual(1);
    });
});

test('bitwise and', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
2 & 1
        `;
        expect(m.run_mcode(code)).toEqual(0);
    });
});

test('bitwise or', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
2 | 1
        `;
        expect(m.run_mcode(code)).toEqual(3);
    });
});

test('bitwise xor', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
8 ^ 15
        `;
        expect(m.run_mcode(code)).toEqual(7);
    });
});
