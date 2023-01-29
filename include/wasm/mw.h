#include "wasm/libc.h"

wasm_export_name(version) const char *version();
wasm_export_name(compile_code) u8 *compile_code(const char *text);
wasm_export_name(highlight_code) u8 *highlight_code(const char *text);
wasm_export_name(get_code_size) u32 get_code_size();
