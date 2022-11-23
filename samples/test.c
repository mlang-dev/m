
union XY{
    int x;
    double y;
};

#define EXPORT __attribute__((visibility("default")))
__attribute__((import_module("imports"), import_name("print"))) void print(const char *fmt, ...);

EXPORT void _start()
{
    union XY xy = { 0 };
    print("hello: %f", xy.y);
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
