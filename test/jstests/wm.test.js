const wasi = require('../../docs/wasi.js');
const wm = require('../../docs/wm.js');

function log(text){}

test('expr 10 + 20', ()=>{
    var result = wm(wasi(), './wm.wasm', log, false);
    return result.then((m)=>{
        expect(m.run_mcode("let run()=10 + 20")).toEqual(30);
    });
});

test('expr 10 + 20 * 3', () => {
    var result = wm(wasi(), './wm.wasm', log, false);
    return result.then((m) => {
        expect(m.run_mcode("let run()=10 + 20 * 3")).toEqual(70);
    });
});

test('expr -10 + 20 * 3', () => {
    var result = wm(wasi(), './wm.wasm', log, false);
    return result.then((m) => {
        expect(m.run_mcode("let run()=-10 + 20 * 3")).toEqual(50);
    });
});


test('expr -10.0 + 20.0 * 3.0', () => {
    var result = wm(wasi(), './wm.wasm', log, false);
    return result.then((m) => {
        expect(m.run_mcode("let run()=-10.0 + 20.0 * 3.0")).toEqual(50.0);
    });
});
