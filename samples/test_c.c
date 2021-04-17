struct Point2D {
    int x;
    int y;
};

// double f(struct Point2D xy)
// {
//     return xy.x;
// }
struct Point2D f()
{
    struct Point2D point = { 10, 20 };
    return point;
}
