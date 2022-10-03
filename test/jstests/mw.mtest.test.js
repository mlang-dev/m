const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mtest() {
    return mw(wasi(), './mtest.wasm', console.log, false);
}

test('test mtest', () => {
    var result = get_mtest();
    return result.then((m) => {
        expect(m.module.instance.exports._start()).toEqual(0);
    });
});
