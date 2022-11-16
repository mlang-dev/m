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
        output.push(text);
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
let f x
`;
        expect(m.run_code(code)).toEqual(undefined);
        expected = [
            "symbol [:] is expected to parse [param_decl = IDENT .: type_expr] but got [NEWLINE].\n",
            "symbol [NEWLINE] is not expected after grammar rule [param_decl = IDENT].\n"
        ];     
        expect(output).toEqual(expected, output);
    });
});
