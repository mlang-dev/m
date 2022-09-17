
#include <complex.h>
#include <math.h>
#include <stdbool.h>


#define EXPORT __attribute__((visibility("default")))
__attribute__((import_module("imports"), import_name("print"))) void print(const char *fmt, ...);
struct abc{
    bool a;
    bool b;
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
    char i = 'a';
    int j = 100;
    print("hello: %c, %d", i, j);
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
