const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
    return mw(wasi(), './mw.wasm', log, false);
}

test('10 < 20', ()=>{
    var result = get_mw();
    return result.then((m)=>{
        expect(m.run_mcode("10 < 20")).toEqual(1);
    });
});

test('10.0 < 20.0', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("10.0 < 20.0")).toEqual(1);
    });
});

test('10.0 <= 20.0', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("10.0 <= 20.0")).toEqual(1);
    });
});

test('10 <= 20', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("10 <= 20")).toEqual(1);
    });
});

test('20 <= 20', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("20 <= 20")).toEqual(1);
    });
});

test('21 <= 20', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("21 <= 20")).toEqual(0);
    });
});

test('21 >= 20', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("21 >= 20")).toEqual(1);
    });
});

test('21 > 20', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("21 > 20")).toEqual(1);
    });
});

test('21 == 20', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("21 == 20")).toEqual(0);
    });
});


test('20 == 20', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("20 == 20")).toEqual(1);
    });
});

test('21 != 20', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("21 != 20")).toEqual(1);
    });
});


test('20 != 20', () => {
    var result = get_mw();
    return result.then((m) => {
        expect(m.run_mcode("20 != 20")).toEqual(0);
    });
});