const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}


function mtest(name, description, code, expect_value, is_tutorial=true, save_wasm=false)
{
    test(name, () => {
        var result = get_mw();
        return result.then((m) => {
            if(save_wasm){
                m.compile(code, "test.wasm");
            }
            expect(m.run_code(code)).toEqual(expect_value);
        });
    });  
}


function mtest_string(name, description, code, expect_value)
{
    test(name, () => {
        let output = null;
        function log_fun (text) {
            output = text;
        }
        var result = get_mw(log_fun);
        return result.then((m) => {
            expect(m.run_code(code)).toEqual(undefined);
            expect(output).toEqual(expect_value);
        });
    });
}

function mtest_strings(name, description, code, expect_value, is_tutorial=true)
{
    test(name, () => {
        let outputs = [];
        function log_fun (text) {
            outputs.push(text);
        }
        var result = get_mw(log_fun);
        return result.then((m) => {
            expect(m.run_code(code)).toEqual(undefined);
            expect(outputs).toEqual(expect_value);
        });
    });
}

exports.mtest = mtest;
exports.mtest_string = mtest_string;
exports.mtest_strings = mtest_strings;