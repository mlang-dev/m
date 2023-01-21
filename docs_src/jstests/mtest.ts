import { mw } from '../mw';
import { wasi } from '../wasi'

function get_mw(log:CallableFunction|null=null){
    let log_nothing = (t:any) => {};
    return mw(wasi(), '../docs/mw.wasm', log || log_nothing, false, null);
}

export function mtest(name:string, description:string, code:string, expect_value:any, is_tutorial=true, save_wasm=false)
{
    test(name, () => {
        var result = get_mw();
        return result.then((m) => {
            if(save_wasm){
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

export function mtest_string(name:string, description:string, code:string, expect_value:string, is_tutorial=true)
{
    test(name, () => {
        let output:string | null = null;
        function log_fun (text:string) {
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

export function mtest_strings(name:string, description:string, code:string, expect_value:string[], is_tutorial=true)
{
    test(name, () => {
        let outputs:string[] = [];
        function log_fun (text:string) {
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
