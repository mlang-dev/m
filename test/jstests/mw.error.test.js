const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('parse char not having correct length', () => {
    let output = [];
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
c = '
`;
        expect(m.run_mcode(code)).toEqual(undefined);
        expect(output).toEqual("missing end quote for char literal. location (line, col): (2, 5)\n");
    });
});
