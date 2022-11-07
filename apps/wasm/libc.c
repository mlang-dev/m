#include "wasm/libc.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *_malloc(size_t bytes)
{
    return malloc(bytes);
}

void _free(void *data)
{
    free(data);
}

i32 _strlen(const char *chars)
{
    return strlen(chars);
}

void print(const char *restrict fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    va_end(ap);
}

void _putchar(int ch)
{
    putchar(ch);
}

f64 _acos(f64 x)
{
    return acos(x);
}

f64 _asin(f64 x)
{
    return asin(x);
}

f64 _atan(f64 x)
{
    return atan(x);
}

f64 _atan2(f64 y, f64 x)
{
    return atan2(y, x);
}

f64 _cos(f64 x)
{
    return cos(x);
}

f64 _sin(f64 x)
{
    return sin(x);
}

f64 _sinh(f64 x)
{
    return sinh(x);
}

f64 _tanh(f64 x)
{
    return tanh(x);
}

f64 _exp(f64 x)
{
    return exp(x);
}

f64 _log(f64 x)
{
    return log(x);
}

f64 _log10(f64 x)
{
    return log10(x);
}

f64 _pow(f64 x, f64 y)
{
    return pow(x, y); //x ^ y
}

f64 _sqrt(f64 x)
{
    return sqrt(x);
}
