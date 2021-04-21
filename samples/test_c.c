struct Point2D {
    int x;
    int y;
};

int f(struct Point2D xy)
{
    return xy.x;
}

// struct Point2D f()
// {
//     struct Point2D point = { 10.0, 20.0 };
//     return point;
// }
