const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('int ref use stack memory', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
i = 10
j = &i
i
        `;
        expect(m.run_code(code)).toEqual(10);
    });
});

test('int ref return ref content', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
i = 10
j = &i
*j
        `;
        expect(m.run_code(code)).toEqual(10);
    });
});

test('int ref change via ref', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
i = 10
j = &i
*j = 20
i
        `;
        expect(m.run_code(code)).toEqual(20);
    });
});

test('int ref change via ref 2', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
i = 10
j = &i
*j = 20
*j
        `;
        expect(m.run_code(code)).toEqual(20);
    });
});

// test('int ref change via ref 2', () => {
//     var result = get_mw();
//     return result.then((m) => {
//         let code = `
// z = cf64(10.0, 20.0)
// j = &z
// j.im
//         `;
//         expect(m.run_code(code)).toEqual(20);
//     });
// });
