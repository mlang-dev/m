#include <iostream>

extern "C" {
    double avg(double, double);
    double distance(double, double, double, double);
    int factorial(int);
    int loopprint(int);
}

int main() {
    printf("average of 13.0 and 14.0: %f\n", avg(13.0, 14.0));
    printf("distance between (1, 2) and (4,6): %f\n", distance(1, 2, 4, 6));
    printf("factorial of 5!: %d\n", factorial(5));
    loopprint(10);
    printf("\n");
    return 0;
}