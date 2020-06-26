#include <stdio.h>

extern double test();

int main() {
    printf("hello: %f", test());
    return 0;
}