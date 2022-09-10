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

double _acos(double x)
{
    return acos(x);
}

double _asin(double x)
{
    return asin(x);
}

double _atan(double x)
{
    return atan(x);
}

double _atan2(double y, double x)
{
    return atan2(y, x);
}

double _cos(double x)
{
    return cos(x);
}

double _sin(double x)
{
    return sin(x);
}

double _sinh(double x)
{
    return sinh(x);
}

double _tanh(double x)
{
    return tanh(x);
}

double _exp(double x)
{
    return exp(x);
}

double _log(double x)
{
    return log(x);
}

double _log10(double x)
{
    return log10(x);
}

double _pow(double x, double y)
{
    return pow(x, y); //x ^ y
}

double _sqrt(double x)
{
    return sqrt(x);
}
