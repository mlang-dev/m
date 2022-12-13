function mw(wasi_env, module_name, print_func, remote_file, set_image_data) {
	var mw_instance = null;  //m compiler instance
	var code_instance = null;
	var code_memory_as_array = null;
	var print_func = print_func;
	var version = null;
	var set_image_data = set_image_data
	var module;
	if (remote_file){
		module = WebAssembly.instantiateStreaming(fetch(module_name), 
		{ 
			"wasi_snapshot_preview1": wasi_env
		});
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
				run_code: run_code, 
				compile: compile,
				module: obj,
				version: version
			}); //when success
		}).catch(function (reason) {
			reject(reason); //when failure
		});
	});
	
	return result;

	function init_module(module)
	{
		mw_instance = module.instance;
		wasi_env.setEnv(mw_instance, print_func);
		if(mw_instance.exports.version != null){
			let c_str = mw_instance.exports.version();
			version = to_js_str(mw_instance, c_str);
			mw_instance.exports.free(c_str);
		}
	}

	function to_js_str(instance, c_str)
	{
		const str_len = instance.exports.strlen(c_str)	
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

	function setImageData(data, width, height)
	{
		if(set_image_data != null && set_image_data != undefined){
			u8_array = new Uint8ClampedArray(mw_instance.exports.memory.buffer, data, width * height * 4);
			set_image_data(u8_array, width, height);
		}
	}

	function run_wasm_code(instance, wasm, wasm_size)
	{
		let ta = new Uint8Array(instance.exports.memory.buffer, wasm, wasm_size);
		var compiled = new WebAssembly.Module(ta);
		var memory_base = instance.exports.malloc(64 * 1024); //assigned 1 page: 64k
		const __memory_base = new WebAssembly.Global({value: "i32", mutable: false}, memory_base);
		code_instance = new WebAssembly.Instance(compiled, 
			{ 
				sys: 
				{ 
					print: instance.exports.print,
					putchar: instance.exports.putchar,
					memory: instance.exports.memory,
					__memory_base: __memory_base,
					__stack_pointer: instance.exports.__stack_pointer,
					setImageData: setImageData
				},
				math:{
					pow: instance.exports.pow,
					log2: instance.exports.log2,
					log: instance.exports.log
				}
			});
		code_memory_as_array = new Uint8Array(code_instance.exports.memory.buffer);
		var ret = code_instance.exports._start();
		instance.exports.free(memory_base);
		return ret;
	}

	function run_code(code) 
	{
		if (mw_instance == null) {
			print_func("m loading is failed.");
			return;
		}
		let new_ptr = mw_instance.exports.malloc(10*1024);
		str_to_ab(mw_instance, code, new_ptr);
		let wasm = mw_instance.exports.compile_code(new_ptr);
		let wasm_size = mw_instance.exports.get_code_size();
		if(!wasm_size){
			return undefined;
		}
		let result = run_wasm_code(mw_instance, wasm, wasm_size);
		mw_instance.exports.free(wasm);
		return result;
	}

	function compile(code, file_path)
	{
		let new_ptr = mw_instance.exports.malloc(1024);
		str_to_ab(mw_instance, code, new_ptr);
		let wasm = mw_instance.exports.compile_code(new_ptr);
		let wasm_size = mw_instance.exports.get_code_size();
		if(!wasm_size){
			return undefined;
		}
		let ta = new Uint8Array(mw_instance.exports.memory.buffer, wasm, wasm_size);
		const fs = require('fs');
		fs.writeFileSync(file_path, ta, 'binary');
		mw_instance.exports.free(wasm);
	}
}

if (typeof module === 'object') {
	module.exports = mw;
}
