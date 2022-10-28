
#include <complex.h>
#include <math.h>
#include <stdbool.h>


#define EXPORT __attribute__((visibility("default")))
__attribute__((import_module("imports"), import_name("print"))) void print(const char *fmt, ...);

EXPORT void _start()
{
    double i = 10;
    double *j = &i;
    print("hello: %d, %p", i, j);
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
