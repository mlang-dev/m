struct Point2D{
    double a;
    double b;
    double c;
    double x;
    double y;
    double z;
};

struct Point2D test(struct Point2D x, struct Point2D y) 
{
    struct Point2D z;
    z.a = x.a + y.a;
    z.b = x.b + y.b;
    z.c = x.c + y.c;
    z.x = x.x + y.x;
    z.y = x.y + y.y;
    z.z = x.z + y.z;
    return z;
}