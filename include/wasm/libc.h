#include "clib/typedef.h"
#include <stddef.h>

#ifdef NATIVE_APP
#define wasm_export_name(name)
#else
#define wasm_export_name(name) __attribute__((export_name(#name)))
#endif

/*libc*/
/*io*/
wasm_export_name(print) void print(const char *fmt, ...);
wasm_export_name(putchar) void _putchar(int);

wasm_export_name(malloc) void *_malloc(size_t bytes);
wasm_export_name(free) void _free(void *data);
wasm_export_name(strlen) i32 _strlen(const char *chars);

/*math*/
wasm_export_name(acos) f64 _acos(f64 x);
wasm_export_name(asin) f64 _asin(f64 x);
wasm_export_name(atan) f64 _atan(f64 x);
wasm_export_name(atan2) f64 _atan2(f64 y, f64 x);
wasm_export_name(cos) f64 _cos(f64 x);
wasm_export_name(sin) f64 _sin(f64);
wasm_export_name(sinh) f64 _sinh(f64 x);
wasm_export_name(tanh) f64 _tanh(f64 x);
wasm_export_name(exp) f64 _exp(f64 x);
wasm_export_name(log) f64 _log(f64 x);
wasm_export_name(log10) f64 _log10(f64 x);
wasm_export_name(log2) f64 _log2(f64 x);
wasm_export_name(pow) f64 _pow(f64 x, f64 y);
wasm_export_name(sqrt) f64 _sqrt(f64 x);
