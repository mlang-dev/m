struct Point2D {
    double x;
    double y;
};

double f(struct Point2D xy)
{
    return xy.x;
}
// struct Point2D f()
// {
//     struct Point2D point = { 10.0, 20.0 };
//     return point;
// }
