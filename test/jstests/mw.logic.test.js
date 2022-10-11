const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('true', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
true
        `;
        expect(m.run_code(code)).toEqual(1);
    });
});

test('false', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
false
        `;
        expect(m.run_code(code)).toEqual(0);
    });
});

test('true && true', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
true && true
        `;
        expect(m.run_code(code)).toEqual(1);
    });
});

test('true && false', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
true && false
        `;
        expect(m.run_code(code)).toEqual(0);
    });
});


test('false && true', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
false && true
        `;
        expect(m.run_code(code)).toEqual(0);
    });
});

test('false && false', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
false && false
        `;
        expect(m.run_code(code)).toEqual(0);
    });
});

test('false || false', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
false || false
        `;
        expect(m.run_code(code)).toEqual(0);
    });
});

test('true || false', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
true || false
        `;
        expect(m.run_code(code)).toEqual(1);
    });
});

test('false || true', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
false || true
        `;
        expect(m.run_code(code)).toEqual(1);
    });
});

test('true || true', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
true || true
        `;
        expect(m.run_code(code)).toEqual(1);
    });
});

test('!false', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
!false
        `;
        expect(m.run_code(code)).toEqual(1);
    });
});

test('!true', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
!true
        `;
        expect(m.run_code(code)).toEqual(0);
    });
});
