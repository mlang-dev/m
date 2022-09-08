#include "clib/typedef.h"
#include <stddef.h>

#define wasm_export_name(name) __attribute__((export_name(#name)))

/*libc*/
wasm_export_name(print) void print(const char *fmt, ...);
wasm_export_name(fprint) void _fprint(const char *fmt, ...);
wasm_export_name(malloc) void *_malloc(size_t bytes);
wasm_export_name(free) void _free(void *data);
wasm_export_name(strlen) i32 _strlen(const char *chars);

/*math*/
wasm_export_name(acos) double _acos(double x);
wasm_export_name(asin) double _asin(double x);
wasm_export_name(atan) double _atan(double x);
wasm_export_name(atan2) double _atan2(double y, double x);
wasm_export_name(cos) double _cos(double x);
wasm_export_name(sin) double _sin(double);
wasm_export_name(sinh) double _sinh(double x);
wasm_export_name(tanh) double _tanh(double x);
wasm_export_name(exp) double _exp(double x);
wasm_export_name(log) double _log(double x);
wasm_export_name(log10) double _log10(double x);
wasm_export_name(pow) double _pow(double x, double y);
wasm_export_name(sqrt) double _sqrt(double x);
