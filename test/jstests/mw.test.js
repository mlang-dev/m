const wasi = require('../../docs/wasi.js');
const mw = require('../../docs/mw.js');

function get_mw(log=null){
    if(log==null)
        log = t => {};
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
        m.compile(code, "test.wasm");
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
x = 1000
x * 2`;
        expect(m.run_mcode(code)).toEqual(2000);
    });
});

test('hello world', () => {
    let output = null;
    function log_fun (text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
print "hello world"
`;
        expect(m.run_mcode(code)).toEqual(undefined);
        expect(output).toEqual("hello world");
    });
});

test('string format', () => {
    let output = null;
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
print "hello %d" 10
`;      
        expect(m.run_mcode(code)).toEqual(undefined);
        expect(output).toEqual("hello 10");
    });
});

test('string format with var', () => {
    let output = null;
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
v = 10
print "hello %d" v
`;
        m.compile(code, "test.wasm");
        expect(m.run_mcode(code)).toEqual(undefined);
        expect(output).toEqual("hello 10");
    });
});

test('string format with both int var', () => {
    let output = null;
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
v1 = 10
v2 = 20
print "hello %d %d" v1 v2
`;
        expect(m.run_mcode(code)).toEqual(undefined);
        expect(output).toEqual("hello 10 20");
    });
});

test('string format with float param', () => {
    let output = null;
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
v = 10.0
print "hello %.2f" v
`;
        expect(m.run_mcode(code)).toEqual(undefined);
        expect(output).toEqual("hello 10.00");
    });
});

test('string format with string, float, and int param', () => {
    let output = null;
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
s = "world"
f = 20.0
i = 10
print "hello %s %.2f %d" s f i
`;
        expect(m.run_mcode(code)).toEqual(undefined);
        expect(output).toEqual("hello world 20.00 10");
    });
});

test('utc-8 string', () => {
    let output = null;
    function log_fun(text) {
        output = text;
    }
    var result = get_mw(log_fun);
    return result.then((m) => {
        let code = `
print "你好"
`;
        expect(m.run_mcode(code)).toEqual(undefined);
        expect(output).toEqual("你好");
    });
});


test('math.sqrt', () => {
    var result = get_mw();
    return result.then((m) => {
        let code = `
sqrt 4.0
`;
        expect(m.run_mcode(code)).toEqual(2.0);
    });
});
