#define EXPORT __attribute__((visibility("default")))

EXPORT int id(int x)
{
    return x;
}

EXPORT int _start()
{
    return id(10);
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
