function to_js_str(instance, c_str)
{
	const str_len = instance.exports.str_len(c_str)	
	let ta = new Uint8Array(instance.exports.memory.buffer, c_str, str_len)
	return (new TextDecoder()).decode(ta)
}

function str_to_ab(instance, str, c_str)
{
	let ta = (new TextEncoder()).encode(str) //Uint8Array
	let str_len = ta.length + 1;
	let dst = new Uint8Array(instance.exports.memory.buffer, c_str, str_len)
	dst.set(ta);
	dst[ta.length] = 0;
}

function run_wasm_code(wasm)
{
	var compiled = new WebAssembly.Module(wasm);
	var instance = new WebAssembly.Instance(compiled, {});
	return instance.exports.run();
}