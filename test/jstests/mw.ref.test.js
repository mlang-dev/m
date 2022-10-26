const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('int ref', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
10
        `;
        expect(m.run_code(code)).toEqual(10);
    });
});
