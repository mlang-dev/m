#include <stdio.h>

struct Point2D {
    double x;
    double y;
};
extern struct Point2D xy;
extern void test();

int main() {
    test();
    printf("hello: %f", xy.x);
    return 0;
}