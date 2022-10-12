const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('parse fun def missing =', () => {
    let output = [];
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
let f x
`;
        expect(m.run_code(code)).toEqual(undefined);
        expect(output).toEqual("symbol [=] is expected to parse [func_def = let IDENT param_decls .= statement] but got [NEWLINE].\n");
    });
});
