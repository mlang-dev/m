const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('10 + 20', ()=>{
    var result = get_mw();
    return result.then((m)=>{
        expect(m.run_mcode("10 + 20")).toEqual(30);
    });
});

test('10 + 20 * 3', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("10 + 20 * 3")).toEqual(70);
    });
});

test('-10 + 20 * 3', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("-10 + 20 * 3")).toEqual(50);
    });
});

test('-10.0 + 20.0 * 3.0', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("-10.0 + 20.0 * 3.0")).toEqual(50.0);
    });
});

test('2.0 ** 3.0', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("2.0 ** 3.0")).toEqual(8.0);
    });
});

test('8 % 2', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("8 % 2")).toEqual(0);
    });
});

test('8 % 5 == 3', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("8 % 5")).toEqual(3);
    });
});