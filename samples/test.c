#define EXPORT __attribute__((visibility("default")))

EXPORT int run()
{
    return 10 + 20;
}

EXPORT int _start()
{
    return run();
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
