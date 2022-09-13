
#include <complex.h>
#include <math.h>


#define EXPORT __attribute__((visibility("default")))
__attribute__((import_module("imports"), import_name("print"))) void print(const char *fmt, ...);
double complex _get_complex()
{
    double complex d = 1.0 + 2.0i;
    return d;
}

EXPORT void _start()
{
    double complex z = _get_complex();
    print("hello: %f, %f", creal(z), cimag(z));
}
// struct Point2D {
//     double x;
//     double y;
// };

// struct Point2D f()
// {
//     struct Point2D xy = { 10.0, 20.0 };
//     return xy;
// }

// struct Point2D f()
// {
//     struct Point2D point = { 10.0, 20.0 };
//     return point;
// }
