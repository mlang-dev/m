#define EXPORT __attribute__((visibility("default")))
__attribute__((import_module("imports"), import_name("print"))) void print(const char *fmt, ...);
extern unsigned int __stack_pointer;

EXPORT void _start()
{
    double v = 10.0;
    print("hello %.2f", v);
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
