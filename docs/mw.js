function mw(wasi_env, module_name, print_func, remote_file) {
	var wm_instance = null;
	var print_func = print_func;

	var module;
	if (remote_file){
		module = WebAssembly.instantiateStreaming(fetch(module_name), { "wasi_snapshot_preview1": wasi_env });
	}else{
		/* for nodejs use */
		const fs = require('fs');
		const path = require("path");
		const wasmBuffer = fs.readFileSync(path.resolve(__dirname, module_name));
		module = WebAssembly.instantiate(wasmBuffer, { "wasi_snapshot_preview1": wasi_env });
	}
	let result = new Promise(function (resolve, reject) {
		// "Producing Code" (May take some time)
		module.then(function (obj) {
			init_module(obj);
			resolve({
				run_mcode: run_mcode, 
				compile: compile,
				module: obj
			}); //when success
		}).catch(function (reason) {
			reject(reason); //when failure
		});
	});
	
	return result;

	function init_module(module)
	{
		wm_instance = module.instance;
		wasi_env.setEnv(wm_instance, print_func);
		if(wm_instance.exports.version != null){
			let c_str = wm_instance.exports.version();
			print_func(to_js_str(wm_instance, c_str));
			wm_instance.exports.free_mem(c_str);
		}
	}

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

	function run_wasm_code(instance, wasm, wasm_size)
	{
		let ta = new Uint8Array(instance.exports.memory.buffer, wasm, wasm_size);
		var compiled = new WebAssembly.Module(ta);
		var instance = new WebAssembly.Instance(compiled, {imports: { print : instance.exports.print }});
		return instance.exports._start();
	}

	function run_mcode(code) 
	{
		if (wm_instance == null) {
			print_func("m loading is failed.");
			return;
		}
		let new_ptr = wm_instance.exports.alloc_mem(1024);
		str_to_ab(wm_instance, code, new_ptr);
		let wasm = wm_instance.exports.compile_code(new_ptr);
		let wasm_size = wm_instance.exports.get_code_size();
		let result = run_wasm_code(wm_instance, wasm, wasm_size);
		wm_instance.exports.free_mem(wasm);
		return result;
	}

	function compile(code, file_path)
	{
		let new_ptr = wm_instance.exports.alloc_mem(1024);
		str_to_ab(wm_instance, code, new_ptr);
		let wasm = wm_instance.exports.compile_code(new_ptr);
		let wasm_size = wm_instance.exports.get_code_size();
		let ta = new Uint8Array(wm_instance.exports.memory.buffer, wasm, wasm_size);
		const fs = require('fs');
		fs.writeFileSync(file_path, ta, 'binary');
		wm_instance.exports.free_mem(wasm);
	}
}

if (typeof module === 'object') {
	module.exports = mw;
}
