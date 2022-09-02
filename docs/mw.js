function mw(wasi_env, module_name, print_func, remote_file) {
	var mw_instance = null;  //m compiler instance
	var code_instance = null;
	var code_memory_as_array = null;
	var print_func = print_func;
	var version = null;

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
			mw_instance.exports.free_mem(c_str);
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

	function _decode_uint(buffer, offset)
	{
		result = 0;
		shift = 0;
		while(true){
			byte = buffer[offset++];
			result |= (byte & 0x7F) << shift;
			if ((byte & 0x80) == 0){
				break;
			}
			shift += 7;
		}
		return [offset, result];
	}

	function print_log(fmt_offset, ...params)
	{
		[fmt_offset, fmt_length] = _decode_uint(code_memory_as_array, fmt_offset);
		var bytes = new Uint8Array(code_instance.exports.memory.buffer, fmt_offset, fmt_length);
		var string = new TextDecoder('utf8').decode(bytes);
		print_func(string);
	}

	function run_wasm_code(instance, wasm, wasm_size)
	{
		let ta = new Uint8Array(instance.exports.memory.buffer, wasm, wasm_size);
		var compiled = new WebAssembly.Module(ta);
		code_instance = new WebAssembly.Instance(compiled, 
			{ imports: 
				{ 
					print: print_log,
				}
			});
		code_memory_as_array = new Uint8Array(code_instance.exports.memory.buffer);
		return code_instance.exports._start();
	}

	function run_mcode(code) 
	{
		if (mw_instance == null) {
			print_func("m loading is failed.");
			return;
		}
		let new_ptr = mw_instance.exports.alloc_mem(1024);
		str_to_ab(mw_instance, code, new_ptr);
		let wasm = mw_instance.exports.compile_code(new_ptr);
		let wasm_size = mw_instance.exports.get_code_size();
		let result = run_wasm_code(mw_instance, wasm, wasm_size);
		mw_instance.exports.free_mem(wasm);
		return result;
	}

	function compile(code, file_path)
	{
		let new_ptr = mw_instance.exports.alloc_mem(1024);
		str_to_ab(mw_instance, code, new_ptr);
		let wasm = mw_instance.exports.compile_code(new_ptr);
		let wasm_size = mw_instance.exports.get_code_size();
		let ta = new Uint8Array(mw_instance.exports.memory.buffer, wasm, wasm_size);
		const fs = require('fs');
		fs.writeFileSync(file_path, ta, 'binary');
		mw_instance.exports.free_mem(wasm);
	}
}

if (typeof module === 'object') {
	module.exports = mw;
}
