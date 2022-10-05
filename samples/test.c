
#include <complex.h>
#include <math.h>
#include <stdbool.h>


#define EXPORT __attribute__((visibility("default")))
__attribute__((import_module("imports"), import_name("print"))) void print(const char *fmt, ...);
struct abc{
    int a;
    int b;
};

int _get_int()
{
    return 100;
}

struct abc _get_struct()
{
    struct abc a = {true, false};
    return a;
}

double complex _get_complex()
{
    return 100 + 200i;
}

double _get_double()
{
    return 1.0;
}

EXPORT void _start()
{
    struct abc x;
    x.a = 100;
    x.b = 200;
    print("hello: %d, %d", x.a, x.b);

    struct abc y;
    x.a = 300;
    x.b = 400;
    print("world: %d, %d, %d, %d", x.a, x.b, y.a, y.b);
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
