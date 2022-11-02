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
        let code = `10 + 20`;
        expect(m.run_code(code)).toEqual(30);
    });
});

test('10 + 20 * 3', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `10 + 20 * 3`;
        expect(m.run_code(code)).toEqual(70);
    });
});

test('-10 + 20 * 3', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `-10 + 20 * 3`;
        expect(m.run_code(code)).toEqual(50);
    });
});

test('-10.0 + 20.0 * 3.0', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = "-10.0 + 20.0 * 3.0";
        expect(m.run_code(code)).toEqual(50.0);
    });
});

test('2.0 ** 3.0', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = "2.0 ** 3.0";
        expect(m.run_code(code)).toEqual(8.0);
    });
});

test('8 % 2', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = "8 % 2";
        expect(m.run_code(code)).toEqual(0);
    });
});

test('8 % 5 == 3', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = "8 % 5"
        expect(m.run_code("8 % 5")).toEqual(3);
    });
});
