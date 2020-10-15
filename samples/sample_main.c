#include <stdio.h>
struct Point2D {double a; double b; double c; double x; double y; double z;};
extern struct Point2D test();

int main() {
    struct Point2D x = {10.0, 20.0, 30.0, 40.0, 50.0, 60.0};
    struct Point2D y = {10.0, 20.0, 30.0, 40.0, 50.0, 60.0};
    printf("hello: %f", test(x, y).x);
    return 0;
}