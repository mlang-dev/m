const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function log(text){
    ;
}

function get_mw(){
    return mw(wasi(), './mw.wasm', log, false);
}

function get_mtest() {
    return mw(wasi(), './mtest.wasm', console.log, false);
}

test('test mtest', () => {
    var result = get_mtest();
    return result.then((m) => {
        expect(m.module.instance.exports._start()).toEqual(0);
    });
});

test('expr 10 + 20', ()=>{
    var result = get_mw();
    return result.then((m)=>{
        expect(m.run_mcode("10 + 20")).toEqual(30);
    });
});

test('expr 10 + 20 * 3', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("10 + 20 * 3")).toEqual(70);
    });
});

test('expr -10 + 20 * 3', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("-10 + 20 * 3")).toEqual(50);
    });
});

test('expr -10.0 + 20.0 * 3.0', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("-10.0 + 20.0 * 3.0")).toEqual(50.0);
    });
});

test('call func', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let run() = 10 + 20
run()
        `;
        expect(m.run_mcode(code)).toEqual(30);
    });
});

test('call func with params', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let id x = x
id 10
        `;
        m.compile(code, "call_fun.wasm");
        expect(m.run_mcode(code)).toEqual(10);
    });
});

test('call func sub', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
let sub x y = x - y
sub 10 30
        `;
        expect(m.run_mcode(code)).toEqual(-20);
    });
});

test('use variable', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
x = 10
x
        `;
        expect(m.run_mcode(code)).toEqual(10);
    });
});
