const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('analyzer type mismatch', () => {
    let output = [];
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
i:int = "string"
`;
        expect(m.run_code(code)).toEqual(undefined);
        expect(output).toEqual("variable type not matched with literal constant. loc (line, col): (2, 1)\n");
    });
});
