export type MInstance = {
	mw_instance: WebAssembly.Instance,
	version: string
	run_code: CallableFunction, 
	highlight_code: CallableFunction,
	compile: any,
	canvas_id:string,
	text_id:string,
};

type MExports = {
	malloc:CallableFunction,
	free:CallableFunction,
	compile_code:CallableFunction,
	highlight_code:CallableFunction,
	get_code_size:CallableFunction,
	get_version: CallableFunction,
	strlen:CallableFunction,

	putchar: CallableFunction,
	print: CallableFunction,
	pow: CallableFunction,
	log: CallableFunction,
	log2: CallableFunction,
	memory:WebAssembly.Memory,

	__stack_pointer: any
};

export type RunResult = {
	start_result: any,
	code_instance: WebAssembly.Instance,
	code_bytes: any
};

var m_instance:MInstance;
var m_exports:MExports;
export function mw(wasi_env:any, module_name:string, print_func:CallableFunction, is_remote_file:boolean, set_image_data:CallableFunction|null) : Promise<MInstance> {
	var code_instance = null;
	var print_func = print_func;
	var version:string;
	var module:Promise<WebAssembly.WebAssemblyInstantiatedSource>;
	if (is_remote_file){
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
	let result = new Promise<MInstance>(function (resolve, reject) {
		// "Producing Code" (May take some time)
		module.then(function (obj) {
			m_instance = {
				run_code: run_code, 
				highlight_code: highlight_code,
				compile: compile,
				version: version,
				mw_instance: obj.instance,
				canvas_id: '',
				text_id: ''
			};
			m_exports = {
				malloc: obj.instance.exports.malloc as CallableFunction,
				free: obj.instance.exports.free as CallableFunction,
				compile_code: obj.instance.exports.compile_code as CallableFunction,
				highlight_code: obj.instance.exports.highlight_code as CallableFunction,
				get_code_size: obj.instance.exports.get_code_size as CallableFunction,
				get_version: obj.instance.exports.version as CallableFunction,
				strlen: obj.instance.exports.strlen as CallableFunction,

				putchar: obj.instance.exports.putchar as CallableFunction,
				print: obj.instance.exports.print as CallableFunction,
				pow: obj.instance.exports.pow as CallableFunction,
				log: obj.instance.exports.log as CallableFunction,
				log2: obj.instance.exports.log2 as CallableFunction,

				memory: obj.instance.exports.memory as WebAssembly.Memory,

				__stack_pointer: obj.instance.exports.__stack_pointer
			}
			init_module();
			resolve(m_instance); //when success
		}).catch(function (reason) {
			reject(reason); //when failure
		});
	});
	
	return result;

	function init_module() : void
	{
		wasi_env.setEnv(m_exports.memory, print_func);
		if(m_exports.get_version){
			let c_str = m_exports.get_version();
			version = to_js_str(c_str);
			m_instance.version = version;
		}
	}

	function to_js_str(c_str:any) : string
	{
		const str_len = m_exports.strlen(c_str)	
		let ta = new Uint8Array(m_exports.memory.buffer, c_str, str_len)
		return (new TextDecoder()).decode(ta)
	}

	function str_to_ab(str:string, c_str:any) : void
	{
		let ta = (new TextEncoder()).encode(str) //Uint8Array
		let str_len = ta.length + 1;
		let dst = new Uint8Array(m_exports.memory.buffer, c_str, str_len)
		dst.set(ta);
		dst[ta.length] = 0;
	}

	function setImageData(data:any, width:number, height:number) : void
	{
		if(set_image_data != null && set_image_data != undefined){
			let u8_array = new Uint8ClampedArray(m_exports.memory.buffer, data, width * height * 4);
			set_image_data(u8_array, width, height);
		}
	}

	function run_wasm_code(wasm:any, wasm_size:number) : RunResult
	{
		let ta = new Uint8Array(m_exports.memory.buffer, wasm, wasm_size);
		var compiled = new WebAssembly.Module(ta);
		const __memory_base = new WebAssembly.Global({value: "i32", mutable: false}, 64 * 1024);
		code_instance = new WebAssembly.Instance(compiled, 
			{ 
				sys: 
				{ 
					print: m_exports.print,
					putchar: m_exports.putchar,
					memory: m_exports.memory,
					__memory_base: __memory_base,
					__stack_pointer: m_exports.__stack_pointer,
					setImageData: setImageData
				},
				math:{
					pow: m_exports.pow,
					log2: m_exports.log2,
					log: m_exports.log
				}
			});
		let code_start_func = code_instance.exports._start as CallableFunction;
		return { 
			start_result: code_start_func(),
			code_instance: code_instance,
			code_bytes: wasm
		};
	}

	function run_code(code:string, release_wasm_memory=true) : RunResult | null
	{
		if (!m_instance) {
			print_func("m loading is failed.");
			return null;
		}
		let new_ptr = m_exports.malloc(10*1024);
		str_to_ab(code, new_ptr);
		let wasm = m_exports.compile_code(new_ptr);
		let wasm_size = m_exports.get_code_size();
		if(!wasm_size){
			return null;
		}
		let result = run_wasm_code(wasm, wasm_size);
		if(release_wasm_memory){
			m_exports.free(wasm);
			//result = result.start_result;
		}
		return result;
	}

	function highlight_code(code:string)
	{
		let new_ptr = m_exports.malloc(10*1024);
		str_to_ab(code, new_ptr);
		let highlighted_code = m_exports.highlight_code(new_ptr);
		let hc = to_js_str(highlighted_code);
		m_exports.free(highlighted_code);
		return hc;
	}
	function compile(code:string, file_path:string) : void
	{
		let new_ptr = m_exports.malloc(1024);
		str_to_ab(code, new_ptr);
		let wasm = m_exports.compile_code(new_ptr);
		let wasm_size = m_exports.get_code_size();
		if(!wasm_size){
			return undefined;
		}
		let ta = new Uint8Array(m_exports.memory.buffer, wasm, wasm_size);
		const fs = require('fs');
		fs.writeFileSync(file_path, ta, 'binary');
		m_exports.free(wasm);
	}
}
