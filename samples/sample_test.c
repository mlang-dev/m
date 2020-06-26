struct Point2D{
    double x;
    double y;
};

double test() 
{
    struct Point2D xy = {10.0, 0.0};
    return xy.x;
}