const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('parse char missing end quote', () => {
    let output = [];
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
c = '
`;
        expect(m.run_code(code)).toEqual(undefined);
        expect(output).toEqual("missing end quote for char literal. location (line, col): (2, 5)\n");
    });
});

test('parse char too long', () => {
    let output = [];
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
c = 'ab'
`;
        expect(m.run_code(code)).toEqual(undefined);
        expect(output).toEqual("character literal is found to have more than 1 character. location (line, col): (2, 5)\n");
    });
});

test('parse string missing end quote', () => {
    let output = [];
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
c = "abc
`;
        expect(m.run_code(code)).toEqual(undefined);
        expect(output).toEqual("missing end quote for string literal. location (line, col): (2, 5)\n");
    });
});
