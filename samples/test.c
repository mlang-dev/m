#define EXPORT __attribute__((visibility("default")))
__attribute__((import_module("imports"), import_name("print"))) int print(const char *fmt, ...);

EXPORT int _start()
{
    int x = 10;
    return print("%d", x);
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
