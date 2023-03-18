"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.mtest_strings = exports.mtest_string = exports.mtest = void 0;
const mw_1 = require("../mw");
const wasi_1 = require("../wasi");
function get_mw(log = null) {
    let log_nothing = (t) => { };
    return (0, mw_1.mw)((0, wasi_1.wasi)(), '../docs/mw.wasm', log || log_nothing, false, null);
}
function mtest(name, description, code, expect_value, is_tutorial = true, save_wasm = false) {
    test(name, () => {
        var result = get_mw();
        return result.then((m) => {
            if (save_wasm) {
                m.compile(code, "test.wasm");
            }
            let run_result = m.run_code(code);
            let start_result = run_result ? run_result.start_result : run_result;
            if (start_result == undefined)
                start_result = null;
            expect(start_result).toEqual(expect_value);
        });
    });
}
exports.mtest = mtest;
function mtest_string(name, description, code, expect_value, is_tutorial = true) {
    test(name, () => {
        let output = null;
        function log_fun(text) {
            output = text;
        }
        var result = get_mw(log_fun);
        return result.then((m) => {
            let run_result = m.run_code(code);
            let start_result = run_result ? run_result.start_result : run_result;
            if (start_result == undefined)
                start_result = null;
            expect(start_result).toEqual(null);
            expect(output).toEqual(expect_value);
        });
    });
}
exports.mtest_string = mtest_string;
function mtest_strings(name, description, code, expect_value, is_tutorial = true) {
    test(name, () => {
        let outputs = [];
        function log_fun(text) {
            outputs.push(text);
        }
        var result = get_mw(log_fun);
        return result.then((m) => {
            let run_result = m.run_code(code);
            let start_result = run_result ? run_result.start_result : run_result;
            if (start_result == undefined)
                start_result = null;
            expect(start_result).toEqual(null);
            expect(outputs).toEqual(expect_value);
        });
    });
}
exports.mtest_strings = mtest_strings;
//# sourceMappingURL=mtest.js.map