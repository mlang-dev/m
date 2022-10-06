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
