const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('generic function', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let sq x = x * x # generic function
sq 10.0
        `;
        expect(m.run_code(code)).toEqual(100.0);
    });
});
