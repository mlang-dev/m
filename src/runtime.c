/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language runtime
 */
#include "runtime.h"
#include <stdio.h>

double print(double x)
{
    printf("%f\n", x);
    return x;
}
