#define EXPORT __attribute__((visibility("default")))



EXPORT int _start()
{
    int x = 10;
    return x;
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
