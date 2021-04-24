struct Point2D {
    double x;
    double y;
};

struct Point2D f(struct Point2D *xy)
{
    struct Point2D *ptr = xy;
    ptr->x = 12.0;
    return *ptr;
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
